#include "utills.h"


namespace constants {
	std::vector<std::string> algorithm_names = 
	{ 
		EXACT, ONE_SHOT_DOULION, 
		ONE_SHOT_COLORFUL, ONE_SHOT_EDGE_WEDGE_SAMPLING,
		LOCAL_WEDGE_SAMPLING, LOCAL_REVISITED_WEDGE_SAMPLING,
		EXACT_INS_ONLY, STREAMING_TRIEST_BASE, STREAMING_TRIEST_IMPR, STREAMING_MASCOT_C, STREAMING_MASCOT, GPS,
		EXACT_FULL_DYN, STREAMING_TRIEST_FD, THINKD_ACC
	};
	std::unordered_map<std::string, std::string> folder_algo_name = 
	{
		{ EXACT, "exact" },{ EXACT_INS_ONLY, "exact" },{ EXACT_FULL_DYN, "exact" },
		{ ONE_SHOT_DOULION, "one_shot" },{ ONE_SHOT_COLORFUL, "one_shot" },	{ONE_SHOT_EDGE_WEDGE_SAMPLING, "one_shot"},
		{ LOCAL_WEDGE_SAMPLING , "local_sampling" }, { LOCAL_REVISITED_WEDGE_SAMPLING , "local_sampling"},
		{ STREAMING_TRIEST_BASE , "ins_only_stream" }, { STREAMING_TRIEST_IMPR, "ins_only_stream" }, { STREAMING_MASCOT_C, "ins_only_stream" }, { STREAMING_MASCOT, "ins_only_stream" }, { GPS, "ins_only_stream" },
		{ STREAMING_TRIEST_FD , "fully_dyn_stream"}, {THINKD_ACC, "fully_dyn_stream"}
	};
	std::unordered_map<std::string, std::string> suffix_output_address =
	{
		{ EXACT, "exact" },{ EXACT_INS_ONLY, "exact_ins_only" },{ EXACT_FULL_DYN, "exact_full_dyn" },
		{ ONE_SHOT_DOULION, "doulion" },{ ONE_SHOT_COLORFUL, "colorful" }, {ONE_SHOT_EDGE_WEDGE_SAMPLING, "ewsamp"},
		{ LOCAL_WEDGE_SAMPLING , "wedge" }, { LOCAL_REVISITED_WEDGE_SAMPLING , "revisited_wedge"},
		{ STREAMING_TRIEST_BASE , "triest_base" },{ STREAMING_TRIEST_IMPR, "triest_impr" }, { STREAMING_MASCOT_C, "mascot_C" },	{ STREAMING_MASCOT, "mascot" }, { GPS, "gps" },
		{ STREAMING_TRIEST_FD, "triest_fd"}, {THINKD_ACC, "thinkd_acc"}
	};
}

namespace settings {
	int snapshots;
	long double max_time = -1;
	int exp_repeatition = -1;
	double min_p, max_p, p;
	int min_clr, max_clr, n_colors;
	bool compressed = false;
	bool interactive_mode = true;
	int reservoir_size, max_res_size, min_res_size;
	std::string chosen_algo;
	std::vector<std::string>::iterator itr;
	bool is_static;

	void is_static_or_streaming() {
		if (settings::interactive_mode == true) {
			std::cerr << " Is this a static graph?(y/n) [\"n\" means that the graph is streaming]" << std::endl;
		}
		settings::is_static = helper_functions::yesno_query();
	}

