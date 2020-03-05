#include "counting.h"

namespace counting {
	namespace triangle {
		unsigned long long exact_edge_centeric_global_counting(Graph& G) {
			unsigned long long res = 0;
			auto& edges = G.get_edges();
			for (auto& edge : edges) {
				res += utill::ordered_intersect(G, edge.first, edge.second);
			}
			return res; // TODO: can we do it better to avoid duplicate counting? 
		}
		int exact_edge_centeric_local_edge_counting(Graph &G, const int& A, const int& B) {
			return utill::intersect(G, A, B);
		}
		int exact_edge_centeric_local_edge_counting(reservoir &R, const int& A, const int& B) {
			return utill::intersect(R, A, B);
		}
		double edge_centeric_local_edge_counting_by_martingle(reservoir &R, const int& A, const int& B, std::unordered_map <long long, double>& lookup_weight, const double& z_star) {
			double ret = 0;
			auto& adj_a = R.get_adj(A);
			auto& adj_b = R.get_adj(B);
			if (adj_a.size() < adj_b.size()) {
				for (auto& neighbor : adj_a) {
					if (adj_b.find(neighbor) != adj_b.end()) {
						double q1 = mmin(1.0, z_star <= 1e-10 ? 1.0 : lookup_weight[R.to_hash(A, neighbor)] / z_star);
						double q2 = mmin(1.0, z_star <= 1e-10 ? 1.0 : lookup_weight[R.to_hash(B, neighbor)] / z_star);
						ret += 1.0 / (q1 * q2);
					}
				}
			}
			else {
				for (auto& neighbor : adj_b) {
					if (adj_a.find(neighbor) != adj_a.end()) {
						double q1 = mmin(1.0, z_star <= 1e-10 ? 1.0 : lookup_weight[R.to_hash(A, neighbor)] / z_star);
						double q2 = mmin(1.0, z_star <= 1e-10 ? 1.0 : lookup_weight[R.to_hash(B, neighbor)] / z_star);
						ret += 1.0 / (q1 * q2);
					}
				}
			}
			return ret;
		}
		unsigned long long exact_edge_centeric_local_vertex_counting(Graph &G, const int& vertex) {
			int ret = 0;
			for (auto& neighbor : G.get_adj_set(vertex)) {
				ret += utill::intersect(G, vertex, neighbor);
			}
			ret /= 2; // TODO: can we do it better to avoid duplicate counting? 
			return ret;
		}
	}
}

namespace utill {
	int ordered_intersect(Graph& G, const int& x, const int& y) {
		int ret = 0;
		int degree_x = settings::compressed ? G.get_compressed_degree(x) : G.get_degree(x);
		int degree_y = settings::compressed ? G.get_compressed_degree(y) : G.get_degree(y);
		int mx = mmax(x, y);
		if (degree_x < degree_y) {
			if (settings::compressed) {
				auto pair = G.get_compressed_adj(x);
				for (int i = pair.first; i < pair.second; i++) {
					int& neighbor = G.get_compressed_ith_neighbor(i);
					if (neighbor > mx && G.is_compressed_edge(y, neighbor) == true)
						ret++;
				}
			}
			else {
				std::unordered_set<int>& y_adj_set = G.get_adj_set(y);
				for (auto& neighbor : G.get_adj_set(x)) {
					if (neighbor > mx && y_adj_set.find(neighbor) != y_adj_set.end())
						ret++;
				}
			}
		}
		else {
			if (settings::compressed) {
				auto pair = G.get_compressed_adj(y);
				for (int i = pair.first; i < pair.second; i++) {
					int& neighbor = G.get_compressed_ith_neighbor(i);
					if (neighbor > mx && G.is_compressed_edge(x, neighbor) == true)
						ret++;
				}
			}
			else {
				std::unordered_set<int>& x_adj_set = G.get_adj_set(x);
				for (auto& neighbor : G.get_adj_set(y)) {
					if (neighbor > mx && x_adj_set.find(neighbor) != x_adj_set.end())
						ret++;
				}
			}
		}
		return ret;
	}
	int intersect(reservoir& R, const int& x, const int& y) {
		int ret = 0;
		auto& adj_x = R.get_adj(x);
		auto& adj_y = R.get_adj(y);
		if (adj_x.size() < adj_y.size()) {
			for (auto& neighbor : adj_x) {
				if (adj_y.find(neighbor) != adj_y.end())
					ret++;
			}
		}
		else {
			for (auto& neighbor : adj_y) {
				if (adj_x.find(neighbor) != adj_x.end())
					ret++;
			}
		}
		return ret;
	}
	int intersect(Graph& G, const int& x, const int& y) {
		int ret = 0;
		int degree_x = settings::compressed ? G.get_compressed_degree(x) : G.get_degree(x);
		int degree_y = settings::compressed ? G.get_compressed_degree(y) : G.get_degree(y);
		if (degree_x < degree_y) {
			if (settings::compressed) {
				auto pair = G.get_compressed_adj(x);
				for (int i = pair.first; i < pair.second; i++) {
					int& neighbor = G.get_compressed_ith_neighbor(i);
					if (G.is_compressed_edge(y, neighbor) == true)
						ret++;
				}
			}
			else {
				std::unordered_set<int>& y_adj_set = G.get_adj_set(y);
				for (auto& neighbor : G.get_adj_set(x)) {
					if (y_adj_set.find(neighbor) != y_adj_set.end())
						ret++;
				}
			}
		}
		else {
			if (settings::compressed) {
				auto pair = G.get_compressed_adj(y);
				for (int i = pair.first; i < pair.second; i++) {
					int& neighbor = G.get_compressed_ith_neighbor(i);
					if (G.is_compressed_edge(x, neighbor) == true)
						ret++;
				}
			}
			else {
				std::unordered_set<int>& x_adj_set = G.get_adj_set(x);
				for (auto& neighbor : G.get_adj_set(y)) {
					if (x_adj_set.find(neighbor) != x_adj_set.end())
						ret++;
				}
			}
		}
		return ret;
	}
	double normalized(const double unnormalized, const double factor) {
		double ret = unnormalized * factor;
		return ret;
	}
}