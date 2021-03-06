#ifndef options_h
#define options_h

#include <cstdlib>
#include <cstring>
#include <string>
#include <chuffed/support/misc.h>

// When enabled, there is a command-line option -phase_saving=n, defaults to
// 0=none (1=some, 2=always, behaviour is exactly the same as MiniSat 2.2.0).
#define PHASE_SAVING 0

#define DEBUG_VERBOSE 0

class Options {
public:
	// Solver options
	int nof_solutions;               // Number of solutions to find
	int time_out;                    // Amount of time before giving up
	int rnd_seed;                    // Random seed
	int verbosity;                   // Verbosity
	bool print_sol;                  // Print solutions
	int restart_base;                // How many conflicts before restart

	// Search options
	bool toggle_vsids;               // Alternate between search ann/vsids
	bool branch_random;              // Use randomization for tie-breaking
    int switch_to_vsids_after;       // Switch from search ann to vsids after a given number of conflicts
	int sat_polarity;                // Polarity of bool var to choose (0 = default, 1 = same, 2 = anti, 3 = random)

	// Propagator options
	bool prop_fifo;                  // Propagators are queued in FIFO, otherwise LIFO

	// Disjunctive propagator options
	bool disj_edge_find;             // Use edge finding
	bool disj_set_bp;                // Use set bounds propagation

	// Cumulative propagator options
	bool cumu_global;		 // Use the global cumulative propagator

	// Preprocessing options
	bool sat_simplify;               // Simplify clause database at top level
	bool fd_simplify;                // Simplify FD propagators at top level

	// Lazy clause options
	bool lazy;                       // Use lazy clause
	bool finesse;                    // Get better explanations sometimes
	bool learn;                      // Learn clauses
	bool vsids;                      // Use VSIDS as branching heuristic
#if PHASE_SAVING
	int phase_saving;                // Repeat same variable polarity (0=no, 1=recent, 2=always)
#endif
	bool sort_learnt_level;          // Sort lits in learnt clause based on level
	bool one_watch;                  // One watch learnt clauses

        bool exclude_introduced;         // Exclude introduced variables from learnt clauses
        bool decide_introduced;          // Search on introduced variables
        bool introduced_heuristic;       // Use name of variable to decide whether a variable is introduced
        bool use_var_is_introduced;      // Use var_is_introduced annotation to decide
                                         // whether a variable is introduced
        bool print_nodes;                // Print nodes as they are sent to the profiler (or
                                         // would be sent)
        bool print_implications;
        bool print_variable_list;
        bool send_skipped;               // Send skipped nodes?
        bool send_domains;               // Compute and send variable domains
        std::string filter_domains;
        bool learnt_stats;
        bool learnt_stats_nogood;
        bool debug;                      // Produce debug output
        bool exhaustive_activity;

        bool bin_clause_opt;          // Should length-2 learnt clauses be optimised?
        
	int eager_limit;                 // Max var range before we use lazy lit generation
	int sat_var_limit;               // Max number of sat vars before turning off lazy clause
	int nof_learnts;                 // Learnt clause no. limit
	int learnts_mlimit;              // Learnt clause mem limit

	// Language of explanation extension options
	bool lang_ext_linear;
    
    // MDD options 
	bool mdd;                        // Use MDD propagator

	// MIP options
	bool mip;                        // Use MIP propagator
	bool mip_branch;                 // Use MIP branching

	// Sym break options
	bool sym_static;
	bool ldsb;                       // Use lightweight dynamic symmetry breaking 1UIP crippled
	bool ldsbta;                     // Use lightweight dynamic symmetry breaking 1UIP
	bool ldsbad;                     // Use lightweight dynamic symmetry breaking all decision clause

	// Well founded semantics options
	bool well_founded;

	// Parallel options
	bool parallel;                   // Running in parallel mode
	int num_threads;                 // Number of worker threads
	int thread_no;                   // Thread number of this thread
	double share_param;              // Parameter for controlling which clauses are shared
	double bandwidth;                // How many lits per second we can share, counting all threads
	int trial_size;                  // Number of shared clauses put on trial (temp. immune to pruning)
	int share_act;                   // How to share clause activities between threads (0 = none, 1 = act, 2 = react)
	int num_cores;                   // Number of cpu cores in machine

	// Experimental
	int saved_clauses;
	bool use_uiv;

	// for Nick's test (defaults probably work best in practice)
	bool alldiff_cheat;              // if n vars over n vals, enforce that all vals are taken
	bool alldiff_stage;              // if bounds or domain consistency, put value propagator too

	Options();

};

extern Options so;

void parseOptions(int& argc, char**& argv);

#endif
