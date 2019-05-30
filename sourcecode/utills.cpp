#include "utills.h"

namespace constants {
	std::vector<std::string> algorithm_names = { EXACT, ONE_SHOT_DOULION, ONE_SHOT_COLORFUL,
		LOCAL_WEDGE_SAMPLING, STREAMING_TRIEST_BASE, STREAMING_TRIEST_IMPR, STREAMING_MASCOT_C, STREAMING_MASCOT };
	std::unordered_map<std::string, std::string> folder_algo_name = 
	{
		{ EXACT, "exact" },{ ONE_SHOT_DOULION, "one_shot/doulion" },{ ONE_SHOT_COLORFUL, "one_shot/colorful" },
		{ LOCAL_WEDGE_SAMPLING , "local_sampling/wedge_sampling" },{ STREAMING_TRIEST_BASE , "streaming/triest_base" },
		{ STREAMING_TRIEST_IMPR, "streaming/impr" },{ STREAMING_MASCOT_C, "streaming/mascot_C" },
		{ STREAMING_MASCOT, "streaming/mascot" }
	};
}

namespace settings {
	int snapshots;
	long double max_time = -1;
	int exp_repeatition = -1;
	int n_colors;
	double p;
	int reservoir_size;
	std::string chosen_algo;
	std::vector<std::string>::iterator itr;
	void get_settings() {
		settings::clear_settings();
		while (true) {
			if (settings::chosen_algo != "") {
				std::cerr << " The algorithm is not found!" << std::endl;
			}

			std::cerr << " Insert a number corresponding to an algorithm, listed below:" << std::endl;
			for (int i = 0; i < (int)constants::algorithm_names.size(); i++) {
				fprintf(stderr, "\t[%d] %s\n", i + 1, constants::algorithm_names[i].c_str());
			}

			std::cerr << " >>> "; std::cin >> settings::chosen_algo;
			if (helper_functions::is_int_num(settings::chosen_algo)) {
				int idx = helper_functions::to_int(settings::chosen_algo);
				settings::chosen_algo = constants::algorithm_names[idx - 1];
				break;
			}
		}

		if (settings::chosen_algo != EXACT) {
			std::cerr << " Insert #repeatition of the experiments:" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::exp_repeatition;
		}

		if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
			std::cerr << " Insert the execution time (in seconds):" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::max_time;
		}

		if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			std::cerr << " Insert #colors for painting vertices:" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::n_colors;
			assert(settings::n_colors > 1);
		}

		if ((std::unordered_set<std::string>({ ONE_SHOT_DOULION, STREAMING_MASCOT_C, STREAMING_MASCOT })).count(settings::chosen_algo) > 0) {
			std::cerr << " Insert the sampling probability:" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::p;
			assert(0.0 < settings::p && settings::p <= 1.0);
		}

		if (settings::chosen_algo != EXACT) {
			std::cerr << " Insert # snapshots (# lines in output):" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::snapshots;
			assert(0 < settings::snapshots);
		}
	}

	void clear_settings() {
		max_time = -1;
		exp_repeatition = -1;
		chosen_algo = "";
	}

	bool continue_run() {
		char command;
		std::cerr << " Would you like to try with different algorithms/settings?(y/n)" << std::endl;
		std::cerr << " >>> "; std::cin >> command;
		return command == 'Y' || command == 'y';
	}

}

namespace helper_functions {
	std::string to_str(int x) {
		std::stringstream ss;
		ss << x;
		return ss.str();
	}
	int to_int(const std::string& str) {
		std::stringstream ss; ss << str;
		int x; ss >> x;
		return x;
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
}
