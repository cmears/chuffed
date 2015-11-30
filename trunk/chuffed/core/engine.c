#include <cstdio>
#include <cassert>

#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>

#include <chuffed/core/options.h>
#include <chuffed/core/engine.h>
#include <chuffed/core/sat.h>
#include <chuffed/core/propagator.h>
#include <chuffed/branching/branching.h>
#include <chuffed/mip/mip.h>
#include <chuffed/parallel/parallel.h>
#include <chuffed/ldsb/ldsb.h>

#include <chuffed/flatzinc/flatzinc.h>

#include "cpp-integration/connector.hh"

#define DEBUG_VERBOSE 0

using namespace Profiling;

Engine engine;

uint64_t bit[65];

Tint trail_inc;

int nextnodeid = 0;
std::vector<int> nodepath;


std::vector<int> altpath;
std::vector<int> decisionLevelTip;
int restartCount;

Profiling::Connector c(6565);
std::map<IntVar*, string> intVarString;
        string mostRecentLabel;

Engine::Engine() :
		finished_init(false)
	, problem(NULL)
	, opt_var(NULL)
	, best_sol(-1)
	, last_prop(NULL)

	, start_time(wallClockTime())
	, opt_time(0)
	, conflicts(0)
	, nodes(1)
	, propagations(0)
	, solutions(0)
	, next_simp_db(0)
{
	p_queue.growTo(num_queues);
	for (int i = 0; i < 64; i++) bit[i] = ((long long) 1 << i);
	branching = new BranchGroup();
	mip = new MIP();
}

inline void Engine::newDecisionLevel() {
	trail_inc++;
	trail_lim.push(trail.size());
	sat.newDecisionLevel();
	if (so.mip) mip->newDecisionLevel();
	assert(dec_info.size() == decisionLevel());
}

inline void Engine::doFixPointStuff() {
	// ask other objects to do fix point things
	for (int i = 0; i < pseudo_props.size(); i++) {
		pseudo_props[i]->doFixPointStuff();
	}
}

inline void Engine::makeDecision(DecInfo& di, int alt) {
	++nodes;
        altpath.push_back(alt);
	if (di.var) {
#if DEBUG_VERBOSE
          std::cerr << "makeDecision: " << intVarString[(IntVar*)di.var] << " / " << di.val << " (" << alt << ")" << std::endl;
#endif
          std::stringstream ss;
          ss << intVarString[(IntVar*)di.var] << " / " << di.val << " (" << alt << ")";
          mostRecentLabel = ss.str();
            ((IntVar*) di.var)->set(di.val, di.type ^ alt);
        } else {
#if DEBUG_VERBOSE
            std::cerr << "enqueing SAT literal: " << di.val << "^" << alt << " = " << (di.val ^ alt) << std::endl;
#endif
          std::stringstream ss;
          ss << litString[di.val^alt];
          mostRecentLabel = ss.str();
            sat.enqueue(toLit(di.val ^ alt));
        }
	if (so.ldsb && di.var && di.type == 1) ldsb.processDec(sat.trail.last()[0]);
//	if (opt_var && di.var == opt_var && ((IntVar*) di.var)->isFixed()) printf("objective = %d\n", ((IntVar*) di.var)->getVal());
}

void optimize(IntVar* v, int t) {
	engine.opt_var = v;
	engine.opt_type = t;
	engine.branching->add(v);
	v->setPreferredVal(t == OPT_MIN ? PV_MIN : PV_MAX);

}

inline bool Engine::constrain() {
	best_sol = opt_var->getVal();
	opt_time = wallClockTime() - start_time - init_time;

	sat.btToLevel(0);
        restartCount++;
        nodepath.resize(0);
        altpath.resize(0);
        nextnodeid = 0;
        c.restart("chuffed", restartCount);

	if (so.parallel) {
		Lit p = opt_type ? opt_var->getLit(best_sol+1, 2) : opt_var->getLit(best_sol-1, 3);
		vec<Lit> ps;
		ps.push(p);
		Clause *c = Clause_new(ps, true);
		slave.shareClause(*c);
		free(c);
	}

//	printf("opt_var = %d, opt_type = %d, best_sol = %d\n", opt_var->var_id, opt_type, best_sol);
//	printf("opt_var min = %d, opt_var max = %d\n", opt_var->min, opt_var->max);

	if (so.mip) mip->setObjective(best_sol);

	return (opt_type ? opt_var->setMin(best_sol+1) : opt_var->setMax(best_sol-1));
}

