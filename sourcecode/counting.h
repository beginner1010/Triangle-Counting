#pragma once
#include <random>

#include "Graph.h"
#include "sampler.h"

namespace exact {
	namespace global {
		long long kai_exact_count(Graph& g);
	}
	namespace wedge {
		int bfly_in_wedge(Graph& g, std::vector<int>& wedge);
	}
}

namespace randomized {
	namespace edge {
		long double randomized_ebfc(Graph& g, int& vertex_left, int& vertex_right);
		long double randomized_centered_ebfc(Graph& g, int& vertex_a, int& vertex_b);
	}
	namespace path {
		bool z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b);
		bool centered_z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b);
	}
	
}

namespace utility {
	int intersect(Graph& G, const int &x, const int &y);
	double normalized(const double unnormalized, const double factor);
}