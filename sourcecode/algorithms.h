#pragma once

#include "Graph.h"
#include "counting.h"
#include "sampler.h"

namespace base_algorithm_class {
	class algorithms {
	protected:
		double unnormalized_triangle_count;
		double runtime;
	public:
		algorithms() {
			this->reset();
		}
		inline double get_runtime() {
			return this->runtime;
		}
		virtual void reset() {
			this->unnormalized_triangle_count = 0;
			this->runtime = 0;
		}
		double get_unnormalized_count() {
			return this->unnormalized_triangle_count;
		}
	};
}

namespace exact {
	class algorithms : public base_algorithm_class::algorithms {
	public:
		void exact_algorithm(Graph& G);
	};
}

namespace static_processing {
	namespace one_shot {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			/* Static graph: Approximation by one shot sampling */
			void doulion(Graph& G);
			void colorful_counting(Graph& G);
			double graph_construction_time;
			double counting_on_sampled_graph_time;
		public:
			void run(Graph& G);
			void reset() override {
				base_algorithm_class::algorithms::reset();
				this->graph_construction_time = 0;
				this->counting_on_sampled_graph_time = 0;
			}
			inline double get_construction_time() {
				return this->graph_construction_time;
			}
			inline double get_counting_on_sampled_graph_time() {
				return this->counting_on_sampled_graph_time;
			}
			std::vector<double> get_results(const int& iter_exp);
		};
	}

	namespace local_sampling {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			double preprocessing_time;
			double interval_time;
			double last_time_printed;
			long long n_sampled;
			/* Static graph: Approximation by local sampling */
			void fast_edge_sampling(Graph& G);
			void fast_centered_edge_sampling(Graph& G);
			void wedge_sampling(Graph& G);
			void path_sampling(Graph& G);
			void path_centered_sampling(Graph& G);
			void path_centered_sampling_by_wedge(Graph& G);
			void path_centered_sampling_by_degeneracy(Graph& G);
		public:
			void setup(const int& iter_exp, Graph& G) {
				this->reset();
				this->n_sampled = 0;
				this->last_time_printed = 0;
				this->interval_time = settings::max_time / settings::snapshots;
				
				if (iter_exp == 1) {
					if (settings::chosen_algo == FAST_EDGE_SAMPLING) {
						this->preprocessing_time = 0;
					}
					else if (settings::chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
						clock_t start_time = clock();
						G.sort_vertices_by_degree(); // Question: why edges are kept the same as vanilla fast edge sampling?
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == WEDGE_SAMPLING) {
						clock_t start_time = clock();
						G.get_n_w();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_SAMPLING) {
						clock_t start_time = clock();
						G.get_n_z();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_CENTERED_SAMPLING) {
						clock_t start_time = clock();
						G.sort_vertices_by_degree();
						G.get_n_centered_z();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_WEDGE) {
						clock_t start_time = clock();
						G.get_n_z();
						G.sort_vertices_by_wedges();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_DEGENERACY) {
						clock_t start_time = clock();
						G.sort_vertices_by_degeneracy();
						G.get_n_centered_z();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
				}
			}
			inline void update_last_time_printed(const double& current_time) {
				this->last_time_printed = current_time;
			}
			inline bool should_print(const double& current_time) {
				return current_time - this->last_time_printed >= this->interval_time;
			}
			inline double get_preprocessing_time() {
				return this->preprocessing_time;
			}
			inline long long get_n_sampled() {
				return this->n_sampled; 
			}
			void run(Graph& G);
			std::vector<double> get_results(const int& iter_exp, Graph& G);
		};
	}
}
