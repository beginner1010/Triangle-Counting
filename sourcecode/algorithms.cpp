#include "algorithms.h"

/* Static graph: Approximation by one shot sampling */

namespace exact {
	void algorithms::edge_centric_exact_algorithm(Graph& G) {
		clock_t start_time = clock();
		this->unnormalized_triangle_count = (double)counting::triangle::exact_edge_centeric_global_counting(G);
		this->runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
	}
}

namespace static_processing {
	namespace one_shot {
		void algorithms::doulion(Graph& G, const double p) {
			/*
				Tsourakakis, Charalampos E., U.Kang, Gary L.Miller, and Christos Faloutsos.
				"Doulion: counting triangles in massive graphs with a coin."
				In Proceedings of the 15th ACM SIGKDD international conference on Knowledge discovery and data mining, pp. 837 - 846. ACM, 2009.
			*/
			clock_t start_time = clock();
			Graph sampled_graph = Graph();
			for (auto& edge : G.get_edges()) {
				if (this->generate_random_dbl(0.0, 1.0) <= p) {
					int left_vertex = sampled_graph.get_vertex_index(edge.first);
					int right_vertex = sampled_graph.get_vertex_index(edge.second);
					sampled_graph.add_new_edge(left_vertex, right_vertex, false);
				}
			}
			if (settings::compressed) {
				sampled_graph.compress_graph(false);
			}
			this->unnormalized_triangle_count = (double)counting::triangle::exact_edge_centeric_global_counting(sampled_graph);
			this->runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::colorful_counting(Graph& G, const int n_color) {
			/*
				Pagh, Rasmus, and Charalampos E. Tsourakakis.
				"Colorful triangle counting and a mapreduce implementation."
				Information Processing Letters 112, no. 7 (2012): 277-281.
			*/
			clock_t start_time = clock();
			std::vector <int> vertex_color;
			int n = G.get_n_vertices();
			for (int i = 0; i < n; i++) {
				vertex_color.push_back((int)this->generate_random_int(0, n_color - 1));
			}

			Graph sampled_graph = Graph();
			for (auto& edge : G.get_edges()) {
				if (vertex_color[edge.first] == vertex_color[edge.second]) {
					int left_vertex = sampled_graph.get_vertex_index(edge.first);
					int right_vertex = sampled_graph.get_vertex_index(edge.second);
					sampled_graph.add_new_edge(left_vertex, right_vertex, false);
				}
			}
			if (settings::compressed) {
				sampled_graph.compress_graph(false);
			}
			this->unnormalized_triangle_count = (double)counting::triangle::exact_edge_centeric_global_counting(sampled_graph);
			this->runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::ewsamp(Graph& G, const double p) {
			/*
				Türkoglu, Duru, and Ata Turk. 
				"Edge-based wedge sampling to estimate triangle counts in very large graphs." 
				In 2017 IEEE International Conference on Data Mining (ICDM), pp. 455-464. IEEE, 2017.
			*/
			clock_t start_time = clock();

			std::vector < std::pair<int, int> > sampled_edges;
			for (auto& edge : G.get_edges()) {
				if (this->generate_random_dbl(0.0, 1.0) <= p) {
					sampled_edges.push_back(edge);
				}
			}

			if (settings::compressed == false) {
				G.sort_adj_vectors_by_ids();
			}

			if (settings::compressed == true) {
				for (auto& edge : sampled_edges) {
					int u = edge.first;
					int v = edge.second;
					if (G.get_compressed_degree(u) < G.get_compressed_degree(v))
						std::swap(u, v);
					if (G.get_compressed_degree(v) <= 1)
						continue;
					int random_idx = (int)this->generate_random_int(G.get_compressed_offset(v), G.get_compressed_offset(v + 1) - 1 - 1);
					if (G.get_compressed_ith_neighbor(random_idx) >= u) // neighbors are sorted here. So this condition is fine to use.
						random_idx++;
					int w = G.get_compressed_ith_neighbor(random_idx);
					if (G.is_compressed_edge(u, w))
						this->unnormalized_triangle_count += G.get_degree(v) - 1;
				}
			}
			else {
				for (auto& edge : sampled_edges) {
					int u = edge.first;
					int v = edge.second;
					if (G.get_degree(u) < G.get_degree(v))
						std::swap(u, v);
					if (G.get_degree(v) <= 1)
						continue;
					int random_idx = (int)this->generate_random_int(0, G.get_degree(v) - 1 - 1);
					std::vector<int>& neighbors_of_v = G.get_adj_vec(v);
					if (neighbors_of_v[random_idx] >= u) // neighbors are sorted here. So this condition is fine to use.
						random_idx++;
					int w = neighbors_of_v[random_idx];
					if (G.is_edge(u, w))
						this->unnormalized_triangle_count += G.get_degree(v) - 1;
				}
			}
			this->runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}
		void algorithms::run(Graph& G) {
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				this->doulion(G, settings::p);
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				this->colorful_counting(G, settings::n_colors);
			}else if (settings::chosen_algo == ONE_SHOT_EDGE_WEDGE_SAMPLING) {
				this->ewsamp(G, settings::p);
			}
		}
		std::vector<double> algorithms::get_results(const int& exp_iter) {
			double unnormalized = this->get_unnormalized_count();
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				double global_triangle_count = utill::normalized(unnormalized, 1. / (settings::p * settings::p * settings::p));
				return { (double)exp_iter, global_triangle_count, this->get_runtime() };
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				double global_triangle_count = utill::normalized(unnormalized, (settings::n_colors * settings::n_colors));
				return { (double)exp_iter, global_triangle_count, this->get_runtime() };
			}
			else if (settings::chosen_algo == ONE_SHOT_EDGE_WEDGE_SAMPLING) {
				double global_triangle_count = utill::normalized(unnormalized, 1. / (settings::p * 3.0));
				return { (double)exp_iter, global_triangle_count, this->get_runtime() };
			}
			return {}; // should never happen.
		}
	}