bool Engine::propagate() {
	if (async_fail) {
		async_fail = false;
		assert(!so.lazy || sat.confl);
		return false;
	}

	last_prop = NULL;

	WakeUp:

	if (!sat.consistent() && !sat.propagate()) return false;

	for (int i = 0; i < v_queue.size(); i++) {
		v_queue[i]->wakePropagators();
	}
	v_queue.clear();

	if (sat.confl) return false;

	last_prop = NULL;

	for (int i = 0; i < num_queues; i++) {
		if (p_queue[i].size()) {
			Propagator *p = p_queue[i].last(); p_queue[i].pop();
			propagations++;
			bool ok = p->propagate();
			p->clearPropState();
			if (!ok) return false;
			goto WakeUp;
		}
	}

	return true;
}

// Clear all uncleared intermediate propagation states
void Engine::clearPropState() {
	for (int i = 0; i < v_queue.size(); i++) v_queue[i]->clearPropState();
	v_queue.clear();

	for (int i = 0; i < num_queues; i++) {
		for (int j = 0; j < p_queue[i].size(); j++) p_queue[i][j]->clearPropState();
		p_queue[i].clear();
	}
}

void Engine::btToPos(int pos) {
	for (int i = trail.size(); i-- > pos; ) {
		trail[i].undo();
	}
  trail.resize(pos);
}

void Engine::btToLevel(int level) {
#if DEBUG_VERBOSE
    std::cerr << "Engine::btToLevel( " << level << ")\n";
#endif
	if (decisionLevel() == 0 && level == 0) return;
	assert(decisionLevel() > level);

	btToPos(trail_lim[level]);
  trail_lim.resize(level);
	dec_info.resize(level);
}



void Engine::topLevelCleanUp() {
	trail.clear();

	if (so.fd_simplify && propagations >= next_simp_db) simplifyDB();

	sat.topLevelCleanUp();
}

void Engine::simplifyDB() {
	int cost = 0;
	for (int i = 0; i < propagators.size(); i++) {
		cost += propagators[i]->checkSatisfied();
	}
	cost += propagators.size();
	for (int i = 0; i < vars.size(); i++) {
		cost += vars[i]->simplifyWatches();
	}
	cost += vars.size();
	cost *= 10;
//	printf("simp db cost: %d\n", cost);
	next_simp_db = propagations + cost;
}

void Engine::blockCurrentSol() {
	if (outputs.size() == 0) NOT_SUPPORTED;
	Clause& c = *Reason_new(outputs.size());
	bool root_failure = true;
	for (int i = 0; i < outputs.size(); i++) {
		Var *v = (Var*) outputs[i];
		if (v->getType() == BOOL_VAR) {
			c[i] = ((BoolView*) outputs[i])->getValLit();
		} else {
			c[i] = ((IntVar*) outputs[i])->getValLit();
		}
		if (!sat.isRootLevel(var(c[i]))) root_failure = false;
		assert(sat.value(c[i]) == l_False);
	}
	if (root_failure) sat.btToLevel(0);
	sat.confl = &c;
}


int Engine::getRestartLimit(int starts) {
//	return so.restart_base * ((int) pow(1.5, starts));
//	return so.restart_base;
	return (((starts-1) & ~starts) + 1) * so.restart_base;
}

void Engine::toggleVSIDS() {
	if (!so.vsids) {
		vec<Branching*> old_x;
		branching->x.moveTo(old_x);
		branching->add(&sat);
		for (int i = 0; i < old_x.size(); i++) branching->add(old_x[i]);
		branching->fin = 0;
		branching->cur = -1;
		so.vsids = true;
	} else {
		vec<Branching*> old_x;
		branching->x.moveTo(old_x);
		for (int i = 1; i < old_x.size(); i++) branching->add(old_x[i]);
		branching->fin = 0;
		branching->cur = -1;
		so.vsids = false;
	}
}

