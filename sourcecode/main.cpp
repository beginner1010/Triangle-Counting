#define _CRT_SECURE_NO_WARNINGS

#include "Graph.h"
#include "algorithms.h"
#include "counting.h"
#include "IO.h"
#include "utills.h"
#include "printing.h"

int main() {
	std::ios::sync_with_stdio(false);

	IO::IO_addresses();
	IO::fin = fopen(IO::input_address.c_str(), "r");
	Graph graph = Graph();
	graph.read_from_file();
	std::fclose(IO::fin);

	bool run = true;
	while (run) {
		settings::get_settings();
		IO::create_folder();

		if (settings::compressed == true) {
			graph.compress_graph();
		}

		print::statistics_table(graph.get_n_vertices(), graph.get_n_edges(), graph.get_maximum_degree());

		exact::algorithms* exact_algo = new exact::algorithms();
		static_processing::one_shot::algorithms* static_one_shot_algo = new static_processing::one_shot::algorithms();
		static_processing::local_sampling::algorithms* static_local_samp_algo = new static_processing::local_sampling::algorithms();
		streamming::one_pass::algorithms* stream_one_pass_algo = new streamming::one_pass::algorithms();

		IO::fout = fopen(IO::output_address.c_str(), "w");
		print::print_header();
		if (settings::chosen_algo == EXACT) {
			exact_algo->edge_centric_exact_algorithm(graph);
			auto global_triangle_count = exact_algo->get_unnormalized_count(); // unnormalized count in this case is indeed a normalized one since the counter is exact.
			print::print_result({global_triangle_count, static_local_samp_algo->get_runtime()});
		}
		else {
			for (int iter_exp = 1; iter_exp <= settings::exp_repeatition; iter_exp++) {
				if (settings::chosen_algo == ONE_SHOT_DOULION) {
					static_one_shot_algo->reset();
					print::clear_line();
					print::done_experiments(iter_exp);
					static_one_shot_algo->doulion(graph, settings::p);
					double unnormalized = static_one_shot_algo->get_unnormalized_count();
					double global_triangle_count = utill::normalized(unnormalized, 1. / (settings::p * settings::p * settings::p));
					print::print_result({(double)iter_exp, global_triangle_count, static_one_shot_algo->get_runtime()});

				} else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
					static_one_shot_algo->reset();
					print::clear_line();
					print::done_experiments(iter_exp);
					static_one_shot_algo->colorful_counting(graph, settings::n_colors);
					double unnormalized = static_one_shot_algo->get_unnormalized_count();
					double global_triangle_count = utill::normalized(unnormalized, settings::n_colors * settings::n_colors);
					print::print_result({ (double)iter_exp, global_triangle_count, static_one_shot_algo->get_runtime() });

				} else if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
					if (iter_exp == 1)
						graph.process_wedges();
					static_local_samp_algo->reset();
					static_local_samp_algo->setup();
					while (static_local_samp_algo->get_runtime() <= settings::max_time) {
						static_local_samp_algo->wedge_sampling(graph);
						if (static_local_samp_algo->should_print(static_local_samp_algo->get_runtime())) {
							static_local_samp_algo->update_last_time_printed(static_local_samp_algo->get_runtime());
							double unnormalized = static_local_samp_algo->get_unnormalized_count();
 							double global_triangle_count = utill::normalized(unnormalized, (double)graph.get_n_wedges() / static_local_samp_algo->get_n_sampled() / 3.0);
							print::print_result({ (double)iter_exp, (double)static_local_samp_algo->get_n_sampled(), global_triangle_count, static_local_samp_algo->get_runtime() });
							print::clear_line();
							print::done_experiments(iter_exp);
							print::done_work_percentage(static_local_samp_algo->get_runtime(), settings::max_time, "wedge sampling");
						}
					}
				}
			}
		}
		std::cerr << std::endl;
		std::cerr << "\r The experiment is finished. Look at for results: " << IO::output_address << std::endl << std::endl;
		run = settings::continue_run();
	}

	std::fclose(IO::fout);
}