	/* Static graph: Approximation by local sampling */
	namespace local_sampling {
		void algorithms::wedge_sampling(Graph& G) {
			/*
				Seshadhri, C., Ali Pinar, and Tamara G. Kolda.
				"Wedge sampling for computing clustering coefficients and triangle counts on large graphs."
				Statistical Analysis and Data Mining: The ASA Data Science Journal 7, no. 4 (2014): 294-307.

				Seshadhri, Comandur, Ali Pinar, and Tamara G. Kolda.
				"Fast triangle counting through wedge sampling."
				In Proceedings of the SIAM Conference on Data Mining, vol. 4, p. 5. 2013.
			*/
			clock_t start_time = clock();
			this->n_sampled++;
			const std::vector<long long>& wedges = G.get_wedges();
			const auto pair = this->weighted_sampling(wedges);
			int center = pair.first;
			long long rand_number = pair.second;
			auto wedge = G.get_wedge(rand_number, center, G.get_adj_vec(center));
			if (settings::compressed == true) {
				this->unnormalized_triangle_count += G.is_compressed_edge(wedge[0], wedge[2]) ? 1.0 : 0.0;
			}
			else {
				this->unnormalized_triangle_count += G.is_edge(wedge[0], wedge[2]) ? 1.0 : 0.0; // check if wedge is closed (i.e. if it forms a triangle)
			}
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::revisited_wedge_sampling(Graph& G) {
			/*
				Turk, Ata, and Duru Turkoglu. 
				"Revisiting Wedge Sampling for Triangle Counting." 
				In The World Wide Web Conference, pp. 1875-1885. ACM, 2019.
			*/
			clock_t start_time = clock();
			this->n_sampled++;
			const std::vector<long long>& wedges = G.get_wedges();
			const auto pair = this->weighted_sampling(wedges);
			int center = pair.first;
			long long rand_number = pair.second;
			auto wedge = G.get_wedge(rand_number, center, G.get_ordered_adj_vec(center)); // note that the neighbors are ordered.
			if (settings::compressed == true) {
				this->unnormalized_triangle_count += G.is_compressed_edge(wedge[0], wedge[2]) ? 1.0 : 0.0;
			}
			else {
				this->unnormalized_triangle_count += G.is_edge(wedge[0], wedge[2]) ? 1.0 : 0.0; // check if wedge is closed (i.e. if it forms a triangle)
			}
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::run(Graph& G) {
			if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
				this->wedge_sampling(G);
			}
			else if (settings::chosen_algo == LOCAL_REVISITED_WEDGE_SAMPLING) {
				this->revisited_wedge_sampling(G);
			}
		}
		std::vector<double> algorithms::get_results(const int& iter_exp, Graph& G) {
			double unnormalized = this->get_unnormalized_count();
			if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
				double global_triangle_count = utill::normalized(unnormalized, (double)G.get_n_wedges() / (this->get_n_sampled() * 3.0));
				return { (double)iter_exp, this->get_preprocessing_time(), (double)this->get_n_sampled(), global_triangle_count, this->get_runtime() };
			}
			else if (settings::chosen_algo == LOCAL_REVISITED_WEDGE_SAMPLING) {
				double global_triangle_count = utill::normalized(unnormalized, (double)G.get_n_wedges() / (this->get_n_sampled()));
				return { (double)iter_exp, this->get_preprocessing_time(), (double)this->get_n_sampled(), global_triangle_count, this->get_runtime() };
			}
			else
				return {};
		}
	}
}


