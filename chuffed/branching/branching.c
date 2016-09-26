#include <chuffed/branching/branching.h>
#include <chuffed/vars/vars.h>
#include <chuffed/core/engine.h>
#include <chuffed/core/options.h>

BranchGroup::BranchGroup(VarBranch vb, bool t) :
	var_branch(vb), terminal(t), fin(0), cur(-1) {}

BranchGroup::BranchGroup(vec<Branching*>& _x, VarBranch vb, bool t) :
	x(_x), var_branch(vb), terminal(t), fin(0), cur(-1) {}

bool BranchGroup::finished() {
	if (fin) return true;
	for (int i = 0; i < x.size(); i++) {
		if (!x[i]->finished()) return false;
	}
	fin = 1;
	return true;
}

double BranchGroup::getScore(VarBranch vb) {
	double sum = 0;
	for (int i = 0; i < x.size(); i++) {
		sum += x[i]->getScore(vb);
	}
	return sum / x.size();
}

DecInfo* BranchGroup::branch() {
	// If we're working on a group and it's not finished, continue there
	if (cur >= 0 && !x[cur]->finished()) return x[cur]->branch();

//	printf("bra");

	// Need to find new group
	if (var_branch == VAR_INORDER) {
		int i = 0;
		while (i < x.size() && x[i]->finished()) i++;
		if (i == x.size()) {
//			assert(is_top_level_branching);
			return NULL;
		}
		if (!terminal) cur = i;
		return x[i]->branch();
	}

	double best = -1e100;
	moves.clear();
	for (int i = 0; i < x.size(); i++) {
		if (x[i]->finished()) continue;
		double s = x[i]->getScore(var_branch);
		if (s >= best) {
			if (s > best) {
				best = s;
				moves.clear();
			}
			moves.push(i);
		}
	}
	if (moves.size() == 0) {
//		assert(is_top_level_branching);
		return NULL;
	}
	int best_i = moves[0];
	if (so.branch_random) best_i = moves[rand()%moves.size()];

//	printf("best = %.2f\n", best);
//	printf("%d: %d ", engine.decisionLevel(), best_i);

	if (!terminal) cur = best_i;
	return x[best_i]->branch();
}


void branch(vec<Branching*> x, VarBranch var_branch, ValBranch val_branch) {
	engine.branching->add(new BranchGroup(x, var_branch, true));
	if (val_branch == VAL_DEFAULT) return;
	PreferredVal p;
	switch (val_branch) {
		case VAL_MIN: p = PV_MIN; break;
		case VAL_MAX: p = PV_MAX; break;
		case VAL_SPLIT_MIN: p = PV_SPLIT_MIN; break;
		case VAL_SPLIT_MAX: p = PV_SPLIT_MAX; break;
		default: NEVER;
	}
	for (int i = 0; i < x.size(); i++) ((Var*) x[i])->setPreferredVal(p);
}
