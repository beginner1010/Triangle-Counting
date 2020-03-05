#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <sstream>

#include "IO.h"

#define mmin(x,y) ((x)>(y)?(y):(x))
#define mmax(x,y) ((x)<(y)?(y):(x))

#define EXACT "* Exact algorithm -- Edge centric exact algorithm for Static Graphs"
#define ONE_SHOT_DOULION "* Doulion (one shot) algorithm for Static Graphs"
#define ONE_SHOT_COLORFUL "* Colorful (one shot) algorithm for Static Graphs"
#define LOCAL_WEDGE_SAMPLING "* Wedge sampling algorithm for Static Graphs" 
#define ONE_SHOT_EDGE_WEDGE_SAMPLING "* One Shot Edge Wedge Sampling for Static Graphs"
#define LOCAL_REVISITED_WEDGE_SAMPLING "* Local Revisited Wedge Sampling for Static Graphs"
#define EXACT_INS_ONLY "** Exact Dynamic for Insertion-only Streams"
#define STREAMING_TRIEST_BASE "** Streaming Triest-base for Insertion-only Streams" 
#define STREAMING_TRIEST_IMPR "** Streaming Triest-impr for Insertion-only Streams" 
#define STREAMING_MASCOT_C "** Streaming Mascot-C for Insertion-only Streams"
#define STREAMING_MASCOT "** Streaming Mascot for Insertion-only Streams"
#define GPS "** Graph Priority Sampling for Insertion-only Streams"
#define EXACT_FULL_DYN "*** Exact Dynamic Algorithm for Fully-dynamic Streams"
#define STREAMING_TRIEST_FD "*** Streaming triest-fd for Fully-dynamic Streams"
#define THINKD_ACC "*** ThinkD Accuracy for Fully-dynamic Streams"



namespace constants {
	extern std::vector<std::string> algorithm_names;
	extern std::unordered_map<std::string, std::string> folder_algo_name;
	extern std::unordered_map<std::string, std::string> suffix_output_address;
}

namespace settings {
	extern int snapshots;
	extern long double max_time;
	extern int exp_repeatition;
	extern int min_clr, max_clr, n_colors;
	extern double p, min_p, max_p;
	extern bool compressed;
	extern bool interactive_mode;
	extern int reservoir_size, max_res_size, min_res_size;
	extern std::string chosen_algo;
	extern std::vector<std::string>::iterator itr;
	extern bool is_static;
	void is_static_or_streaming();
	void get_settings();
	void clear_settings();
	bool continue_run();
	bool is_exact_algorithm();
	bool is_one_shot_algorithm();
	bool need_fixed_prob_algorithms();
	bool has_fixed_reservoir_size();
	bool is_static_randomized_algorithm();
	bool is_streaming_insertion_only();
	bool is_streaming_fully_dynamic();
	bool is_streaming_algorithm();
	bool is_local_sampling_algorithm();
	bool take_snapshots();
	int next_parameter();
}

namespace helper_functions {
	std::string to_str(int x);
	long long to_ll(const std::string& str);
	int to_int(const std::string& str);
	double to_double(const std::string& str);
	bool is_equal(double a, double b);
	bool is_int_num(const std::string& str);
	long long choose2(int x);
	bool yesno_query();
	void is_interactive(int argc, char** argv);
}