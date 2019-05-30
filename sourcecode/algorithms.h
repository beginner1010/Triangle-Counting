#pragma once

#include "Graph.h"
#include "counting.h"
#include "sampler.h"

namespace base_algorithm_class {
	class algorithms : public sampler {
	protected:
		double unnormalized_triangle_count;
	public:
		algorithms() {
			this->reset();
		}
		void reset() {
			this->unnormalized_triangle_count = 0;
		}
		double get_unnormalized_count() {
			return this->unnormalized_triangle_count;
		}
	};
}

namespace static_processing {
	namespace one_shot {
		class algorithms : public base_algorithm_class::algorithms {
		public:
			//algorithms() {
			//	this->reset();
			//}
			/* Static graph: Approximation by one shot sampling */
			void doulion(Graph& G, const double p, bool compressed = false);
			void colorful_counting(Graph& G, const int n_color, bool compressed = false);
		};
	}

	namespace local_sampling {
		class algorithms : public base_algorithm_class::algorithms {
		public:
			//algorithms() {
			//	this->reset();
			//}
			/* Static graph: Approximation by local sampling */
			void wedge_sampling(Graph& G, bool compressed = false);
		};
	}
}


namespace streamming{
	namespace one_pass {
		class algorithms : public base_algorithm_class::algorithms {
		public:
			//algorithms() {
			//	this->reset();
			//}
			void triest_base(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step);
			void triest_impr(Graph& reservoir, const int reservoir_size, const std::pair<int, int>& new_edge, const int& time_step);
			void mascot_C(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge);
			void mascot(Graph& reservoir, const double& p, const std::pair<int, int>& new_edge);
		};
	}
}


