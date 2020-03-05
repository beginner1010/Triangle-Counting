#pragma once

#include <set>

#include "Graph.h"
#include "counting.h"
#include "sampler.h"
#include "reservoir.h"

#include <boost/math/distributions/hypergeometric.hpp>
#include <boost/math/distributions/hypergeometric.hpp>
#include <boost/math/policies/policy.hpp>

namespace base_algorithm_class {
	class algorithms : public sampler {
	protected:
		double unnormalized_triangle_count;
		double runtime;
		double pre_runtime;
	public:
		algorithms() {
			this->reset();
		}
		inline double get_runtime() {
			return this->runtime;
		}
		inline double get_preprocessing_time() {
			return this->pre_runtime;
		}
		void reset() {
			this->unnormalized_triangle_count = 0;
			this->runtime = 0;
			this->pre_runtime = 0;
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
			void ewsamp(Graph& G, const double p);
			void run(Graph& G);
			std::vector<double> get_results(const int& exp_iter);
		};
	}

	namespace local_sampling {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			double interval_time;
			double last_time_printed;
			int n_sampled;
		public:
			void setup(Graph& G, const int& iter_exp);
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
			void revisited_wedge_sampling(Graph& G);

			void run(Graph& G);
			std::vector<double> get_results(const int& iter_exp, Graph& G);
			std::vector<double> get_results(const int& exp_iter);
			virtual void reset() {
				base_algorithm_class::algorithms::reset();
				this->last_time_printed = 0;
				this->n_sampled = 0;
			}
		};
	}
}

namespace streamming {
	namespace one_pass {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			reservoir R;
			int time_step;
			int stream_size;
			int interval_batch;
			int stream_current_size;
		public:
			void reset(Graph& G) {
				base_algorithm_class::algorithms::reset();
				this->R.clear();
				this->R.set_max_size(settings::reservoir_size);
				this->time_step = 0;
				if (settings::is_streaming_insertion_only() == true) this->stream_size = (int)G.get_insertion_only_stream().size();
				else this->stream_size = (int)G.get_fully_dynamic_stream().size();
				settings::p = mmin(1.0, ((double)settings::reservoir_size) / (this->stream_size + 0.0));
				// for algorithm GPS
				this->rank_set.clear();
				this->lookup_weight.clear();
				this->z_star = 0;
				// for random pairing sampling, used in triest-fd and thinkd-acc
				this->n_b = 0;
				this->n_g = 0;
				this->current_n_edges = 0;
				this->stream_current_size = 0;
				// end!
			}
			inline void set_interval(Graph& G) {
				if (settings::is_streaming_insertion_only())
					this->interval_batch = mmax(1, ((int)G.get_insertion_only_stream().size()) / settings::snapshots);
				else
					this->interval_batch = mmax(1, ((int)G.get_fully_dynamic_stream().size()) / settings::snapshots);
			}
			inline bool should_print() {
				return this->time_step == this->stream_size || this->interval_batch == 1 || (this->time_step > 0 && this->time_step % this->interval_batch == 0);
			}
			// ---------- insertion only algorithms ----------------
			void exact_ins_only(const int& A, const int& B);
			void triest_base(const int& A, const int& B);
			void triest_impr(const int& A, const int& B);
			void mascot(const int& A, const int& B);
			void mascot_C(const int& A, const int& B);
			void gps(const int& A, const int& B);
			// the following variables are used for GPS algorithm
			std::set < std::pair< double, std::pair <int, int> > > rank_set;
			std::unordered_map <long long, double> lookup_weight;
			double z_star;
			// ---------- fully dynamic -----------------------------
			void exact_full_dyn(const int& A, const int& B, bool op);
			void triest_fd(const int& A, const int& B, bool op);
			void thinkd_acc(const int& A, const int& B, bool op);
			// the following variables are used in triest_fd and thinkd_acc algorithms
			int n_b, n_g;
			int current_n_edges;
			// end!
			void run(const int& A, const int& B, const bool op);
			std::vector<double> get_results(int exp_iter);
		};
	}
}