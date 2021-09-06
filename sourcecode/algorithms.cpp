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
			this->pre_runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			if (settings::compressed) {
				sampled_graph.compress_graph();
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
			this->pre_runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			if (settings::compressed) {
				sampled_graph.compress_graph();
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
			this->pre_runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			if (settings::compressed == false) {
				G.sort_adj_vectors_by_ids();
			}
			//if (settings::compressed == true) {
			//	for (auto& edge : sampled_edges) {
			//		int u = edge.first;
			//		int v = edge.second;
			//		if (G.get_compressed_degree(u) < G.get_compressed_degree(v))
			//			std::swap(u, v);
			//		if (G.get_compressed_degree(v) <= 1)
			//			continue;
			//		int random_idx = (int)this->generate_random_int(G.get_compressed_offset(v), G.get_compressed_offset(v + 1) - 1 - 1);
			//		if (G.get_compressed_ith_neighbor(random_idx) >= u) // neighbors are sorted here. So this condition is fine to use.
			//			random_idx++;
			//		int w = G.get_compressed_ith_neighbor(random_idx);
			//		if (G.is_compressed_edge(u, w))
			//			this->unnormalized_triangle_count += G.get_degree(v) - 1;
			//	}
			//}
			//else {
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
			//}
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
				return { (double)exp_iter, this->get_preprocessing_time(), global_triangle_count, this->get_runtime() };
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				double global_triangle_count = utill::normalized(unnormalized, (settings::n_colors * settings::n_colors));
				return { (double)exp_iter, this->get_preprocessing_time(), global_triangle_count, this->get_runtime() };
			}
			else if (settings::chosen_algo == ONE_SHOT_EDGE_WEDGE_SAMPLING) {
				double global_triangle_count = utill::normalized(unnormalized, 1. / (settings::p * 3.0));
				return { (double)exp_iter, this->get_preprocessing_time(), global_triangle_count, this->get_runtime() };
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

		void algorithms::setup(Graph& G, const int& iter_exp) {
			if (iter_exp == 1) {
				double start_time = clock();
				if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
					G.process_wedges();
				}
				else if (settings::chosen_algo == LOCAL_REVISITED_WEDGE_SAMPLING) {
					G.sort_vertices_by_degree();
					G.process_ordered_wedges();
				}
				this->pre_runtime = ((double)clock() - start_time) / CLOCKS_PER_SEC;
				this->interval_time = settings::max_time / settings::snapshots;
			}
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

		void algorithms::exact_ins_only(const int& A, const int& B) {
			R.add_edge_to_reservoir(A, B);
			this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
		}

		void algorithms::triest_base(const int& A, const int& B) {
			/*
				Stefani, Lorenzo De, Alessandro Epasto, Matteo Riondato, and Eli Upfal.
				"Triest: Counting local and global triangles in fully dynamic streams with fixed memory size."
				ACM Transactions on Knowledge Discovery from Data (TKDD) 11, no. 4 (2017): 43.
			*/
			if (time_step <= R.get_maximum_size()) {
				R.add_edge_to_reservoir(A, B);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
			}
			else {
				if (this->generate_random_dbl(0.0, 1.0) <= (double)R.get_current_size() / time_step) {
					auto rem_edge = this->R.remove_random_edge_from_reservoir();
					this->unnormalized_triangle_count -= counting::triangle::exact_edge_centeric_local_edge_counting(R, rem_edge.first, rem_edge.second);
					this->R.add_edge_to_reservoir(A, B);
					this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
				}
			}
		}

		void algorithms::triest_impr(const int& A, const int& B) {
			/*
				Stefani, Lorenzo De, Alessandro Epasto, Matteo Riondato, and Eli Upfal.
				"Triest: Counting local and global triangles in fully dynamic streams with fixed memory size."
				ACM Transactions on Knowledge Discovery from Data (TKDD) 11, no. 4 (2017): 43.
			*/
			double eta = mmax(1.0, ((double)time_step / (double)this->R.get_maximum_size()) * (((double)(time_step - 1.0)) / (R.get_maximum_size() - 1.0)));
			this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B) * eta;
			if (time_step <= R.get_maximum_size()) {
				R.add_edge_to_reservoir(A, B);
			}
			else {
				if (this->generate_random_dbl(0.0, 1.0) <= (double)this->R.get_maximum_size() / time_step) {
					R.replace_in_reservoir(A, B);
				}
			}
		}

		void algorithms::mascot_C(const int& A, const int& B) {
			/*
				Lim, Yongsub, and U. Kang.
				"Mascot: Memory-efficient and accurate sampling for counting local triangles in graph streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			if (this->generate_random_dbl(0.0, 1.0) <= settings::p) {
				R.add_edge_to_reservoir(A, B);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
			}
		}

		void algorithms::mascot(const int& A, const int& B) {
			/*
				Lim, Yongsub, and U. Kang.
				"Mascot: Memory-efficient and accurate sampling for counting local triangles in graph streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
			if (this->generate_random_dbl(0.0, 1.0) <= settings::p) {
				R.add_edge_to_reservoir(A, B);
			}
		}

		void algorithms::gps(const int& A, const int& B) {
			/*
				Nesreen K. Ahmed, Nick Duffield, Theodore L. Willke, Ryan A. Rossi
				"On Sampling from Massive Graph Streams."
				In Proceedings of the 21th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining, pp. 685-694. ACM, 2015.
			*/
			this->unnormalized_triangle_count += counting::triangle::edge_centeric_local_edge_counting_by_martingle(R, A, B, lookup_weight, z_star);
			double edge_weight = 9.0 * counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B) + 1; // this is suggested in VLDB'17, due to Ahmed et al.
			double ran_num = sampler::generate_random_dbl(0.0 + 1e-10, 1.0); // we add 1e-10 to avoid division by zero in the next line
			double rank = edge_weight / ran_num;
			auto edge_hash = this->R.to_hash(A, B);
			std::pair < double, std::pair<int,int> > paired_edge = { rank,{ A, B } };

			if (this->time_step <= this->R.get_maximum_size()) {
				this->rank_set.insert(paired_edge);
				this->R.add_edge_to_reservoir(A, B);
				this->lookup_weight[edge_hash] = edge_weight;
				return;
			}

			if (paired_edge < *rank_set.begin()) {
				this->z_star = mmax(this->z_star, rank);
				return;
			}

			int rem_A = (*rank_set.begin()).second.first;
			int rem_B = (*rank_set.begin()).second.second;
			auto rem_hash = this->R.to_hash(rem_A, rem_B);

			this->R.remove_edge_from_reservoir(rem_A, rem_B);
			this->lookup_weight.erase(rem_hash);
			this->z_star = mmax(this->z_star, (*rank_set.begin()).first);
			this->rank_set.erase(rank_set.begin());

			this->R.add_edge_to_reservoir(A, B);
			this->lookup_weight[edge_hash] = edge_weight;
			this->rank_set.insert(paired_edge);
		}


		// ---------------------- fully dynamic algorithms ----------------------------------
		void algorithms::exact_full_dyn(const int& A, const int& B, bool op) {
			if (op == true) {
				this->R.add_edge_to_reservoir(A, B);
				this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
			} else {
				this->R.remove_edge_from_reservoir(A, B);
				this->unnormalized_triangle_count -= counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
			}
		}
		void algorithms::triest_fd(const int& A, const int& B, bool op) {
			if (op == true) {
				if (this->n_b + this->n_g == 0) {
					if (this->R.get_current_size() < this->R.get_maximum_size()) {
						this->R.add_edge_to_reservoir(A, B);
						this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
					}
					else {
						double tossed_prob = sampler::generate_random_dbl(0.0, 1.0);
						if (tossed_prob * this->stream_current_size <= this->R.get_maximum_size()) {
							auto rem_edge = this->R.remove_random_edge_from_reservoir();
							this->unnormalized_triangle_count -= counting::triangle::exact_edge_centeric_local_edge_counting(R, rem_edge.first, rem_edge.second);
							this->R.add_edge_to_reservoir(A, B);
							this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
						}
					}
				}
				else {
					double tossed_prob = sampler::generate_random_dbl(0.0, 1.0);
					if (tossed_prob <= ((double)this->n_b) / ((double)this->n_b + this->n_g)) {
						this->R.add_edge_to_reservoir(A, B);
						this->unnormalized_triangle_count += counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
						this->n_b--;
					}
					else {
						this->n_g--;
					}
				}
			}
			else { // op is false, which means an edge deletion occured.
				if (this->R.remove_edge_from_reservoir(A, B) == true) {
					this->unnormalized_triangle_count -= counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B);
					this->n_b++; // bad uncompensated deletion as the edge appears in the stream
				}
				else
					this->n_g++; // good uncompensated deletion
			}
		}
		void algorithms::thinkd_acc(const int& A, const int& B, bool op) {
			double y = mmin(this->R.get_maximum_size(), this->stream_current_size + this->n_b + this->n_g);
			if (this->stream_current_size >= 2) {
				double prob = 1.0;
				for (int i = 0; i < 2; i++) {
					prob *= ((y - i) / ((double)this->stream_current_size + this->n_b + this->n_g - i));
				}
				this->unnormalized_triangle_count += (op ? +1 : -1) * counting::triangle::exact_edge_centeric_local_edge_counting(R, A, B) / prob;
			}
			else
				this->unnormalized_triangle_count = 0;
			if (op == true) {
				if (this->n_b + this->n_g == 0) {
					if (this->R.get_current_size() < this->R.get_maximum_size()) this->R.add_edge_to_reservoir(A, B);
					else {
						double tossed_prob = sampler::generate_random_dbl(0.0, 1.0);
						if (tossed_prob * this->stream_current_size <= this->R.get_maximum_size())
							this->R.replace_in_reservoir(A, B);
					}
				}
				else {
					double tossed_prob = sampler::generate_random_dbl(0.0, 1.0);
					if (tossed_prob <= ((double)this->n_b) / ((double)this->n_b + this->n_g)) {
						this->R.add_edge_to_reservoir(A, B);
						this->n_b--;
					}
					else {
						this->n_g--;
					}
				}
			}
			else {
				if (this->R.remove_edge_from_reservoir(A, B) == true)
					this->n_b++; // bad uncompensated deletion as the edge appears in the stream
				else
					this->n_g++; // good uncompensated deletion
			}
		}
		void algorithms::run(const int& A, const int& B, const bool op) {
			this->time_step++;
			this->stream_current_size += (op ? 1 : -1);
			if (settings::chosen_algo == EXACT_INS_ONLY) {
				double start_time = (double)clock();
				this->exact_ins_only(A, B);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == EXACT_FULL_DYN) {
				double start_time = (double)clock();
				this->exact_full_dyn(A, B, op);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == STREAMING_TRIEST_BASE) {
				double start_time = (double)clock();
				this->triest_base(A, B);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == STREAMING_TRIEST_IMPR) {
				double start_time = (double)clock();
				this->triest_impr(A, B);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == STREAMING_MASCOT) {
				double start_time = (double)clock();
				this->mascot(A, B);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == STREAMING_MASCOT_C) {
				double start_time = (double)clock();
				this->mascot_C(A, B);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == GPS) {
				double start_time = (double)clock();
				this->gps(A, B);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == STREAMING_TRIEST_FD) {
				double start_time = (double)clock();
				this->triest_fd(A, B, op);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
			else if (settings::chosen_algo == THINKD_ACC) {
				double start_time = (double)clock();
				this->thinkd_acc(A, B, op);
				this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			}
		}
		std::vector<double> algorithms::get_results(int exp_iter = -1) {
			double unnormalized = this->get_unnormalized_count();
			double global_triangle_count = 0;
			if (settings::is_exact_algorithm() == true) {
				global_triangle_count = unnormalized;
				return{ global_triangle_count, this->get_runtime(), (double)this->time_step };
			}
			else {
				if (settings::chosen_algo == STREAMING_MASCOT) {
					global_triangle_count = (unnormalized / settings::p) *  (1.0 / settings::p);
				} 
				else if (settings::chosen_algo == STREAMING_MASCOT_C) {
					global_triangle_count = unnormalized;
					for(int i = 0; i < 3; i ++)
						global_triangle_count /= settings::p;
				} 
				else if(settings::chosen_algo == STREAMING_TRIEST_BASE) {
					double eta = 1.0;
					if (this->time_step > this->R.get_maximum_size()) {
						for (int i = 0; i < 3; i++)
							eta *= ((double)(this->time_step - i)) / ((double)(this->R.get_maximum_size() - i));
					}
					global_triangle_count = unnormalized * eta;
				} 
				else if(settings::chosen_algo == STREAMING_TRIEST_IMPR || settings::chosen_algo == GPS || settings::chosen_algo == THINKD_ACC) {
					global_triangle_count = unnormalized;
				}
				else if (settings::chosen_algo == STREAMING_TRIEST_FD) {
					int& Mt = this->R.get_current_size();
					if (Mt >= 3 && this->stream_current_size >= 3) {
						int wt = mmin(this->R.get_maximum_size(), this->stream_current_size + this->n_b + this->n_g);
						boost::math::hypergeometric_distribution<double> hyper(this->stream_current_size, wt, this->n_g + this->n_b + this->stream_current_size);
						double kt = 0.0;
						for (int i = 0; i < 3; i++) {
							if (i >= mmax(0, wt - this->n_b - this->n_g) && i <= mmin(wt, this->stream_current_size)) {
								kt += boost::math::pdf<double>(hyper, i);
							}
						}
						double mul_factor = 1.0;
						for (int i = 0; i < 3; i++) {
							mul_factor *= (Mt - i + 0.0) / (this->stream_current_size - i + 0.0);
						}
						//std::cout << kt << std::endl;
						//std::cout << mul_factor << std::endl;
						//std::cout << unnormalized << std::endl;
						//std::cout << " ************************ " << std::endl;
						mul_factor *= (1.0 - kt);
						if (mul_factor != 0)
							global_triangle_count = unnormalized / mul_factor;
						else
							global_triangle_count = 0.0;
					}
					else
						global_triangle_count = 0.0;
				}
				return{ (double)exp_iter, global_triangle_count, this->get_runtime(), (double)this->time_step, (double)this->R.get_current_size() };
			}
		}
	}
}
