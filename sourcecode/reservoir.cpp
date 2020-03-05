#include "reservoir.h"

const long long shift = 1000 * 1000 * 1000;

reservoir::reservoir() {
	this->clear();
}

reservoir::~reservoir() {}

long long reservoir::to_hash(const int& A, const int& B) {
	if (A > B) return B * shift + A;
	else return A * shift + B;
}

std::pair <int, int> reservoir::to_edge(const long long& hash) {
	return std::make_pair(hash / shift, hash % shift);
}

void reservoir::add_edge_to_reservoir(int A, int B) {
	if (A > B) std::swap(A, B);
	long long edge_hash = this->to_hash(A, B);
	this->adj[A].insert(B);
	this->adj[B].insert(A);
	this->edge_to_index[edge_hash] = (int)this->edge_pool.size();
	this->edge_pool.push_back(edge_hash);
	this->current_reservoir_size = (int)this->edge_pool.size();
}

bool reservoir::remove_edge_from_reservoir(int A, int B) {
	if (A > B) std::swap(A, B);
	long long edge_hash = this->to_hash(A, B);
	if (edge_to_index.find(edge_hash) != edge_to_index.end()) {

		this->adj[A].erase(B);
		this->remove_if_empty(A);

		this->adj[B].erase(A);
		this->remove_if_empty(B);

		int idx = edge_to_index[edge_hash];
		edge_to_index.erase(edge_hash);

		if (idx != (int)this->edge_pool.size() - 1) {
			this->edge_pool[idx] = this->edge_pool.back();
			edge_to_index[this->edge_pool.back()] = idx;
		}
		this->edge_pool.pop_back();

		this->current_reservoir_size = (int)this->edge_pool.size();
		return true;
	}
	return false;
}

std::pair<int,int> reservoir::remove_random_edge_from_reservoir() {
	int ran_idx = (int)this->generate_random_int(0, edge_pool.size() - 1);
	auto rem_edge = to_edge(this->edge_pool[ran_idx]);
	this->remove_edge_from_reservoir(rem_edge.first, rem_edge.second);
	return rem_edge;
}

void reservoir::replace_in_reservoir(const int& A, const int& B) {
	this->remove_random_edge_from_reservoir();
	this->add_edge_to_reservoir(A, B);
}

void reservoir::replace_in_reservoir_with_fixed_size(const int& A, const int& B) {
	int ran_idx = (int)this->generate_random_int(0, this->max_size_of_reservoir - 1);
	if (ran_idx < this->current_reservoir_size) {
		auto rem_edge = to_edge(this->edge_pool[ran_idx]);
		this->remove_edge_from_reservoir(rem_edge.first, rem_edge.second);
		this->add_edge_to_reservoir(A, B);
	}
	else {
		this->add_edge_to_reservoir(A, B);
	}
}

void reservoir::set_max_size(int sz) {
	this->max_size_of_reservoir = sz;
	return;
}

void reservoir::clear() {
	this->adj.clear();
	this->edge_pool.clear();
	this->EMPTY.clear();
	this->edge_to_index.clear();
	this->current_reservoir_size = 0;
}
