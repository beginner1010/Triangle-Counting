#include "utills.h"


namespace constants {
	std::vector<std::string> algorithm_names = { 
		EXACT, 
		ONE_SHOT_COLORFUL,
		ONE_SHOT_DOULION,
		FAST_EDGE_SAMPLING,
		WEDGE_SAMPLING,
		FAST_CENTERED_EDGE_SAMPLING, PATH_SAMPLING,
		PATH_CENTERED_SAMPLING, PATH_CENTERED_SAMPLING_BY_WEDGE,
		PATH_CENTERED_SAMPLING_BY_DEGENERACY };
	std::unordered_map<std::string, std::string> suffix_output_address =
	{
		{ EXACT, "exact" },
		{ ONE_SHOT_DOULION, "doulion" },
		{ ONE_SHOT_COLORFUL, "colorful" },
		{ FAST_EDGE_SAMPLING, "fast_edge" },
		{ FAST_CENTERED_EDGE_SAMPLING, "c_fast_edge" },
		{ PATH_SAMPLING, "path" },
		{ PATH_CENTERED_SAMPLING , "c_path" },
		{ PATH_CENTERED_SAMPLING_BY_WEDGE , "c_path_ord_wedge" },
		{ WEDGE_SAMPLING, "wedge" },
		{ PATH_CENTERED_SAMPLING_BY_DEGENERACY, "c_path_ord_degen" }
	};
	int ebfc_iterations = 2000;
}

namespace settings {
	int snapshots;
	long double max_time = -1;
	int exp_repeatition = -1;
	int n_colors = -1;
	double p = -1;
	double min_p, max_p;
	int min_clr, max_clr;
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

		if (settings::is_exact_algorithm() == false) {
			std::cerr << " Insert #repeatition of the experiments:" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::exp_repeatition;
		}

		if (settings::chosen_algo == ONE_SHOT_DOULION) {
			std::cerr << " Insert the sampling probability:" << std::endl;
			std::string line;
			std::cerr << " >>> "; std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_p >> settings::max_p;
				settings::p = settings::min_p;
			}else {
				settings::p = helper_functions::to_double(line);
				settings::min_p = settings::max_p = settings::p; 
			}
			assert(0.0 < settings::min_p && settings::max_p <= 1.0);
		}

		if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			std::cerr << " Insert #colors for painting vertices:" << std::endl;
			std::string line;
			std::cerr << " >>> "; std::cin >> line;
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
			std::cerr << " Insert the execution time (in seconds):" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::max_time;
			assert(settings::max_time >= 1.0);
		}

		if (settings::is_local_sampling_algorithm() == true) {
			/* 
				I removed this setting as I am going to print output files simply every second.
				//std::cerr << " Insert # snapshots (# lines in output per experiment):" << std::endl;
				//std::cerr << " >>> "; std::cin >> settings::snapshots;
				//assert(0 < settings::snapshots);
			*/
			settings::snapshots = (int)(settings::max_time + 1e-6);
		}
	}

	bool is_exact_algorithm() {
		return std::unordered_set<std::string>({ EXACT }).count(settings::chosen_algo) > 0;
	}

	bool is_one_shot_algorithm() {
		return std::unordered_set<std::string>({
			ONE_SHOT_DOULION,
			ONE_SHOT_COLORFUL
		}).count(settings::chosen_algo) > 0;
	}

	bool is_local_sampling_algorithm() {
		return std::unordered_set<std::string>({ 
			FAST_EDGE_SAMPLING, 
			FAST_CENTERED_EDGE_SAMPLING,
			WEDGE_SAMPLING,
			PATH_SAMPLING,
			PATH_CENTERED_SAMPLING,
			PATH_CENTERED_SAMPLING_BY_WEDGE,
			PATH_CENTERED_SAMPLING_BY_DEGENERACY
		}).count(settings::chosen_algo) > 0;
	}

	void clear_settings() {
		max_time = -1;
		exp_repeatition = -1;
		chosen_algo = "";
	}

	bool continue_run() {
		std::cerr << " Would you like to try experiments for the same graph but different algorithms/settings?(y/n)" << std::endl;
		return helper_functions::yesno_query();
	}
	int next_parameter() {
		if (settings::chosen_algo == ONE_SHOT_DOULION) {
			if (settings::p >= settings::max_p) {
				settings::p = settings::min_p;
				return -1;
			}
			else if (settings::p * 2 <= settings::max_p || helper_functions::is_equal(settings::p, settings::max_p)) {
				settings::p = settings::p * 2;
				return 0;
			}
			else {
				settings::p = mmin(settings::max_p, settings::p + 0.05);
				return 0;
			}
		}
		else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			if (settings::n_colors == settings::min_clr) {
				settings::n_colors = settings::max_clr; // return it back to the initial value
				return -1;
			}
			else if (settings::n_colors / 2 >= settings::min_clr) {
				settings::n_colors = settings::n_colors / 2;
				return 0;
			}
			else {
				settings::n_colors = mmax(settings::min_clr, settings::n_colors - 20);
				return 0;
			}
		}
		else
			return -2; // should never happen!
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
	double to_double(const std::string& str) {
		std::stringstream ss; ss << str;
		double x; ss >> x;
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
	bool is_equal(double a, double b) {
		return std::fabs(a - b) <= 1e-6;
	}
	long long choose2(int x) {
		return ((long long)x * (x - 1)) >> 1;
	}
	bool yesno_query() {
		std::string command;
		std::cerr << " >>> "; std::cin >> command;
		return command[0] == 'Y' || command[0] == 'y';
	}
}
