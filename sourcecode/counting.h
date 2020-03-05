#pragma once
#include "Graph.h"
#include "counting.h"
#include "reservoir.h"

namespace counting {
	namespace triangle {
		unsigned long long exact_edge_centeric_global_counting(Graph &G);
		int exact_edge_centeric_local_edge_counting(Graph &G, const int& A, const int& B);
		int exact_edge_centeric_local_edge_counting(reservoir &R, const int& A, const int& B);
		unsigned long long exact_edge_centeric_local_vertex_counting(Graph &G, const int& vertex);
		double edge_centeric_local_edge_counting_by_martingle(reservoir &R, const int& A, const int& B, std::unordered_map <long long, double>& lookup_weight, const double& z_star);
	}
}

namespace utill {
	int intersect(Graph& G, const int &x, const int &y);
	int ordered_intersect(Graph& G, const int& x, const int& y);
	int intersect(reservoir& R, const int& x, const int& y);
	double normalized(const double unnormalized, const double factor);
}