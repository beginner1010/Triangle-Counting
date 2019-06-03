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
			graph.compress_graph(true);
		}

		print::statistics_table(graph.get_n_vertices(), graph.get_n_edges(), graph.get_maximum_degree());

		exact::algorithms* exact_algo = new exact::algorithms();
		static_processing::one_shot::algorithms* static_one_shot_algo = new static_processing::one_shot::algorithms();
		static_processing::local_sampling::algorithms* static_local_samp_algo = new static_processing::local_sampling::algorithms();
		streamming::one_pass::algorithms* stream_one_pass_algo = new streamming::one_pass::algorithms();

		IO::fout = fopen(IO::output_address.c_str(), "w");
		print::print_header();
		if (settings::chosen_algo == EXACT) {
			exact_algo->reset();
			exact_algo->edge_centric_exact_algorithm(graph);
			auto global_triangle_count = exact_algo->get_unnormalized_count(); // unnormalized count in this case is indeed a normalized one since the counter is exact.
			print::print_result({global_triangle_count, exact_algo->get_runtime()});
		}
		else {
			for (int iter_exp = 1; iter_exp <= settings::exp_repeatition; iter_exp++) {
				if (settings::is_one_shot_algorithm() == true) {
					static_one_shot_algo->reset();
					print::clear_line();
					print::done_experiments(iter_exp);
					static_one_shot_algo->run(graph);
					print::print_result(static_one_shot_algo->get_results(iter_exp));

				} else if (settings::is_local_sampling_algorithm() == true) {
					static_local_samp_algo->setup(iter_exp, graph);
					double tolerance = settings::max_time / settings::snapshots - 1e-6;
					while (static_local_samp_algo->get_runtime() <= settings::max_time + tolerance) {
						static_local_samp_algo->run(graph);
						if (static_local_samp_algo->should_print(static_local_samp_algo->get_runtime())) {
							static_local_samp_algo->update_last_time_printed(static_local_samp_algo->get_runtime());
							print::print_result(static_local_samp_algo->get_results(iter_exp, graph));
							print::clear_line();
							print::done_experiments(iter_exp);
							print::done_work_percentage(static_local_samp_algo->get_runtime(), settings::max_time, "local sampling");
						}
					}
				}
			}
		}
		print::clear_line();
		std::cerr << " The experiment is finished. Look at for results: " << IO::output_address << std::endl << std::endl;
		run = settings::continue_run();
	}

	std::fclose(IO::fout);
}
