#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <random>
#include <cstdio>
#include <sstream>


#include "Graph.h"
#include "algorithms.h"
#include "counting.h"
#include "IO.h"
#include "utills.h"

int main() {
	std::ios::sync_with_stdio(false);

	settings::get_settings();
	IO::create_folder();
	
	IO::fin = fopen(IO::input_address.c_str(), "r");
	Graph graph = Graph();
	graph.read_from_file();
	std::fclose(IO::fin);

	static_processing::one_shot::algorithms* static_one_shot_algo = new static_processing::one_shot::algorithms();
	static_processing::local_sampling::algorithms* static_local_samp_algo = new static_processing::local_sampling::algorithms();
	streamming::one_pass::algorithms* stream_one_pass_algo = new streamming::one_pass::algorithms();

	IO::fout = fopen(IO::output_address.c_str(), "w");
	print::print_header();
	if (settings::chosen_algo == EXACT) {
		auto global_triangle_count = counting::triangle::exact_edge_centeric_global_counting(graph);
		fprintf(IO::fout, "EXACT, %llu\n", global_triangle_count);
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
		std::cerr << "\r The experiment is finished. Look at: " << IO::output_address << std::endl;
	}

	std::fclose(IO::fout);
}