	void get_settings() {
		settings::clear_settings();
		while (true) {
			if (settings::interactive_mode == true) {
				if (settings::chosen_algo != "") {
					std::cerr << " The algorithm is not found!" << std::endl;
				}
				std::cerr << " Insert a number corresponding to an algorithm, listed below:" << std::endl;
				for (int i = 0; i < (int)constants::algorithm_names.size(); i++) {
					fprintf(stderr, "\t[%2d] %s\n", i + 1, constants::algorithm_names[i].c_str());
				}
			}

			if (settings::interactive_mode == true) {
				std::cerr << " >>> ";
			}
			std::cin >> settings::chosen_algo;
			if (helper_functions::is_int_num(settings::chosen_algo)) {
				int idx = helper_functions::to_int(settings::chosen_algo);
				settings::chosen_algo = constants::algorithm_names[idx - 1];
				break;
			}
		}

		//if (settings::is_streaming_algorithm() == false) {
		//	std::cerr << " Run experiments on COO (compressed) version of graph ? (y/n)" << std::endl;
		//	compressed = helper_functions::yesno_query();
		//}

		if (settings::is_exact_algorithm() == false) {
			if (settings::interactive_mode == true) {
				std::cerr << " Insert #repeatition of the experiments:" << std::endl;
				std::cerr << " >>> ";
			}
			std::cin >> settings::exp_repeatition;
		}

		if (settings::need_fixed_prob_algorithms() == true && settings::is_streaming_algorithm() == false) {
			if (settings::interactive_mode == true) {
				std::cerr << " Insert the sampling probability:" << std::endl;
				std::cerr << " >>> ";
			}
			std::string line;
			std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_p >> settings::max_p;
				settings::p = settings::min_p;
			}
			else {
				settings::p = helper_functions::to_double(line);
				settings::min_p = settings::max_p = settings::p;
			}
			assert(0.0 < settings::min_p && settings::max_p <= 1.0);
		}

		if (settings::is_streaming_algorithm() == true && settings::is_exact_algorithm() == false) {
			if (settings::interactive_mode == true) {
				std::cerr << " Insert reservoir sizes";
				if (settings::need_fixed_prob_algorithms() == true) {
					std::cerr << " (the fixed probability is equal to stream size / reservoir size)";
				}
				std::cerr << ":" << std::endl;
				std::cerr << " >>> ";
			}
			std::string line;
			std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_res_size >> settings::max_res_size;
				settings::reservoir_size = settings::min_res_size;
			}
			else {
				settings::reservoir_size = helper_functions::to_int(line);
				settings::min_res_size = settings::max_res_size = settings::reservoir_size;
			}
		}

		if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			if (settings::interactive_mode == true) {
				std::cerr << " Insert #colors for painting vertices:" << std::endl;
				std::cerr << " >>> ";
			}
			std::string line;
			std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_clr >> settings::max_clr;
				settings::n_colors = settings::max_clr;
			}
			else {
				settings::n_colors = helper_functions::to_int(line);
				settings::min_clr = settings::max_clr = settings::n_colors;
			}
			assert(settings::min_clr >= 1);
		}

		if (settings::is_local_sampling_algorithm() == true) {
			if (settings::interactive_mode == true) {
				std::cerr << " Insert the execution time (in seconds):" << std::endl;
				std::cerr << " >>> ";
			}
			std::cin >> settings::max_time;
		}

		if (settings::is_local_sampling_algorithm() == true) {
			//std::cerr << " Insert # snapshots (# lines in output):" << std::endl;
			//std::cerr << " >>> "; std::cin >> settings::snapshots;
			//assert(0 < settings::snapshots);
			settings::snapshots = (int)(settings::max_time + 1e-6);
		}

		if (settings::take_snapshots() == true) {
			if (settings::interactive_mode == true) {
				std::cerr << " Insert # snapshots (# lines in output per experiment):" << std::endl;
				std::cerr << " >>> ";
			}
			std::cin >> settings::snapshots;
			assert(0 < settings::snapshots);
		}
	}

	bool is_exact_algorithm() {
		return std::unordered_set<std::string>({ EXACT, EXACT_INS_ONLY, EXACT_FULL_DYN }).count(settings::chosen_algo) > 0;
	}

	bool is_one_shot_algorithm() {
		return std::unordered_set<std::string>({ ONE_SHOT_DOULION, ONE_SHOT_COLORFUL, ONE_SHOT_EDGE_WEDGE_SAMPLING }).count(settings::chosen_algo) > 0;
	}

	bool is_local_sampling_algorithm() {
		return std::unordered_set<std::string>({ LOCAL_WEDGE_SAMPLING, LOCAL_REVISITED_WEDGE_SAMPLING }).count(settings::chosen_algo) > 0;
	}

	bool need_fixed_prob_algorithms() {
		return (std::unordered_set<std::string>({ ONE_SHOT_DOULION, ONE_SHOT_EDGE_WEDGE_SAMPLING, STREAMING_MASCOT_C, STREAMING_MASCOT })).count(settings::chosen_algo) > 0;
	}

	bool has_fixed_reservoir_size() {
		return (std::unordered_set<std::string>({ STREAMING_TRIEST_BASE, STREAMING_TRIEST_IMPR, STREAMING_TRIEST_FD, THINKD_ACC })).count(settings::chosen_algo) > 0;
	}

	bool is_static_randomized_algorithm() {
		return settings::is_one_shot_algorithm() == true || settings::is_local_sampling_algorithm() == true;
	}

	bool is_streaming_insertion_only() {
		return std::unordered_set<std::string>({ STREAMING_MASCOT, STREAMING_MASCOT_C, STREAMING_TRIEST_BASE, STREAMING_TRIEST_IMPR, EXACT_INS_ONLY, GPS }).count(settings::chosen_algo) > 0;
	}

	bool is_streaming_fully_dynamic() {
		return std::unordered_set<std::string>({ EXACT_FULL_DYN, STREAMING_TRIEST_FD, THINKD_ACC }).count(settings::chosen_algo) > 0;
	}

	bool is_streaming_algorithm() {
		return settings::is_streaming_insertion_only() == true || settings::is_streaming_fully_dynamic() == true;
	}

	bool take_snapshots() {
		return settings::is_streaming_algorithm() == true || (std::unordered_set<std::string>({ EXACT_INS_ONLY, EXACT_FULL_DYN })).count(settings::chosen_algo) > 0;
	}

	void clear_settings() {
		max_time = -1;
		exp_repeatition = -1;
		chosen_algo = "";
	}

	bool continue_run() {
		if (settings::interactive_mode == true) {
			std::cerr << " Would you like to try experiments for the same graph but different algorithms/settings?(y/n)" << std::endl;
		}
		return helper_functions::yesno_query();
	}

	int next_parameter() {
		if (settings::is_exact_algorithm() == true)
			return -1;
		if (settings::is_streaming_algorithm() == false) {
			if (settings::need_fixed_prob_algorithms()) {
				if (settings::p >= settings::max_p) {
					settings::p = settings::min_p;
					return -1;
				}
				else {
					settings::p = mmin(settings::max_p, settings::p * 2);
					return 0;
				}
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				if (settings::n_colors == settings::min_clr) {
					settings::n_colors = settings::max_clr; // return it back to the initial value
					return -1;
				}
				else {
					settings::n_colors = mmax(settings::min_clr, settings::n_colors / 2);
					return 0;
				}
			}
		}
		else {
			if (settings::reservoir_size >= settings::max_res_size) {
				settings::reservoir_size = settings::min_res_size;
				return -1;
			}
			else {
				settings::reservoir_size = mmin(settings::max_res_size, settings::reservoir_size * 2);
				return 0;
			}
		}
	}
}