/* Streamming algorithm */
namespace streamming {
	namespace one_pass {
		void algorithms::triest_base(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step) {
			/*
				Stefani, Lorenzo De, Alessandro Epasto, Matteo Riondato, and Eli Upfal.
				"Triest: Counting local and global triangles in fully dynamic streams with fixed memory size."
				ACM Transactions on Knowledge Discovery from Data (TKDD) 11, no. 4 (2017): 43.
			*/
			clock_t start_time = clock();
			if (time_step <= reservoir_size) {
				reservoir.add_new_edge(new_edge, false);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			}
			else {
				if (this->generate_random_dbl(0., 1.) <= (double)reservoir_size / time_step) {
					int remove_edge_index = (int)this->generate_random_int(0, (int)reservoir.get_n_edges() - 1);
					this->unnormalized_triangle_count -= counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, reservoir.get_ith_edge(remove_edge_index));
					reservoir.replace_edge(new_edge, remove_edge_index, false);
					this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, reservoir.get_ith_edge(remove_edge_index));
				}
			}
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			// TODO: triangle_count = unnormalized_triangle_count * ??
		}

		void algorithms::triest_impr(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step) {
			/*
				Stefani, Lorenzo De, Alessandro Epasto, Matteo Riondato, and Eli Upfal.
				"Triest: Counting local and global triangles in fully dynamic streams with fixed memory size."
				ACM Transactions on Knowledge Discovery from Data (TKDD) 11, no. 4 (2017): 43.
			*/
			clock_t start_time = clock();
			double eta = mmax(1., ((double)time_step * (time_step - 1)) / ((double)reservoir_size * (reservoir_size - 1)));
			this->unnormalized_triangle_count += eta * counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			if (time_step <= reservoir_size) {
				reservoir.add_new_edge(new_edge, false);
			}
			else {
				if (this->generate_random_dbl(0., 1.) <= (double)reservoir_size / time_step) {
					int remove_edge_index = (int)this->generate_random_int(0, (int)reservoir.get_n_edges() - 1);
					reservoir.replace_edge(new_edge, remove_edge_index, false);
				}
			}
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			// TODO: triangle count = unnormalized_triangle_count / p^3
		}

		void algorithms::mascot_C(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge) {
			/*
				Lim, Yongsub, and U. Kang.
				"Mascot: Memory-efficient and accurate sampling for counting local triangles in graph streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			clock_t start_time = clock();
			if (this->generate_random_dbl(0.0, 1.0) <= p) {
				reservoir.add_new_edge(new_edge, false);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			}
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}

		void algorithms::mascot(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge) {
			/*
				Lim, Yongsub, and U. Kang.
				"Mascot: Memory-efficient and accurate sampling for counting local triangles in graph streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			clock_t start_time = clock();
			this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			if (this->generate_random_dbl(0.0, 1.0) <= p) {
				reservoir.add_new_edge(new_edge, false);
			}
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			// TODO: triangle count = unnormalized_triangle_count / p^2
		}
	}
}