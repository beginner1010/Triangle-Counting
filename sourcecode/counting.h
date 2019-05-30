#pragma once
#include "Graph.h"

namespace counting {
	namespace triangle {
		unsigned long long exact_edge_centeric_global_counting(Graph &G, bool compress = false);
		int exact_edge_centeric_local_edge_counting(Graph &G, const std::pair<int, int>& edge, bool compress = false);
		unsigned long long exact_edge_centeric_local_vertex_counting(Graph &G, const int& vertex, bool compress = false);
	}
}

namespace utill {
	int intersect(Graph& G, const int &x, const int &y, bool compress);
	double normalized(const double unnormalized, const double factor);
}