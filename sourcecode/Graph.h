#pragma once
#include "utills.h"
#include "IO.h"
#include "fancy_printing.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <sys/stat.h>

#define mmin(x,y) ((x)>(y)?(y):(x))
#define mmax(x,y) ((x)<(y)?(y):(x))
const long long SHIFT = 1000 * 1000 * 1000; 

class base_graph {
protected:
	int m;
	int n;
	int maximum_degree;
public:
	inline int get_n_edges() {
		return this->m;
	}
	inline int get_n_vertices() {
		return this->n;
	}
};

class Graph : public base_graph {
public:
	Graph();
	~Graph();
	Graph& operator=(Graph& other) {
		this->edges = other.edges;
		this->vertices = other.vertices;
		this->adj_set = other.adj_set;
		this->adj_vec = other.adj_vec;
		this->m = other.m; 
		this->n = other.n;
		this->n_z = other.n_z; 
		this->n_w = other.n_w;
		this->n_centered_z = other.n_centered_z;
		this->n_centered_w = other.n_centered_w;
		this->maximum_degree = other.maximum_degree;
		return *this;
	}

	void clear();
	void read_from_file();
	/*
		Compressing graph process cannot keep ordering of vertices. 
	*/
	void compress_graph();

	std::vector<long long>& get_wedges() {
		if (this->cum_wedges.empty())
			this->process_wedges();
		return this->cum_wedges;
	}

	inline std::unordered_set<int>& get_adj_set(const int& vertex) {
		assert(vertex < this->n);
		return this->adj_set[vertex];
	}

	inline std::vector<int>& get_adj_vec(const int& vertex) {
		assert(vertex < this->n);
		return this->adj_vec[vertex];
	}

	inline std::pair<int, int> get_compressed_adj(const int& vertex) {
		return std::make_pair(this->offset[vertex], this->offset[vertex + 1]);
	}

	inline std::vector< std::pair<int, int> >& get_edges() {
		return this->edges;
	}

	inline std::vector< int >& get_vertices() {
		return this->vertices;
	}

	inline std::pair<int,int>& get_ith_edge(const int& idx) {
		return this->edges[idx];
	}

	inline int get_compressed_degree(const int& vertex) {
		return this->offset[vertex + 1] - this->offset[vertex];
	}

	inline int get_degree(const int& vertex) {
		return (int)this->adj_set[vertex].size();
	}

	inline bool is_compressed_edge(const int& src, const int& dist) {
		return std::binary_search(this->compressed_adj.begin() + this->offset[src], this->compressed_adj.begin() + this->offset[src + 1], dist);
	}

	inline int& get_compressed_ith_neighbor(const int& idx) {
		return this->compressed_adj[idx];
	}

	int get_vertex_index(long long vertex);
	std::vector<int> get_wedge(long long random_weight, int center);

	void add_new_edge(const int& vertex_left, const int& vertex_right, const bool use_vec = true);
	void add_new_edge(const std::pair<int, int> &edge, const bool use_vec = true);
	void replace_edge(const std::pair<int, int> &new_edge, const int& remove_index, const bool use_vec = true);

	void sort_vertices_by_degree();
	void sort_vertices_by_wedges();
	void sort_vertices_by_degeneracy();

private:
	std::vector < std::pair<int, int> > edges;
	std::vector < int > vertices;
	std::vector < long long > cum_wedges;
	std::vector < std::pair< long long, int > > degree_vertex_vec;
	std::vector < std::pair< long long, int > > wedge_vertex_vec;
	std::vector < std::unordered_set<int> > adj_set;
	std::vector < std::vector<int> > adj_vec;
	std::unordered_map <long long, int> vertex_index;
	std::vector <int> offset;
	std::vector <int> compressed_adj;

	const int text_interval = 100000;
	const int max_fancy_text_width = 60;
	int n_dots;
	int line_number;
	long long n_z; 
	long long n_w;
	long long n_centered_z;
	long long n_centered_w;

	void process_wedges();

	void reindex(std::vector < std::pair< long long, int > >& weights);
	void reindex(std::vector < int > &ordered);
	void resize();

	void preprocessing();

	void update_adj(const int &vertex_left, const int& vertex_right, const int mode, const bool use_vec = true);
	void update_adj(const std::pair<int, int>& edge, const int mode, const bool use_vec = true);


	inline long long encode_edge(int& vertex_left, int& vertex_right) {
		return vertex_left * SHIFT + vertex_right;
	}
	inline std::pair<int, int> decode_edge(long long& hash_value) {
		return std::make_pair(hash_value / SHIFT, hash_value % SHIFT);
	}
};