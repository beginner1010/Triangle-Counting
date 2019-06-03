#include "Graph.h"

Graph::Graph() {
	this->clear();
}
Graph::~Graph() {}

void Graph::read_from_file() {
	this->preprocessing();
}

void Graph::compress_graph(bool log) {
	if (this->offset.empty() == true) {
		this->offset.resize(this->n + 1);
		this->compressed_adj.clear();
		int cur_offset = 0;
		double cur_time = 0;
		double last_print_time = -1000;
		int n_dots = 0;
		for (int src = 0; src < this->n; src++) {
			clock_t start_time = clock();
			offset[src] = cur_offset;
			for (auto& dist : this->get_adj_set(src)) {
				compressed_adj.push_back(dist);
			}
			cur_offset += this->get_degree(src);
			std::sort(compressed_adj.begin() + offset[src], compressed_adj.end());
			cur_time += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			if (log == true)
				print::message_with_dots(last_print_time, n_dots, cur_time, this->n, src + 1, "Compressing graph");
		}
		if (log == true) {
			print::clear_line();
			print::finished_work(cur_time, "The input graph is compressed");
		}
		assert((int)this->compressed_adj.size() == 2 * this->m);
		this->offset[this->n] = 2 * this->m;
	} else {
		if (log == true) 
			std::cerr << " The input graph is already compressed." << std::endl;
	}
}

int Graph::get_vertex_index(long long vertex) {
	if (this->vertex_index.find(vertex) == this->vertex_index.end()) {
		// vertex hasn't been seen already. Welcome home!
		this->vertex_index[vertex] = (int)this->vertices.size();
		this->vertices.push_back(this->vertex_index[vertex]);
		this->adj_set.push_back(std::unordered_set<int>());
		this->adj_vec.push_back(std::vector<int>());
		this->n++;
	}
	return this->vertex_index[vertex];
}

void Graph::update_adj(const int &vertex_left, const int& vertex_right, const int mode, const bool use_vec) {
	if (mode == -1) {
		this->m--;
		this->adj_set[vertex_left].erase(vertex_right);
		this->adj_set[vertex_right].erase(vertex_left);
	}
	else { // mode is +1
		this->m++;
		this->adj_set[vertex_left].insert(vertex_right);
		this->adj_set[vertex_right].insert(vertex_left);
		if (use_vec == true) {
			/*
				In some algorithms, we dont need adj_vec. For example in triest-base.
			*/
			this->adj_vec[vertex_left].push_back(vertex_right);
			this->adj_vec[vertex_right].push_back(vertex_left);
			/*
				Since no order is specified, we set this boolean variable False.
			*/
			this->is_sorted_vectors = false;
		}
	}
}

void Graph::update_adj(const std::pair<int, int>& edge, const int mode, const bool use_vec) {
	this->update_adj(edge.first, edge.second, mode, use_vec);
}

void Graph::add_new_edge(const int& vertex_left, const int& vertex_right, const bool use_vec) {
	this->edges.push_back(std::make_pair(vertex_left, vertex_right));
	this->update_adj(vertex_left, vertex_right, +1, use_vec);
}

void Graph::add_new_edge(const std::pair<int, int> &edge, const bool use_vec) {
	this->add_new_edge(edge.first, edge.second, use_vec);
}

void Graph::replace_edge(const std::pair<int, int> &new_edge, const int& remove_index, const bool use_vec) {
	this->update_adj(this->edges[remove_index], -1, use_vec); // remove the old edge
	this->edges[remove_index] = new_edge; // replace with new edge
	this->update_adj(this->edges[remove_index], +1, use_vec); // update adj with new edge
}

