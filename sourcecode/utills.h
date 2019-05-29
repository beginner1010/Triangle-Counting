#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <sstream>

#include "IO.h"

#define EXACT "Exact algorithm"
#define ONE_SHOT_DOULION "Doulion (one shot) algorithm"
#define ONE_SHOT_COLORFUL "Colorful (one shot) algorithm"
#define LOCAL_WEDGE_SAMPLING "Wedge sampling algorithm" 
#define STREAMING_TRIEST_BASE "Streaming triest-base algorithm" 
#define STREAMING_TRIEST_IMPR "Streaming triest-impr algorithm" 
#define STREAMING_MASCOT_C "Streaming mascot-C algorithm"
#define STREAMING_MASCOT "Streaming mascot algorithm"

namespace constants {
	extern std::vector<std::string> algorithm_names;
	extern std::unordered_map<std::string, std::string> folder_algo_name;
}

namespace settings {
	extern int snapshots;
	extern long double max_time;
	extern int exp_repeatition;
	extern int n_colors;
	extern double p;
	extern int reservoir_size;
	extern std::string chosen_algo;
	extern std::vector<std::string>::iterator itr;
	void get_settings();
}

namespace helper_functions {
	std::string to_str(int x);
	int to_int(const std::string& str);
	bool is_int_num(const std::string& str);
}