namespace helper_functions {
	std::string to_str(int x) {
		std::stringstream ss;
		ss << x;
		return ss.str();
	}
	long long to_ll(const std::string& str) {
		std::stringstream ss; ss << str;
		long long x; ss >> x;
		return x;
	}
	int to_int(const std::string& str) {
		std::stringstream ss; ss << str;
		int x; ss >> x;
		return x;
	}
	double to_double(const std::string& str) {
		std::stringstream ss; ss << str;
		double x; ss >> x;
		return x;
	}
	bool is_equal(double a, double b) {
		return std::fabs(a - b) <= 1e-6;
	}
	bool is_int_num(const std::string& str) {
		if ((int)str.size() > 9)
			return false;
		for (int i = 0; i < (int)str.size(); i++) {
			if ('0' > str[i] || str[i] > '9')
				return false;
		}
		return true;
	}
	long long choose2(int x) {
		return ((long long)x * (x - 1)) >> 1;
	}
	bool yesno_query() {
		std::string command;
		if (settings::interactive_mode == true) {
			std::cerr << " >>> ";
		}
		std::cin >> command;
		return command[0] == 'Y' || command[0] == 'y';
	}
	void is_interactive(int argc, char** argv) {
		settings::interactive_mode = true;
		if (argc >= 2 && (argv[1][0] == '0' || argv[1][0] == 'f' || argv[1][0] == 'F' || argv[1][0] == 'n' || argv[1][0] == 'N'))
			settings::interactive_mode = false;
	}
}
