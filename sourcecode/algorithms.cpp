#include "algorithms.h"

/* Static graph: Approximation by one shot sampling */
namespace static_processing {
	namespace one_shot {
		void algorithms::doulion(Graph& G, const double p) {
			/*
				Tsourakakis, Charalampos E., U.Kang, Gary L.Miller, and Christos Faloutsos.
				"Doulion: counting triangles in massive graphs with a coin."
				In Proceedings of the 15th ACM SIGKDD international conference on Knowledge discovery and data mining, pp. 837 - 846. ACM, 2009.
			*/
			Graph sampled_graph;
			for (auto& edge : G.get_edges()) {
				if (this->generate_random_dbl(0.0, 1.0) <= p) {
					sampled_graph.add_new_edge(edge, false);
				}
			}
			this->unnormalized_triangle_count = counting::triangle::exact_edge_centeric_global_counting(sampled_graph);
			return;
		}

		void algorithms::colorful_counting(Graph& G, const int n_color) {
			/*
				Pagh, Rasmus, and Charalampos E. Tsourakakis.
				"Colorful triangle counting and a mapreduce implementation."
				Information Processing Letters 112, no. 7 (2012): 277-281.
			*/
			std::vector <int> vertex_color;
			int n = G.get_n_vertices();
			for (int i = 0; i < n; i++) {
				vertex_color.push_back(this->generate_random_int(0, n_color - 1));
			}

			Graph sampled_graph;
			for (auto& edge : G.get_edges()) {
				if (vertex_color[edge.first] == vertex_color[edge.second]) {
					sampled_graph.add_new_edge(edge, false);
				}
			}
			this->unnormalized_triangle_count = (double)counting::triangle::exact_edge_centeric_global_counting(sampled_graph);
			return;
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
			std::vector<long long>& wedges = G.get_wedges();
			auto& pair = this->weighted_sampling(wedges);
			auto& wedge = G.get_wedge(pair.second, pair.first);
			this->unnormalized_triangle_count += G.get_adj_set(wedge[0]).find(wedge[2]) != G.get_adj_set(wedge[0]).end() ? 1.0 : 0.0; // check if wedge is closed (i.e. if it forms a triangle)
			// unnormalized_triangle_count ???
			return;
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
			if (time_step <= reservoir_size) {
				reservoir.add_new_edge(new_edge, false);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			}
			else {
				if (this->generate_random_dbl(0., 1.) <= (double)reservoir_size / time_step) {
					int remove_edge_index = this->generate_random_int(0, (int)reservoir.get_n_edges() - 1);
					this->unnormalized_triangle_count -= counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, reservoir.get_ith_edge[remove_edge_index]);
					reservoir.replace_edge(new_edge, remove_edge_index, false);
					this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, reservoir.get_ith_edge[remove_edge_index]);
				}
			}
			// TODO: triangle_count = unnormalized_triangle_count * ??
		}

		void algorithms::triest_impr(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step) {
			/*
				Stefani, Lorenzo De, Alessandro Epasto, Matteo Riondato, and Eli Upfal.
				"Triest: Counting local and global triangles in fully dynamic streams with fixed memory size."
				ACM Transactions on Knowledge Discovery from Data (TKDD) 11, no. 4 (2017): 43.
			*/
			double eta = mmax(1., ((double)time_step * (time_step - 1)) / ((double)reservoir_size * (reservoir_size - 1)));
			this->unnormalized_triangle_count += eta * counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			if (time_step <= reservoir_size) {
				reservoir.add_new_edge(new_edge, false);
			}
			else {
				if (this->generate_random_dbl(0., 1.) <= (double)reservoir_size / time_step) {
					int remove_edge_index = this->generate_random_int(0, (int)reservoir.get_n_edges() - 1);
					reservoir.replace_edge(new_edge, remove_edge_index, false);
				}
			}
			// TODO: triangle count = unnormalized_triangle_count / p^3
		}

		void algorithms::mascot_C(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge) {
			/*
				Lim, Yongsub, and U. Kang.
				"Mascot: Memory-efficient and accurate sampling for counting local triangles in graph streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			if (this->generate_random_dbl(0.0, 1.0) <= p) {
				reservoir.add_new_edge(new_edge, false);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			}
		}

		void algorithms::mascot(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge) {
			/*
				Lim, Yongsub, and U. Kang.
				"Mascot: Memory-efficient and accurate sampling for counting local triangles in graph streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(reservoir, new_edge);
			if (this->generate_random_dbl(0.0, 1.0) <= p) {
				reservoir.add_new_edge(new_edge, false);
			}
			// TODO: triangle count = unnormalized_triangle_count / p^2
		}
	}
}