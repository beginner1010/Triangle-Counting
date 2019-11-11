#include "algorithms.h"

/* Static graph: Approximation by one shot sampling */

namespace exact {
	void algorithms::exact_algorithm(Graph& G) {
		clock_t start_time = clock();
		this->unnormalized_triangle_count = (double)exact::global::kai_exact_count(G);
		this->runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
	}
}

namespace static_processing {
	namespace one_shot {
		void algorithms::doulion(Graph& G) {
			/*
				Tsourakakis, Charalampos E., U.Kang, Gary L.Miller, and Christos Faloutsos.
				"Doulion: counting triangles in massive graphs with a coin."
				In Proceedings of the 15th ACM SIGKDD international conference on Knowledge discovery and data mining, pp. 837 - 846. ACM, 2009.
			*/
			clock_t start_time = clock();
			Graph sampled_graph = Graph();
			for (auto& edge : G.get_edges()) {
				if (sampler::generate_random_dbl(0.0, 1.0) <= settings::p) {
					int left_vertex = sampled_graph.get_vertex_index(edge.first);
					int right_vertex = sampled_graph.get_vertex_index(edge.second);
					sampled_graph.add_new_edge(left_vertex, right_vertex, true);
				}
			}
			this->graph_construction_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			start_time = clock();
			this->unnormalized_triangle_count = (double)exact::global::kai_exact_count(sampled_graph);
			this->counting_on_sampled_graph_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			this->runtime = this->graph_construction_time + this->counting_on_sampled_graph_time;
			return;
		}
		void algorithms::colorful_counting(Graph& G) {
			/*
				Pagh, Rasmus, and Charalampos E. Tsourakakis.
				"Colorful triangle counting and a mapreduce implementation."
				Information Processing Letters 112, no. 7 (2012): 277-281.
			*/
			clock_t start_time = clock();
			std::vector <int> vertex_color;
			int n = G.get_n_vertices();
			for (int i = 0; i < n; i++) {
				vertex_color.push_back((int)sampler::generate_random_int(0, settings::n_colors - 1));
			}
			Graph sampled_graph = Graph();
			for (auto& edge : G.get_edges()) {
				if (vertex_color[edge.first] == vertex_color[edge.second]) {
					int left_vertex = sampled_graph.get_vertex_index(edge.first);
					int right_vertex = sampled_graph.get_vertex_index(edge.second);
					sampled_graph.add_new_edge(left_vertex, right_vertex, true);
				}
			}
			this->graph_construction_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			start_time = clock();
			this->unnormalized_triangle_count = (double)exact::global::kai_exact_count(sampled_graph);
			this->counting_on_sampled_graph_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			this->runtime = this->graph_construction_time + this->counting_on_sampled_graph_time;
			return;
		}


		void algorithms::run(Graph& G) {
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				this->doulion(G);
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				this->colorful_counting(G);
			}
		}
		std::vector<double> algorithms::get_results(const int& exp_iter) {
			double unnormalized = this->get_unnormalized_count();
			double global_triangle_count = 0;
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				global_triangle_count = utility::normalized(unnormalized, 1. / (settings::p * settings::p * settings::p * settings::p));
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				global_triangle_count = utility::normalized(unnormalized, (settings::n_colors * settings::n_colors * settings::n_colors));
			}
			return{ (double)exp_iter, global_triangle_count, this->get_construction_time(), this->get_counting_on_sampled_graph_time(), this->get_runtime() };
		}
	}

	/* Static graph: Approximation by local sampling */
	namespace local_sampling {
		void algorithms::fast_edge_sampling(Graph& G) {
			/*
			
			*/
			clock_t start_time = clock();
			this->n_sampled++;
			int random_edge_index = (int)sampler::generate_random_int(0, G.get_n_edges() - 1);
			std::pair<int, int>& edge = G.get_ith_edge(random_edge_index);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_triangle_count += randomized::edge::randomized_ebfc(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::fast_centered_edge_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			int random_edge_index = (int)sampler::generate_random_int(0, G.get_n_edges() - 1);
			std::pair<int, int>& edge = G.get_ith_edge(random_edge_index);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_triangle_count += randomized::edge::randomized_centered_ebfc(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::wedge_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			auto pair = sampler::weighted_sampling(G.get_cum_w());
			auto wedge = G.get_wedge(pair.second, pair.first, G.get_adj_vec(pair.first));
			this->unnormalized_triangle_count += exact::wedge::bfly_in_wedge(G, wedge);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::path_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			auto pair = sampler::weighted_sampling(G.get_cum_z());
			auto& edge = G.get_ith_edge(pair.first);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_triangle_count += randomized::path::z_forms_bfly(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::path_centered_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			auto pair = sampler::weighted_sampling(G.get_cum_centered_z());
			auto& edge = G.get_ith_edge(pair.first);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_triangle_count += randomized::path::centered_z_forms_bfly(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::path_centered_sampling_by_wedge(Graph& G) {
			/*

			*/
			this->path_centered_sampling(G);
		}

		void algorithms::path_centered_sampling_by_degeneracy(Graph& G) {
			/*

			*/
			this->path_centered_sampling(G);
		}

		void algorithms::run(Graph& G) {
			if (settings::chosen_algo == FAST_EDGE_SAMPLING) {
				this->fast_edge_sampling(G);
			}
			else if (settings::chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
				this->fast_centered_edge_sampling(G);
			}
			else if (settings::chosen_algo == WEDGE_SAMPLING) {
				this->wedge_sampling(G);
			}
			else if (settings::chosen_algo == PATH_SAMPLING) {
				this->path_sampling(G);
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING) {
				this->path_centered_sampling(G);
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_WEDGE) {
				this->path_centered_sampling_by_wedge(G);
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_DEGENERACY) {
				this->path_centered_sampling_by_degeneracy(G);
			}
		}
		std::vector<double> algorithms::get_results(const int& iter_exp, Graph& G) {
			double unnormalized = this->get_unnormalized_count();
			double global_triangle_count = 0;
			if (settings::chosen_algo == FAST_EDGE_SAMPLING) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_edges() / (this->get_n_sampled() * 4.0));
			}
			else if (settings::chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_edges() / (this->get_n_sampled()));
			}
			else if (settings::chosen_algo == WEDGE_SAMPLING) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_w() / (this->get_n_sampled() * 4.0));
			}
			else if (settings::chosen_algo == PATH_SAMPLING) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_z() / (this->get_n_sampled() * 4.0));
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_centered_z() / (this->get_n_sampled()));
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_WEDGE) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_centered_z() / (this->get_n_sampled()));
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_DEGENERACY) {
				global_triangle_count = utility::normalized(unnormalized, (double)G.get_n_centered_z() / (this->get_n_sampled()));
			}
			return { (double)iter_exp, this->get_preprocessing_time(), (double)this->get_n_sampled(), global_triangle_count, this->get_runtime() };;
		}
	}
}
