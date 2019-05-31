#pragma once

#include "Graph.h"
#include "counting.h"
#include "sampler.h"

namespace base_algorithm_class {
	class algorithms : public sampler {
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
		void reset() {
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
		void edge_centric_exact_algorithm(Graph& G);
	};
}

namespace static_processing {
	namespace one_shot {
		class algorithms : public base_algorithm_class::algorithms {
		public:
			/* Static graph: Approximation by one shot sampling */
			void doulion(Graph& G, const double p);
			void colorful_counting(Graph& G, const int n_color);
		};
	}

	namespace local_sampling {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			double interval_time;
			double last_time_printed;
			int n_sampled;
		public:
			inline void setup() {
				this->n_sampled = 0;
				this->last_time_printed = 0;
				this->interval_time = settings::max_time / settings::snapshots;
			}
			inline void update_last_time_printed(const double& current_time) {
				this->last_time_printed = current_time;
			}
			inline bool should_print(const double& current_time) {
				return current_time - this->last_time_printed > this->interval_time;
			}
			inline int get_n_sampled() {
				return this->n_sampled; 
			}
			/* Static graph: Approximation by local sampling */
			void wedge_sampling(Graph& G);
		};
	}
}


namespace streamming{
	namespace one_pass {
		class algorithms : public base_algorithm_class::algorithms {
		public:
			void triest_base(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step);
			void triest_impr(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step);
			void mascot_C(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge);
			void mascot(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge);
		};
	}
}


