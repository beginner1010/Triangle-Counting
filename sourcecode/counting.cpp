#include "counting.h"

namespace exact {
	namespace global {
		long long kai_exact_count(Graph& g) {
			long long ans = 0;
			std::vector<int>& start_vertices = g.get_vertices();
			std::vector<int> counter(g.get_n_vertices(), 0);
			for (auto& start : start_vertices) {
				std::vector<int> seen_vertices;
				std::vector<int>& neighbors_of_start = g.get_adj_vec(start);
				int degree_start = (int)neighbors_of_start.size();
				for (auto& middle : neighbors_of_start) {
					std::vector<int>& neighbors_of_middle = g.get_adj_vec(middle);
					int degree_middle = (int)neighbors_of_middle.size();
					if (degree_start > degree_middle || (degree_start == degree_middle && start > middle)) {
						for (auto& end : neighbors_of_middle) {
							int degree_end = (int)g.get_adj_vec(end).size();
							if (degree_start > degree_end || (degree_start == degree_end && start > end)) {
								ans += counter[end];
								counter[end] ++;
								if (counter[end] == 1) {
									seen_vertices.push_back(end);
								}
							}
						}
					}
				}
				for (auto& seen_vertex : seen_vertices)
					counter[seen_vertex] = 0;
			}
			return ans;
		}
	}

	namespace wedge {
		int bfly_in_wedge(Graph& g, std::vector<int>& wedge) {
			int degree_1 = g.degree(wedge[0]) - 1;
			int degree_2 = g.degree(wedge[2]) - 1;

			std::vector<int>& adj = degree_1 < degree_2 ? g.get_adj_vec(wedge[0]) : g.get_adj_vec(wedge[2]);
			std::unordered_set<int> &other = degree_1 >= degree_2 ? g.get_adj_set(wedge[0]) : g.get_adj_set(wedge[2]);

			int bfly = 0;
			for (auto &neighbor : adj) {
				if (other.find(neighbor) != other.end())
					bfly++;
			}
			return bfly;
		}
	}
}

namespace randomized {
	namespace edge {
		long double randomized_ebfc(Graph& g, int& vertex_a, int& vertex_b) {
			int degree_a = (int)g.get_adj_vec(vertex_a).size();
			int degree_b = (int)g.get_adj_vec(vertex_b).size();
			if (degree_a == 0 || degree_b == 0)
				return 0;
			long double res = 0;
			for (int iter = 0; iter < constants::ebfc_iterations; iter++) {
				int& neighbor_a = g.get_adj_vec(vertex_a)[(int)sampler::generate_random_int(0, degree_a - 1)];
				int& neighbor_b = g.get_adj_vec(vertex_b)[(int)sampler::generate_random_int(0, degree_b - 1)];
				if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
					res++;
				}
			}
			res /= constants::ebfc_iterations;
			res *= ((long double)degree_a) * (degree_b);
			return res;
		}

		long double randomized_centered_ebfc(Graph& g, int& vertex_a, int& vertex_b) {
			std::vector<int>& adj_a = g.get_adj_vec(vertex_a);
			std::vector<int>& adj_b = g.get_adj_vec(vertex_b);
			int index_b_in_adj_a = (int)(std::lower_bound(adj_a.begin(), adj_a.end(), vertex_b) - adj_a.begin());
			int index_a_in_adj_b = (int)(std::lower_bound(adj_b.begin(), adj_b.end(), vertex_a) - adj_b.begin());
			int degree_a = g.degree(vertex_a) - index_b_in_adj_a - 1;
			int degree_b = g.degree(vertex_b) - index_a_in_adj_b - 1;


			if (degree_a == 0 || degree_b == 0)
				return 0;

			std::uniform_int_distribution<int> dis_a(index_b_in_adj_a + 1, g.degree(vertex_a) - 1);
			std::uniform_int_distribution<int> dis_b(index_a_in_adj_b + 1, g.degree(vertex_b) - 1);
			long double res = 0;
			for (int iter = 0; iter < constants::ebfc_iterations; iter++) {
				int& neighbor_a = adj_a[(int)sampler::generate_random_int(index_b_in_adj_a + 1, g.degree(vertex_a) - 1)];
				int& neighbor_b = adj_b[(int)sampler::generate_random_int(index_a_in_adj_b + 1, g.degree(vertex_b) - 1)];
				if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
					res++;
				}
			}
			res /= constants::ebfc_iterations;
			res *= ((long double)degree_a) * (degree_b);
			return res;
		}
	}
	namespace path {
		bool z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b) {
			int degree_a = g.degree(vertex_a);
			int degree_b = g.degree(vertex_b);
			int& neighbor_a = g.get_adj_vec(vertex_a)[(int)sampler::generate_random_int(0, degree_a - 1)];
			int& neighbor_b = g.get_adj_vec(vertex_b)[(int)sampler::generate_random_int(0, degree_b - 1)];
			if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
				return true;
			}
			return false;
		}

		bool centered_z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b) {
			std::vector<int>& adj_a = g.get_adj_vec(vertex_a);
			std::vector<int>& adj_b = g.get_adj_vec(vertex_b);
			int index_b_in_adj_a = (int)(std::lower_bound(adj_a.begin(), adj_a.end(), vertex_b) - adj_a.begin());
			int index_a_in_adj_b = (int)(std::lower_bound(adj_b.begin(), adj_b.end(), vertex_a) - adj_b.begin());
			int degree_a = g.degree(vertex_a) - index_b_in_adj_a - 1;
			int degree_b = g.degree(vertex_b) - index_a_in_adj_b - 1;

			if (degree_a == 0 || degree_b == 0)
				return false;

			int neighbor_a = adj_a[(int)sampler::generate_random_int(index_b_in_adj_a + 1, g.degree(vertex_a) - 1)];
			int neighbor_b = adj_b[(int)sampler::generate_random_int(index_a_in_adj_b + 1, g.degree(vertex_b) - 1)];

			if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
				return true;
			}
			return false;
		}
	}
}


namespace utility {
	int intersect(Graph& G, const int& x, const int& y) {
		int ret = 0;
		int degree_x = G.degree(x);
		int degree_y = G.degree(y);
		if (degree_x < degree_y) {
			std::unordered_set<int> &y_adj_set = G.get_adj_set(y);
			for (auto& neighbor : G.get_adj_set(x)) {
				if (y_adj_set.find(neighbor) != y_adj_set.end())
					ret++;
			}
		}
		else {
			std::unordered_set<int> &x_adj_set = G.get_adj_set(x);
			for (auto& neighbor : G.get_adj_set(y)) {
				if (x_adj_set.find(neighbor) != x_adj_set.end())
					ret++;
			}
		}
		return ret;
	}
	double normalized(const double unnormalized, const double factor) {
		double ret = unnormalized * factor;
		return ret;
	}
}