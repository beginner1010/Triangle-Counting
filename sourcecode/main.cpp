#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <random>
#include <cstdio>
#include <sstream>
#include <sys/stat.h>
#ifdef _WIN32
	#include <direct.h>
#endif
#ifdef _WIN32
	#include <io.h> 
	#define access    _access_s
#else
	#include <unistd.h>
#endif

#include "Graph.h"
#include "algorithms.h"
#include "counting.h"

#define EXACT "Exact algorithm"
#define ONE_SHOT_DOULION "Doulion (one shot) algorithm"
#define ONE_SHOT_COLORFUL "Colorful (one shot) algorithm"
#define LOCAL_WEDGE_SAMPLING "Wedge sampling algorithm" 
#define STREAMING_TRIEST_BASE "Streaming triest-base algorithm" 
#define STREAMING_TRIEST_IMPR "Streaming triest-impr algorithm" 
#define STREAMING_MASCOT_C "Streaming mascot-C algorithm"
#define STREAMING_MASCOT "Streaming mascot algorithm"

std::vector<std::string> algorithm_names({ EXACT, ONE_SHOT_DOULION, ONE_SHOT_COLORFUL,
	LOCAL_WEDGE_SAMPLING, STREAMING_TRIEST_BASE, STREAMING_TRIEST_IMPR, STREAMING_MASCOT_C, STREAMING_MASCOT });

std::unordered_map<std::string, std::string> folder_algo_name
({ 
	{ EXACT, "exact"},  {ONE_SHOT_DOULION, "one_shot/doulion"}, { ONE_SHOT_COLORFUL, "one_shot/colorful"}, 
	{ LOCAL_WEDGE_SAMPLING , "local_sampling/wedge_sampling"}, { STREAMING_TRIEST_BASE , "streaming/triest_base"}, 
	{ STREAMING_TRIEST_IMPR, "streaming/impr"}, { STREAMING_MASCOT_C, "streaming/mascot_C" }, 
	{ STREAMING_MASCOT, "streaming/mascot"} 
});


namespace settings {
	int snapshots;
	long double max_time = -1;
	int exp_repeatition = -1;
	int n_colors;
	double p;
	int reservoir_size;
	std::string chosen_algo;
	std::string input_address;
	std::string output_address;
	std::string input_file_name;
	std::vector<std::string>::iterator itr;
}

bool check_if_file_exists(std::string& input_address) {
	if (access(input_address.c_str(), 0) != 0) {
		std::cerr << " No such file exists!" << std::endl;	
		return false;
	}
	return true;
}

inline std::string to_str(int x) { 
	std::stringstream ss; 
	ss << x;  
	return ss.str();
}
inline int to_int(const std::string& str) {
	std::stringstream ss; ss << str;
	int x; ss >> x;
	return x;
}
inline bool is_int_num(const std::string& str) {
	if ((int)str.size() > 9)
		return false;
	for (int i = 0; i < (int)str.size(); i++) {
		if ('0' > str[i] || str[i] > '9')
			return false;
	}
	return true;
}

void create_folder();
void get_settings();
void print_header(const std::string& chosen_algo);

int main() {
	std::ios::sync_with_stdio(false);

	get_settings();
	create_folder();
	
	FILE *f_in = freopen(settings::input_address.c_str(), "r", stdin);

	Graph graph = Graph();
	graph.read_from_file();

	std::fclose(f_in);

	print_header(settings::chosen_algo);

	static_processing::one_shot::algorithms* static_one_shot_algo = new static_processing::one_shot::algorithms();
	static_processing::local_sampling::algorithms* static_local_samp_algo = new static_processing::local_sampling::algorithms();
	streamming::one_pass::algorithms* stream_one_pass_algo = new streamming::one_pass::algorithms();
	
	if (settings::chosen_algo == EXACT) {
		auto global_triangle_count = counting::triangle::exact_edge_centeric_global_counting(graph);
		printf("EXACT, %.3f\n", global_triangle_count);
	}
	else {
		for (int iter_exp = 1; iter_exp <= settings::exp_repeatition ; iter_exp++) {
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				static_one_shot_algo->doulion(graph, settings::p);
				double unnormalized = static_one_shot_algo->get_unnormalized_count();
				double global_triangle_count = utill::normalized(unnormalized, 1. / (settings::p * settings::p * settings::p));
				printf("%d, DOULION, %.3f\n", iter_exp, global_triangle_count);
			}
		}
		std::cerr << "\r" << std::string(60, ' ');
		std::cerr << "\r The experiment is finished. Look at: " << settings::output_address << std::endl;
	}
}

void get_settings() {
	while (true) {
		if (settings::chosen_algo != "") {
			std::cerr << " The algorithm is not found!" << std::endl;
		}

		std::cerr << " Insert a number corresponding to an algorithm, listed below:" << std::endl;
		for (int i = 0; i < (int)algorithm_names.size(); i++) {
			fprintf(stderr, "\t[%d] %s\n", i + 1, algorithm_names[i].c_str());
		}

		std::cerr << " >>> "; std::cin >> settings::chosen_algo;
		if (is_int_num(settings::chosen_algo)) {
			int idx = to_int(settings::chosen_algo);
			settings::chosen_algo = algorithm_names[idx - 1];
			break;
		}
	}

	do {
		std::cerr << " Insert an input file (input graph) location:" << std::endl;
		std::cerr << " >>> "; std::cin >> settings::input_file_name;
#ifdef OFFICE_PC
		input_address = "input/in." + input_file_name;
#else // on Nova
		settings::input_address = "/work/baskarg/vas/sbfly/in." + settings::input_file_name;
#endif
	} while (check_if_file_exists(settings::input_address) == false);

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

void print_header(const std::string& chosen_algo) {
	if (chosen_algo == EXACT) {
		printf("algo, bfly, time");
	}
	else if (chosen_algo == ONE_SHOT_DOULION) {
		printf("itr, algo, prob, bfly, time");
	}
	else if (chosen_algo == ONE_SHOT_COLORFUL) {
		printf("itr, algo, n_colors, bfly, time");
	}

	printf("\n");
}

void create_folder() {
	std::string folder_name = folder_algo_name[settings::chosen_algo];
#ifdef _WIN32
	::_mkdir("./output");
	::_mkdir(("./output/" + settings::input_address).c_str());
	::_mkdir(("./output/" + settings::input_address + "/" + folder_name).c_str());
#else
	::mkdir("./output", 0777);
	::mkdir(("./output/" + input_address).c_str(), 0777);
	::mkdir(("./output/" + input_address + "/" + algo_name).c_str(), 0777);
#endif
	std::string file_name = "";
	file_name += settings::max_time == -1 ? "" : "t=" + to_str(settings::max_time);
	file_name += settings::exp_repeatition == -1 ? "" : "rep=" + to_str(settings::exp_repeatition);
	file_name = folder_name == "exact" ? "exact" : file_name;
	settings::output_address = "output/" + settings::input_address + "/" + folder_name + "/" + file_name + ".txt";
	freopen(settings::output_address.c_str(), "w", stdout);
	return;
}