void Graph::preprocessing() {
	this->clear();

	std::ios::sync_with_stdio(false);
	
	std::unordered_set < long long > seen_edges;
	double last_print_time = -1000;
	double cur_time = 0;
	int n_dots = 0;
	double total_work = (double)IO::get_file_size();
	double done_work = 0;
	char char_line[1024];

	while (std::fgets(char_line, 1024, IO::fin)) {
		std::string line = std::string(char_line);
		done_work += (double)line.size();

		print::message_with_dots(last_print_time, n_dots, cur_time, total_work, done_work, "Reading graph");
		line_number++;

		clock_t start_time = clock();
		std::stringstream ss; ss << line;
		std::vector <std::string> vec_str;
		for (std::string z; ss >> z; vec_str.push_back(z));
		if (((int)vec_str.size()) >= 2) {
			bool is_all_num = true;
			for (int i = 0; i < 2 ; i++) is_all_num &= helper_functions::is_int_num(vec_str[i]);
			if (is_all_num) {

				int vertex_left = helper_functions::to_int(vec_str[0]);
				int vertex_right = helper_functions::to_int(vec_str[1]);

				if (vertex_left == vertex_right)
					continue;

				vertex_left = this->get_vertex_index(vertex_left);
				vertex_right = this->get_vertex_index(vertex_right);

				if (seen_edges.find(this->encode_edge(vertex_left, vertex_right)) != seen_edges.end()
					|| seen_edges.find(this->encode_edge(vertex_right, vertex_left)) != seen_edges.end())
					continue;

				seen_edges.insert(this->encode_edge(vertex_left, vertex_right));
				this->add_new_edge(vertex_left, vertex_right, true);
				this->maximum_degree = mmax(this->maximum_degree, 
					mmax((int)this->adj_vec[vertex_left].size(), (int)this->adj_vec[vertex_right].size()));
			}
		}
		clock_t end_time = clock();
		cur_time += ((double)end_time - start_time) / CLOCKS_PER_SEC;
	}
	print::clear_line();
	print::finished_work(cur_time, "The input graph is read");
}

void Graph::process_wedges() {
	this->n_w = 0;
	this->cum_wedges.clear();
	for (auto& vertex : this->vertices) {
		int deg = this->get_degree(vertex);
		long long local_wedge = helper_functions::choose2(deg);
		this->n_w += local_wedge;
		this->cum_wedges.push_back(n_w);
	}
}

void Graph::process_ordered_wedges() {
	this->n_w = 0;
	this->cum_wedges.clear();
	for (auto& vertex : this->vertices) {
		int deg = this->get_ordered_degree(vertex);
		long long local_wedge = helper_functions::choose2(deg);
		this->n_w += local_wedge;
		this->cum_wedges.push_back(n_w);
	}
}

std::vector<int> Graph::get_wedge(long long random_weight, int center, std::vector<int>& neighbors_of_center) {
	random_weight -= center == 0 ? 0 : this->cum_wedges[center - 1];
	int lo = 0, hi = (int)neighbors_of_center.size() - 1;
	long long deg = (int)neighbors_of_center.size();
	/*
		TODO: Can we avoid binary search here by using Algorithm 1 of https://public.ca.sandia.gov/~tgkolda/pubs/bibtgkfiles/Triangles-arXiv-1202.5230v1.pdf
	*/
	while (lo < hi) {
		int mid = (lo + hi) >> 1;
		if ((mid + 1) * deg - helper_functions::choose2(mid + 1 + 1) < random_weight) {
			lo = mid + 1;
		}
		else {
			hi = mid;
		}
	}

	int wedge_head1 = neighbors_of_center[lo];
	random_weight -= lo == 0 ? 0 : lo * deg - helper_functions::choose2(lo + 1);
	int wedge_head2 = neighbors_of_center[lo + random_weight];

	std::vector<int> wedge = { wedge_head1, center, wedge_head2 };
	return wedge;
}

void Graph::reindex(std::vector < std::pair< long long, int > > &weights) {
	std::vector <int> aux_rank = std::vector <int>(this->n);
	for (int i = 0; i < this->n; i++)
		aux_rank[weights[i].second] = i;

	std::vector < std::pair<int, int> > aux_edges = this->edges;
	this->resize();
	// we do not update left_vertices, right_vertices (we don't use them. Should we update them?)
	for (auto& aux_edge : aux_edges) {
		int& a = aux_rank[aux_edge.first];
		int& b = aux_rank[aux_edge.second];
		this->add_new_edge(a, b, true);
	}
	for (auto& v : this->vertices) {
		std::vector<int>& adj = this->get_adj_vec(v);
		std::sort(adj.begin(), adj.end());
		this->maximum_degree = mmax(this->maximum_degree, this->get_degree(v));
	}
}

