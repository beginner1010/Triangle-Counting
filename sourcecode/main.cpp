#define _CRT_SECURE_NO_WARNINGS

#include "Graph.h"
#include "algorithms.h"
#include "counting.h"
#include "IO.h"
#include "utills.h"
#include "printing.h"


int main(int argc, char** argv){
	std::ios::sync_with_stdio(false);

	helper_functions::is_interactive(argc, argv); // pass 0 or f to make the interative mode off

	IO::IO_addresses();
	IO::fin = fopen(IO::input_address.c_str(), "r");
	Graph graph = Graph();
	//settings::is_static_or_streaming();
	graph.read_from_file();
	std::fclose(IO::fin);

	print::statistics_table(graph.get_n_vertices(), graph.get_n_edges(), graph.get_maximum_degree());

	bool run = true;
	bool stat_seen = false;
	while (run) {
		settings::get_settings();
		IO::create_folder();

		if (settings::compressed == true) {
			graph.compress_graph();
		}

		exact::algorithms* exact_algo = new exact::algorithms();
		static_processing::one_shot::algorithms* static_one_shot_algo = new static_processing::one_shot::algorithms();
		static_processing::local_sampling::algorithms* static_local_samp_algo = new static_processing::local_sampling::algorithms();
		streamming::one_pass::algorithms* streaming_algo = new streamming::one_pass::algorithms();

		IO::fout = fopen(IO::output_address.c_str(), "w");
		print::print_header();
		if (settings::is_exact_algorithm() == true && settings::is_streaming_algorithm() == false) {
			exact_algo->reset();
			exact_algo->edge_centric_exact_algorithm(graph);
			auto global_triangle_count = exact_algo->get_unnormalized_count(); // unnormalized count in this case is indeed a normalized one since the counter is exact.
			graph.process_wedges();
			print::print_result({global_triangle_count, exact_algo->get_runtime(), (double)graph.get_n_wedges()});
		}
		else if(settings::is_static_randomized_algorithm() == true){
			for (int iter_exp = 1; iter_exp <= settings::exp_repeatition; iter_exp++) {
				if (settings::is_one_shot_algorithm() == true) {
					do{
						static_one_shot_algo->reset();
						static_one_shot_algo->run(graph);
						print::print_result(static_one_shot_algo->get_results(iter_exp));
					}while (settings::next_parameter() != -1);
					if (settings::interactive_mode == true) {
						print::clear_line();
						print::done_experiments(iter_exp);
					}
				} else if (settings::is_local_sampling_algorithm() == true) {
					static_local_samp_algo->reset();
					static_local_samp_algo->setup(graph, iter_exp);
					double tolerance = settings::max_time / settings::snapshots - 1e-6;
					while (static_local_samp_algo->get_runtime() <= settings::max_time + tolerance) {
						static_local_samp_algo->run(graph);
						if (static_local_samp_algo->should_print(static_local_samp_algo->get_runtime())) {
							static_local_samp_algo->update_last_time_printed(static_local_samp_algo->get_runtime());
							print::print_result(static_local_samp_algo->get_results(iter_exp, graph));
							if (settings::interactive_mode == true) {
								print::clear_line();
								print::done_experiments(iter_exp);
								print::done_work_percentage(static_local_samp_algo->get_runtime(), settings::max_time, "local sampling");
							}
						}
					}
				}
			}
		}
		else if (settings::is_streaming_algorithm() == true) {
			if (settings::is_exact_algorithm() == true) {
				if (settings::is_streaming_insertion_only() == true) {
					streaming_algo->reset(graph);
					streaming_algo->set_interval(graph);
					auto& stream = graph.get_insertion_only_stream();
					for (auto& edge : stream) {
						streaming_algo->run(edge.first, edge.second, true);
						if (streaming_algo->should_print()) {
							print::print_result(streaming_algo->get_results(-1));
						}
					}
				} else if(settings::is_streaming_fully_dynamic() == true) {
					streaming_algo->reset(graph);
					streaming_algo->set_interval(graph);
					auto& stream = graph.get_fully_dynamic_stream();
					for (auto& edge : stream) {
						streaming_algo->run(edge.first.first, edge.first.second, edge.second);
						if (streaming_algo->should_print()) {
							print::print_result(streaming_algo->get_results(-1));
						}
					}
				}
			}
			else {
				for (int iter_exp = 1; iter_exp <= settings::exp_repeatition; iter_exp++) {
					do {
						streaming_algo->reset(graph);
						streaming_algo->set_interval(graph);
						if (settings::is_streaming_insertion_only() == true) {
							auto& stream = graph.get_insertion_only_stream();
							for (auto& edge : stream) {
								streaming_algo->run(edge.first, edge.second, true);
								if (streaming_algo->should_print()) {
									print::print_result(streaming_algo->get_results(iter_exp));
								}
							}
						} 
						else if (settings::is_streaming_fully_dynamic() == true) {
							auto& stream = graph.get_fully_dynamic_stream();
							for (auto& edge : stream) {
								streaming_algo->run(edge.first.first, edge.first.second, edge.second);
								if (streaming_algo->should_print()) {
									print::print_result(streaming_algo->get_results(iter_exp));
								}
							}
						}
					} while (settings::next_parameter() != -1);
				}
			}
		}
		if (settings::interactive_mode == true) {
			print::clear_line();
			std::cerr << " The experiment is finished. Output is available at: " << IO::output_address << std::endl << std::endl;
		}
		run = settings::continue_run();
	}

	std::fclose(IO::fout);
}
