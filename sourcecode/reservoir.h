#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "sampler.h"

class reservoir : sampler {
public:
	reservoir();
	~reservoir();
	std::unordered_set<int> EMPTY;
	void add_edge_to_reservoir(int A, int B);
	bool remove_edge_from_reservoir(int A, int B);
	std::pair<int, int> remove_random_edge_from_reservoir();
	void replace_in_reservoir(const int& A, const int& B);
	void replace_in_reservoir_with_fixed_size(const int& A, const int& B);
	void set_max_size(int);
	inline int& get_maximum_size() { return this->max_size_of_reservoir; }
	inline int& get_current_size() { return this->current_reservoir_size; }
	inline std::unordered_set<int>& get_adj(const int &vertex, bool exist = false) {
		if (exist || this->adj.find(vertex) != this->adj.end())
			return this->adj[vertex];
		return EMPTY;
	}
	std::vector <long long> edge_pool;
	std::pair <int, int> to_edge(const long long& hash);
	long long to_hash(const int& A, const int& B);
	void clear();
private:
	inline void remove_if_empty(const int &x) { if (adj[x].empty()) adj.erase(x); }
	std::unordered_map< int, std::unordered_set<int> > adj;
	std::unordered_map< long long, int > edge_to_index;
	int max_size_of_reservoir;
	int current_reservoir_size;
};
