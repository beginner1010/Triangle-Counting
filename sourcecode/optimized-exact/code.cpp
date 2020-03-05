#include <bits/stdc++.h>
//#include "tbb/atomic.h"
//#include "tbb/parallel_for_each.h"
//#include "tbb/task_scheduler_init.h"
//#include "tbb/tick_count.h"

using namespace std;

#define ll long long

const ll SHIFT = (ll)1e9;

int n_vertices;
int* adj;
vector< unordered_set<int> > adj_set;
bool* edge_state;

int* deg;
ll* offset;
ll SNAP;
ll triangles;
int local_triangle;
ll m;
ll stream_size;
ll time_step;
bool* edge_cnt;
double start_time;

unordered_map<ll, int> hash_map;
unordered_set<ll> hash_set;
vector< pair<int,int> > ins_only_edges;
vector< pair<int,int> > unique_edges;
vector< pair<int,int> > fully_dyn_edges;

bool is_dynamic;


#ifdef HOME
	string graph_names[1] = {"livejournal"};
#else
	string graph_names[1] = {"uk2005"};
#endif

inline ll to_hash(const int& a, const int& b) {
       if(a > b) return b * SHIFT + a;
       else return a * SHIFT + b;
}

void add_vertex(ll& a) {
	if(hash_map.find(a) == hash_map.end()) {
	hash_map[a] = n_vertices ++;
	}
	a = hash_map[a];
}

void add_edge(ll& a, ll& b) {
	if(a == b)
		return;
	add_vertex(a);
	add_vertex(b);
	ll hsh = to_hash(a, b);
	if(hash_set.find(hsh) == hash_set.end()) {
		hash_set.insert(hsh);
		ins_only_edges.push_back({a, b});
		unique_edges.push_back({a, b});
		unique_edges.push_back({b, a});
	}
	else if(is_dynamic == false) {
		cout << "The graph is dynamic" << endl;
		is_dynamic = true;
	}
	fully_dyn_edges.push_back({a, b}); // include it even if is been seen before
}

inline bool is_int_num(const string& str) {
	if ((int)str.size() > 18)
		return false;
	for (int i = 0; i < (int)str.size(); i++) {
		if ('0' > str[i] || str[i] > '9')
			return false;
	}
	return true;
}

ll to_int(const string& str) {
	std::stringstream ss; ss << str;
	ll x; ss >> x;
	return x;
}

bool should_print(ll& time_step, ll& stream_size) {
	return time_step == stream_size || (time_step > 0 && time_step % SNAP == 0);
}

bool state;
void update_edge(int a, int b) {
	ll edge_idx =  lower_bound(adj + offset[a], adj + offset[a] + deg[a], b) - adj;
	if(edge_state[edge_idx] == false) { // add it
		state = true;
		edge_state[edge_idx] = true;
		adj_set[a].insert(b);
	} else { // remove the edge
		state = false;
		edge_state[edge_idx] = false;
		adj_set[a].erase(b);
	}
}

