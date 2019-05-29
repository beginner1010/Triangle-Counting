#include "counting.h"

namespace counting {
	namespace triangle {
		unsigned long long exact_edge_centeric_global_counting(Graph& G) {
			unsigned long long res = 0;
			for (auto& edge : G.get_edges()) {
				res += exact_edge_centeric_local_edge_counting(G, edge);
			}
			return res;
		}
		int exact_edge_centeric_local_edge_counting(Graph &G, const std::pair<int, int>& edge) {
			return utill::intersect(G, edge.first, edge.second);
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
	int intersect(Graph& G, const int& x, const int& y) {
		int ret = 0;
		if (G.degree(x) < G.degree(y)) {
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