#include "counting.h"

namespace counting {
	namespace triangle {
		unsigned long long exact_edge_centeric_global_counting(Graph& G, bool compress) {
			unsigned long long res = 0;
			for (auto& edge : G.get_edges()) {
				res += exact_edge_centeric_local_edge_counting(G, edge, compress);
			}
			return res;
		}
		int exact_edge_centeric_local_edge_counting(Graph &G, const std::pair<int, int>& edge, bool compress) {
			return utill::intersect(G, edge.first, edge.second, compress);
		}

		unsigned long long exact_edge_centeric_local_vertex_counting(Graph &G, const int& vertex, bool compress) {
			int ret = 0;
			for (auto& neighbor : G.get_adj_set(vertex)) {
				ret += utill::intersect(G, vertex, neighbor, compress);
			}
			ret /= 2; // TODO: can we do it better to avoid duplicate counting? 
			return ret;
		}
	}
}

namespace utill {
	int intersect(Graph& G, const int& x, const int& y, bool compress) {
		int ret = 0;
		int degree_x = compress ? G.get_compressed_degree(x) : G.get_degree(x);
		int degree_y = compress ? G.get_compressed_degree(y) : G.get_degree(y);
		if (degree_x < degree_y) {
			if (compress) {
				auto pair = G.get_compressed_adj(x);
				for (int i = pair.first; i < pair.second; i++) {
					int& neighbor = G.get_compressed_ith_neighbor(i);
					if (G.is_compressed_edge(y, neighbor) == true)
						ret++;
				}
			}
			else {
				std::unordered_set<int> &y_adj_set = G.get_adj_set(y);
				for (auto& neighbor : G.get_adj_set(x)) {
					if (y_adj_set.find(neighbor) != y_adj_set.end())
						ret++;
				}
			}
		}
		else {
			if (compress) {
				auto pair = G.get_compressed_adj(y);
				for (int i = pair.first; i < pair.second; i++) {
					int& neighbor = G.get_compressed_ith_neighbor(i);
					if (G.is_compressed_edge(x, neighbor) == true)
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
		}
		return ret;
	}
	double normalized(const double unnormalized, const double factor) {
		double ret = unnormalized * factor;
		return ret;
	}
}