RESULT Engine::search() {
	int starts = 0;
	int nof_conflicts = so.restart_base;
	int conflictC = 0;

        restartCount = 0;
        c.restart("chuffed", restartCount);

        decisionLevelTip.push_back(1);

	while (true) {
		if (so.parallel && slave.checkMessages()) return RES_UNK;

                int nodeid = nextnodeid;
                nextnodeid++;
                int parent = (nodepath.size() == 0) ? (-1) : (nodepath[nodepath.size()-1]);
                nodepath.push_back(nodeid);
                int myalt = (altpath.size() == 0) ? (-1) : (altpath[altpath.size()-1]);
#if DEBUG_VERBOSE
                std::cerr << "propagate (";
                for (int i = 0 ; i < nodepath.size() ; i++)
                    std::cerr << " " << nodepath[i];
                std::cerr << ")\n";
                std::cerr << "altpath (";
                for (int i = 0 ; i < altpath.size() ; i++)
                    std::cerr << " " << altpath[i];
                std::cerr << ")\n";
#endif
                    if (decisionLevel() >= decisionLevelTip.size())
                      decisionLevelTip.resize(decisionLevel()+1);
                    decisionLevelTip[decisionLevel()] = nodepath.size();
#if DEBUG_VERBOSE
                    std::cerr << "setting decisionLevelTip[" << decisionLevel() << "] to " << nodepath.size() << "\n";
#endif

                    if (!propagate()) {
#if DEBUG_VERBOSE
                    std::cerr << "failure\n";
                    std::cerr << "createNode(" << nodeid << ", " << parent << ", " << myalt << ", 0, NodeStatus::FAILED)\n";
#endif
                    /* c.createNode(nodeid, parent, myalt, 0, FAILED).set_label(mostRecentLabel).send(); */
                    /* mostRecentLabel = ""; */

			clearPropState();

			Conflict:
			conflicts++; conflictC++;

			if (time(NULL) > so.time_out) {
				printf("Time limit exceeded!\n");
				return RES_UNK;
			}

			if (decisionLevel() == 0) {
                          c.createNode(nodeid, parent, myalt, 0, FAILED).set_label(mostRecentLabel).set_restart_id(restartCount).send();
                          mostRecentLabel = "";
                          return RES_GUN; }
                    

			// Derive learnt clause and perform backjump
			if (so.lazy) {
				sat.analyze();
                                std::stringstream ss;
                                ss << "out_learnt (interpreted):";
                                for (int i = 0 ; i < sat.out_learnt.size() ; i++)
                                  ss << " " << litString[toInt(sat.out_learnt[i])];
                                c.createNode(nodeid, parent, myalt, 0, FAILED).set_label(mostRecentLabel).set_info(ss.str()).set_restart_id(restartCount).send();
                                mostRecentLabel = "";
#if DEBUG_VERBOSE
                                std::cerr << "after analyze, decisionLevel() is " << decisionLevel() << "\n";
                                std::cerr << "decisionLevelTip:";
                                for (int i = 0 ; i < decisionLevelTip.size() ; i++)
                                  std::cerr << " " << decisionLevelTip[i];
                                std::cerr << "\n";
#endif
                                nodepath.resize(decisionLevelTip[decisionLevel()]);
                                altpath.resize(decisionLevelTip[decisionLevel()]-1);
                                std::stringstream ss2;
                                ss2 << "-> ";
                                string ls = litString[toInt(sat.out_learnt[0])];
                                ss2 << ls;
                                if (ls.size() < 2) {
                                    std::cerr << "WARNING: short label for " << toInt(sat.out_learnt[0]) << ": " << ls << "\n";
                                    }
                                mostRecentLabel = ss2.str();
                                altpath.push_back(1);
			}	else {
                                c.createNode(nodeid, parent, myalt, 0, FAILED).set_label(mostRecentLabel).set_restart_id(restartCount).send();
                                mostRecentLabel = "";
				sat.confl = NULL;
				DecInfo& di = dec_info.last();
				sat.btToLevel(decisionLevel()-1);
                                nodepath.resize(decisionLevelTip[decisionLevel()]);
                                altpath.resize(decisionLevelTip[decisionLevel()]-1);
                                /* while (altpath.size() > 0 && altpath.back() == 1) { */
                                /*     nodepath.pop_back(); */
                                /*     altpath.pop_back(); */
                                /* } */
                                /* nodepath.pop_back(); */
                                /* altpath.pop_back(); */
				makeDecision(di, 1);
			}

            if (!so.vsids && !so.toggle_vsids &&  conflictC >= so.switch_to_vsids_after) {
	    	if (so.restart_base >= 1000000000) so.restart_base = 100;
                sat.btToLevel(0);
        restartCount++;
        nodepath.resize(0);
        altpath.resize(0);
        nextnodeid = 0;
        c.restart("chuffed", restartCount);

                toggleVSIDS();
            }

		} else {

			if (conflictC >= nof_conflicts) {
				starts++;
				nof_conflicts += getRestartLimit((starts+1)/2);
				sat.btToLevel(0);
        restartCount++;
        nodepath.resize(0);
        altpath.resize(0);
        nextnodeid = 0;
        c.restart("chuffed", restartCount);


				sat.confl = NULL;
				if (so.lazy && so.toggle_vsids && (starts % 2 == 0)) toggleVSIDS();
				continue;
			}
			
			if (decisionLevel() == 0) topLevelCleanUp();

			DecInfo *di = NULL;
			
			// Propagate assumptions
			while (decisionLevel() < assumptions.size()) {
#if DEBUG_VERBOSE
                            std::cerr << "doing something with assumptions\n";
#endif
				int p = assumptions[decisionLevel()];
				if (sat.value(toLit(p)) == l_True) {
					// Dummy decision level:
					assert(sat.trail.last().size() == sat.qhead.last());
					engine.dec_info.push(DecInfo(NULL, p));
					newDecisionLevel();
				} else if (sat.value(toLit(p)) == l_False) {
					return RES_LUN;
				} else {
					di = new DecInfo(NULL, p);
					break;
				}
			}

			if (!di) di = branching->branch();

			if (!di) {
				solutions++;
				if (so.print_sol) {
					problem->print();
					printf("----------\n");
          fflush(stdout);
				}
#if DEBUG_VERBOSE
                                    std::cerr << "solution\n";
                                    std::cerr << "createNode(" << nodeid << ", " << parent << ", " << myalt << ", 0, SOLVED)\n";
#endif

                                    FlatZinc::FlatZincSpace *fzs = dynamic_cast<FlatZinc::FlatZincSpace*>(problem);
                                    if (fzs != NULL) {
                                        std::stringstream s;
                                        fzs->printStream(s);
                                        c.createNode(nodeid, parent, myalt, 0, SOLVED)
                                            .set_label(mostRecentLabel)
                                            .set_info(s.str())
                                            .set_restart_id(restartCount).send();
                                    } else {
                                        c.createNode(nodeid, parent, myalt, 0, SOLVED)
                                            .set_label(mostRecentLabel)
                                            .set_restart_id(restartCount).send();
                                    }
                                    
                                    mostRecentLabel = "";
				if (!opt_var) {
					if (solutions == so.nof_solutions) return RES_SAT;
					if (so.lazy) blockCurrentSol();
					goto Conflict;
				}
				if (!constrain()) {
					return RES_GUN;
				}
				continue;
			}


			engine.dec_info.push(*di);
      newDecisionLevel();

			doFixPointStuff();

#if DEBUG_VERBOSE
                        std::cerr << "createNode(" << nodeid << ", " << parent << ", " << myalt << ", 2, NodeStatus::BRANCH)\n";
#endif
                        c.createNode(nodeid, parent, myalt, 2, BRANCH).set_label(mostRecentLabel).set_restart_id(restartCount).send();
                        mostRecentLabel = "";
                        
			makeDecision(*di, 0);

			delete di;

		}
	}
}

void Engine::solve(Problem *p) {
	problem = p;

	init();

	so.time_out += time(NULL);

	init_time = wallClockTime() - start_time;
	base_memory = memUsed();

        c.connect();

	if (!so.parallel) {
		// sequential
		status = search();
		if (status == RES_GUN) {
			if (solutions > 0)
				printf("==========\n");
			else
				printf("=====UNSATISFIABLE=====\n");
		}
	} else {
		// parallel
		if (so.thread_no == -1) master.solve();
		else slave.solve();
		if (so.thread_no == -1 && master.status == RES_GUN) printf("==========\n");
	}

        c.done();
        c.disconnect();

	if (so.verbosity >= 1) printStats();

  if (so.parallel) master.finalizeMPI();
}