int main() {
    cin.tie(0), cout.tie(0);
	 ios_base::sync_with_stdio(false);
    
    for(string& graph_name: graph_names) {
		string input_address;

		#ifdef HOME
			input_address = ("in." + graph_name);
		#else
			input_address = ("/work/snt-free/triangle/input/in." + graph_name);
		#endif

		FILE* fin = fopen(input_address.c_str(), "r");
		hash_map.clear();
		hash_set.clear();
		ins_only_edges.clear();   
		unique_edges.clear();
		fully_dyn_edges.clear();
		n_vertices = 0;
		is_dynamic = false;

		char ch_line[1024];
		while(fgets(ch_line, 1024, fin)) {
			string line = string(ch_line);
			stringstream ss; ss << line;
			vector <string> vec_str;
			for (string z; ss >> z; vec_str.push_back(z));
			if (((int)vec_str.size()) <= 1) continue;
			bool is_all_num = true;
			for (int i = 0; i < 2 ; i++) is_all_num &= is_int_num(vec_str[i]);
			if (is_all_num == false) continue;
			ll a = to_int(vec_str[0]), b = to_int(vec_str[1]);
			add_edge(a, b);
		}

		sort(unique_edges.begin(), unique_edges.end());
		m = (ll)unique_edges.size();
		adj = (int *)malloc(m * sizeof(int));
		edge_state = (bool *)calloc(m, sizeof(bool));
		deg = (int *)calloc(n_vertices, sizeof(int));
		offset = (ll *)malloc(m * sizeof(ll));
		ll idx_edge = 0;

		for(ll i = 0; i < m; i ++) {
			adj[i] = unique_edges[i].second;
			deg[unique_edges[i].first] ++;
		}

		offset[0] = 0;
		for(int i = 1; i < n_vertices; i ++) {
			offset[i] = offset[i - 1] + deg[i - 1];
		}
		unique_edges.clear();

		FILE* fout_ins_only = fopen((graph_name + "_ins_only_exact.txt").c_str(), "w");
		for(ll i = 0; i < m; i ++) edge_state[i] = false;
		time_step = 0;
		stream_size = (ll)ins_only_edges.size();
		SNAP = max(1LL, stream_size / 100);
		triangles = 0;
		fprintf(fout_ins_only,"timestep, triangles, time\n");
		adj_set.clear();
		for(int i = 0; i < n_vertices; i ++) adj_set.push_back(unordered_set<int>());
		start_time = clock();


        for(auto& edge: ins_only_edges) {
			time_step ++;
			int& a = edge.first;
			int& b = edge.second;
			if(adj_set[a].size() > adj_set[b].size()) swap(a, b);
			ll edge_idx;
			local_triangle = 0;
			auto beg = adj + offset[b];
			auto fin = adj + offset[b] + deg[b];
			for(auto& neighbor: adj_set[a]) {
				edge_idx = lower_bound(beg, fin, neighbor) - adj;
				//cout << a << " " << b << " >> " << neighbor << " " << adj[edge_idx] << endl;
				//if(adj[edge_idx] == neighbor) 
				local_triangle = local_triangle + (adj[edge_idx] == neighbor && edge_state[edge_idx]);
			}
			update_edge(a, b);
			update_edge(b, a);
			if(state == true) triangles += local_triangle;
			else triangles -= local_triangle;
			if(should_print(time_step, stream_size) == true) {
				//fprintf(fout,"%lld, %lld, %.2f\n", time_step, triangles, (tbb::tick_count::now() - start_time).seconds());
				fprintf(fout_ins_only,"%lld, %lld, %.2f\n", time_step, triangles, (clock() - start_time) / CLOCKS_PER_SEC);
				fflush(fout_ins_only);
			}
		}

		fflush(fout_ins_only);
		fflush(stdout);

		FILE* fout_fully_dyn = fopen((graph_name + "_fully_dyn_exact.txt").c_str(), "w");
		for(ll i = 0; i < m; i ++) edge_state[i] = false;
		time_step = 0;
		stream_size = (ll)fully_dyn_edges.size();
		SNAP = max(1LL, stream_size / 100);
		triangles = 0;
		fprintf(fout_fully_dyn,"timestep, triangles, time\n");
		adj_set.clear();
		for(int i = 0; i < n_vertices; i ++) adj_set.push_back(unordered_set<int>());
		start_time = clock();

		edge_cnt = (bool *)calloc(m, sizeof(bool));
		ll idx_cache = 0;

        for(ll i = 0; i < fully_dyn_edges.size(); i ++) {
			auto& edge_stream = fully_dyn_edges[i];
			time_step ++;
			int& vertex_a = edge_stream.first;
			int& vertex_b = edge_stream.second;
			if(deg[vertex_a] > deg[vertex_b] || (deg[vertex_a] == deg[vertex_b] && vertex_a > vertex_b)) {
				swap(vertex_a, vertex_b);
			}
			idx_edge = lower_bound(adj + offset[vertex_a], adj + offset[vertex_a] + deg[vertex_a], vertex_b) - adj;
			edge_cnt[idx_edge] = !edge_cnt[idx_edge];
			idx_cache ++;
			if(should_print(time_step, stream_size) == false) {
				continue;
			}
			for(ll j = i - idx_cache + 1; j <= i; j ++) {
				auto& edge = fully_dyn_edges[j];
				int& a = edge.first;
				int& b = edge.second;
				idx_edge = lower_bound(adj + offset[a], adj + offset[a] + deg[a], b) - adj;
				if(edge_cnt[idx_edge] == true) {
					edge_cnt[idx_edge] = false;
					if(adj_set[a].size() > adj_set[b].size()) swap(a, b);
					ll edge_idx;
					local_triangle = 0;
					auto beg = adj + offset[b];
					auto fin = adj + offset[b] + deg[b];
					for(auto& neighbor: adj_set[a]) {
						edge_idx = lower_bound(beg, fin, neighbor) - adj;
						//cout << a << " " << b << " >> " << neighbor << " " << adj[edge_idx] << endl;
						//if(adj[edge_idx] == neighbor) 
						//	local_triangle = local_triangle + edge_state[edge_idx] == true;
						local_triangle = local_triangle + (adj[edge_idx] == neighbor && edge_state[edge_idx]);
					}
					update_edge(a, b);
					update_edge(b, a);
					if(state == true) triangles += local_triangle;
					else triangles -= local_triangle;
				}
			}
			idx_cache = 0;
			//fprintf(fout,"%lld, %lld, %.2f\n", time_step, triangles, (tbb::tick_count::now() - start_time).seconds());
			fprintf(fout_fully_dyn,"%lld, %lld, %.2f\n", time_step, triangles, (clock() - start_time) / CLOCKS_PER_SEC);
			fflush(fout_fully_dyn);
		}
    }
    return 0;
}
