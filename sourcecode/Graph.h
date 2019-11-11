#pragma once
#include "utills.h"
#include "IO.h"
#include "printing.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <sys/stat.h>

const long long SHIFT = 1000 * 1000 * 1000; 

class Graph {
public:
	Graph();
	~Graph();
	Graph& operator=(Graph& other) {
		this->edges = other.edges;
		this->vertices = other.vertices;
		this->vertices_left = other.vertices_left;
		this->vertices_right = other.vertices_right;
		this->adj_set = other.adj_set;
		this->adj_vec = other.adj_vec;
		this->cum_z = other.cum_z;
		this->cum_centered_z = other.cum_centered_z;
		this->m = other.m; 
		this->n = other.n;
		this->n_z = other.n_z; 
		this->n_w = other.n_w;
		this->n_centered_z = other.n_centered_z;
		this->n_centered_w = other.n_centered_w;
		this->n_left = other.n_left;
		this->n_right = other.n_right;
		this->maximum_degree = other.maximum_degree;
		return *this;
	}

	void clear();
	inline std::unordered_set<int>& get_adj_set(const int& vertex) {
		return this->adj_set[vertex];
	}
	inline std::vector<int>& get_adj_vec(const int& vertex) {
		return this->adj_vec[vertex];
	}
	inline std::vector <long double>& get_cum_z() {
		return this->cum_z; 
	}
	inline std::vector <long double>& get_cum_w() {
		return this->cum_w;
	}
	inline std::vector <long double>& get_cum_centered_z() {
		return this->cum_centered_z;
	}
	inline int get_n_edges() {
		return this->m;
	}
	inline int get_n_vertices() {
		return this->n;
	}
	inline long double get_n_z() {
		if (this->n_z == -1)
			this->compute_n_z();
		return this->n_z;
	}
	inline long double get_n_centered_z() {
		if (this->n_centered_z == -1)
			this->compute_n_centred_z();
		return this->n_centered_z;
	}
	inline long double get_n_w() {
		if (this->n_w == -1)
			this->compute_n_w();
		return this->n_w;
	}
	inline int get_n_left_vertices() {
		return this->n_left;
	}
	inline int get_n_right_vertices() {
		return this->n_right;
	}
	inline std::vector< std::pair<int, int> >& get_edges() {
		return this->edges;
	}
	inline std::vector< int >& get_vertices() {
		return this->vertices;
	}
	inline std::vector<int>& get_left_vertices() {
		return this->vertices_left;
	}
	inline std::vector<int>& get_right_vertices() {
		return this->vertices_right;
	}
	inline std::pair<int,int>& get_ith_edge(const int& idx) {
		return this->edges[idx];
	}
	inline int degree(const int& vertex) {
		return (int)this->adj_vec[vertex].size();
	}
	inline int degree_const(const int& vertex) {
		return (int)this->adj_vec[vertex].size();
	}
	inline int get_maximum_degree() {
		return this->maximum_degree;
	}

	std::vector<int> get_wedge(long double random_weight, int center, const std::vector<int>& neighbors_of_center);
	int get_vertex_index(long long vertex);
	void add_new_edge(const int& vertex_left, const int& vertex_right, const bool use_vec = true);
	void add_new_edge(const std::pair<int, int> &edge, const bool use_vec = true);

	void sort_vertices_by_degree();
	void sort_vertices_by_wedges();
	void sort_vertices_by_degeneracy();

	void read_from_file();
	void get_original_order();

private:
	std::unordered_map <long long, int> vertex_index;
	std::vector < std::pair<int, int> > edges;
	std::vector < std::pair<int, int> > original_edges_order;
	std::vector < int > vertices;
	std::vector < std::pair< long double, int > > degree_vertex_vec;
	std::vector < std::pair< long double, int > > wedge_vertex_vec;
	std::vector < long double > cum_z;
	std::vector < long double > cum_w;
	std::vector < long double > cum_centered_z;
	std::vector < int > vertices_left;
	std::vector < int > vertices_right;

	std::vector < std::unordered_set<int> > adj_set;
	std::vector < std::vector<int> > adj_vec;

	int m;
	int n;
	int n_left;
	int n_right;
	int maximum_degree;
	const int text_interval = 100000;
	const int max_fancy_text_width = 50;
	int n_dots;
	int line_number;
	long double n_z;
	long double n_w;
	long double n_centered_z;
	long double n_centered_w;

	bool is_sorted_vectors;
	bool is_original;

	void compute_n_w();
	void compute_n_z();
	void compute_n_centred_z();

	void reindex(std::vector < std::pair< long double, int > >& weights);
	void reindex(std::vector < int > &ordered);
	void remove_all_edges();

	void update_adj(const int &vertex_left, const int& vertex_right, const int mode, const bool use_vec = true);
	void update_adj(const std::pair<int, int>& edge, const int mode, const bool use_vec = true);

	inline long long encode_edge(const int& vertex_left, const int& vertex_right) {
		return vertex_left * SHIFT + vertex_right;
	}
	inline std::pair<int, int> decode_edge(long long& hash_value) {
		return std::make_pair(hash_value / SHIFT, hash_value % SHIFT);
	}
};