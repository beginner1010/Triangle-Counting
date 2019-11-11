#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <sstream>

#include "IO.h"

#define mmax(a,b) ((a)>(b)?(a):(b))
#define mmin(a,b) ((a)<(b)?(a):(b))

#define EXACT "Exact algorithm -- Wang et al. Vertex Priority Based Butterfly Counting for Large-scale Bipartite Networks. arXiv 2018"
#define ONE_SHOT_DOULION "(one shot) Edge Sparsification Algorithm  -- (inspired by) Tsourakakis et al. Doulion: counting triangles in massive graphs with a coin. KDD 2009."
#define ONE_SHOT_COLORFUL "(one shot) Colorful Sparsification Algorithm -- (inspired by) Pagh & Tsourakakis. Colorful triangle counting. IPL 2012."
#define FAST_EDGE_SAMPLING "(local sampling) Vanilla Fast Edge Sampling -- Sanei-Mehri et al., Butterfly Counting in Bipartite Networks. KDD 2018"
#define FAST_CENTERED_EDGE_SAMPLING "(local sampling) Centered Fast Edge Sampling, vertices are ordered by degrees"
#define WEDGE_SAMPLING "(local sampling) Vanilla Wedge Sampling"
#define PATH_SAMPLING "(local sampling) Vanilla Path Sampling -- (inspired by) Jha et al. Path sampling: A fast and provable method for estimating 4-vertex subgraph counts. WWW 2015."
#define PATH_CENTERED_SAMPLING "(local sampling) Centered Path Sampling, vertices are ordered by degrees -- (inspired by) Jha et al. WWW 2015."
#define PATH_CENTERED_SAMPLING_BY_WEDGE "(local sampling) Centered Path Sampling ordered by wedge" 
#define PATH_CENTERED_SAMPLING_BY_DEGENERACY "(local sampling) Centered Path Sampling ordered by degeneracy"

namespace constants {
	extern std::vector<std::string> algorithm_names;
	extern std::unordered_map<std::string, std::string> suffix_output_address;
	extern int ebfc_iterations;
}

namespace settings {
	extern int snapshots;
	extern long double max_time;
	extern int exp_repeatition;
	extern int n_colors;
	extern double p;
	extern double min_p, max_p;
	extern int min_clr, max_clr;
	extern std::string chosen_algo;
	extern std::vector<std::string>::iterator itr;
	void get_settings();
	bool is_exact_algorithm();
	bool is_one_shot_algorithm();
	bool is_local_sampling_algorithm();
	void clear_settings();
	bool continue_run();
	int next_parameter();
}

namespace helper_functions {
	std::string to_str(int x);
	int to_int(const std::string& str);
	double to_double(const std::string& str);
	bool is_int_num(const std::string& str);
	bool is_equal(double a, double b);
	long long choose2(int x);
	bool yesno_query();
}