void Graph::reindex(std::vector < int > &ordered) {
	std::vector <int> vertex_rank = std::vector <int>(this->n);
	for (int i = 0; i < this->n; i++)
		vertex_rank[ordered[i]] = i;

	this->resize_ordered_adj();
	for (auto& edge : this->edges) {
		int& a = edge.first;
		int& b = edge.second;
		if (vertex_rank[a] > vertex_rank[b]) {
			this->ordered_adj_vec[b].push_back(a);
			this->ordered_adj_set[b].insert(a);
		}
		else {
			this->ordered_adj_vec[a].push_back(b);
			this->ordered_adj_set[a].insert(b);
		}
	}
}

void Graph::resize_ordered_adj() {
	this->ordered_adj_vec.clear();
	this->ordered_adj_vec.resize(this->n);
	this->ordered_adj_set.clear();
	this->ordered_adj_set.resize(this->n);
}

void Graph::resize() {
	this->adj_vec.clear();
	this->adj_vec.resize(this->n);
	this->adj_set.clear();
	this->adj_set.resize(this->n);
	this->edges.clear();
	this->m = 0;
}

void Graph::sort_adj_vectors_by_ids() {
	if (this->is_sorted_vectors == true)
		return;
	this->is_sorted_vectors = true;
	for (auto& vertex : this->vertices) {
		std::sort(this->adj_vec[vertex].begin(), this->adj_vec[vertex].end());
	}
}

void Graph::sort_vertices_by_degree() {
	this->degree_vertex_vec.clear();
	for (auto& vertex : this->vertices) {
		this->degree_vertex_vec.push_back(std::make_pair(this->get_degree(vertex), vertex));
	}
	std::sort(this->degree_vertex_vec.begin(), this->degree_vertex_vec.end());
	this->reindex(degree_vertex_vec);
}

void Graph::sort_vertices_by_wedges() {
	this->wedge_vertex_vec.clear();
	for (auto& vertex : this->vertices) {
		long long wedges = 0;
		for (auto neighbor : this->adj_vec[vertex]) {
			wedges += this->get_degree(neighbor) ; // indeed, we compare w(a) + d(a)
		}
		this->wedge_vertex_vec.push_back(std::make_pair(wedges, vertex));
	}
	std::sort(this->wedge_vertex_vec.begin(), this->wedge_vertex_vec.end());
	this->reindex(wedge_vertex_vec);
}

void Graph::sort_vertices_by_degeneracy() {
	std::vector< std::vector <int> > degree_bucket(this->maximum_degree + 1, std::vector<int>());
	std::vector< int > vertex_degree(this->n);
	for (auto& vertex : this->vertices) {
		degree_bucket[this->get_degree(vertex)].push_back(vertex);
		vertex_degree[vertex] = this->get_degree(vertex);
	}

	std::vector<bool> visited(this->n, false);
	std::vector<int> ordered;
	int bucket_index = 0;
	while (bucket_index <= this->maximum_degree) {
		if (degree_bucket[bucket_index].empty()) {
			bucket_index++;
			continue;
		}

		int vertex = degree_bucket[bucket_index].back();
		degree_bucket[bucket_index].pop_back();

		if (visited[vertex] == false) {
			visited[vertex] = true;
			ordered.push_back(vertex);
			for (auto& neighbor : this->get_adj_vec(vertex)) {
				if (visited[neighbor] == false) {
					vertex_degree[neighbor] --;
					degree_bucket[vertex_degree[neighbor]].push_back(neighbor);
					bucket_index = mmin(bucket_index, vertex_degree[neighbor]);
				}
			}
		}
	}
	assert((int)ordered.size() == this->n);
	this->reindex(ordered);
}

void Graph::clear() {
	this->edges.clear();
	this->vertices.clear();
	this->adj_set.clear();
	this->adj_vec.clear();
	this->vertex_index.clear();
	this->m = 0;
	this->n = 0;
	this->n_z = 0;
	this->n_w = 0;
	this->maximum_degree = 0;
	this->line_number = 0;
	this->is_sorted_vectors = false;
}
