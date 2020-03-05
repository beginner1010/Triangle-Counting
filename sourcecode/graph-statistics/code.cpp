#include <bits/stdc++.h>

using namespace std;

#define ll long long

const ll SHIFT = (ll)1e9;
const ll LONG_THRESHOLD = (ll)1e15;

vector < unordered_set<int> > adj;
unordered_map<int, int> hash_map;
unordered_set<ll> hash_set;
vector< pair<int,int> > edges;

string graph_names[5] = {"dblp", "livejournal", "orkut", "wiki", "twitter-cut"};

inline ll to_hash(const int& a, const int& b) {
       if(a > b) return b * SHIFT + a;
       else return a * SHIFT + b;
}

inline ll choose2(ll n) {
       return (n * (n - 1)) >> 1;
}

void add_vertex(int& a) {
     if(hash_map.find(a) == hash_map.end()) {
                    hash_map[a] = adj.size();
                    adj.push_back(unordered_set<int>());
     }
     a = hash_map[a];
}

void add_edge(int a, int b) {
	if(a == b)
		return;
     	add_vertex(a);
     	add_vertex(b);
     	ll hsh = to_hash(a, b);
     	if(hash_set.find(hsh) == hash_set.end()) {
		adj[a].insert(b);
		adj[b].insert(a);
		hash_set.insert(hsh);
		edges.push_back({a, b});
     	}
}

inline bool higher_rank(int a, int b) {
       if(adj[a].size() > adj[b].size()) return true;
       if(adj[a].size() == adj[b].size() && a > b) return true;
       return false;
}

inline bool is_int_num(const string& str) {
	if ((int)str.size() > 9)
		return false;
	for (int i = 0; i < (int)str.size(); i++) {
		if ('0' > str[i] || str[i] > '9')
			return false;
	}
	return true;
}

int to_int(const string& str) {
	std::stringstream ss; ss << str;
	int x; ss >> x;
	return x;
}

int main() {
    cin.tie(0); ios_base::sync_with_stdio(false);
    
    for(string& graph_name: graph_names) {
                string input_address = ("/work/snt-free/triangle/input/in." + graph_name);
                FILE* fin = fopen(input_address.c_str(), "r");
                FILE* fout = fopen((graph_name + "_stat.txt").c_str(), "w");
                hash_map.clear();
                hash_set.clear();
                adj.clear();
                edges.clear();                
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
                                   int a = to_int(vec_str[0]), b = to_int(vec_str[1]);
                                   add_edge(a, b);
                }
                ll wedges = 0;
                ll ordered_wedges = 0;
                int vertex_id = 0;
                for(auto& lst: adj) {
                          wedges += choose2(lst.size());
                          vertex_id ++;
                          int cnt = 0;
                          for(auto& neighbor: lst)     if(higher_rank(neighbor, vertex_id)) cnt ++;
                          ordered_wedges += choose2(cnt);
                }
                ll triangles = 0;
                vector<ll> kappa; // to avoid long long int overflow, we used a vector
                ll kappa_val = 0;
                vector<ll> tri_sum_degree; // used for the variance of IMPR_ESPAR
                ll tri_sum_degree_val = 0;
                
                for(auto& edge: edges) {
                          int a = edge.first;
                          int b = edge.second;
                          if(adj[a].size() > adj[b].size()) swap(a, b);
                          ll triangle_e = 0;
                          for(auto& neighbor: adj[a]) {
                                    if(adj[b].find(neighbor) != adj[b].end())
                                                             triangle_e ++;
                          }
                          tri_sum_degree_val += triangle_e * adj[a].size();
                          if(tri_sum_degree_val > LONG_THRESHOLD) {
                                                tri_sum_degree.push_back(tri_sum_degree_val);
                                                tri_sum_degree_val = 0;
                          }
                          kappa_val += choose2(triangle_e);
                          if(kappa_val > LONG_THRESHOLD) {
                                      kappa.push_back(kappa_val);
                                      kappa_val = 0;
                          }
                          triangles += triangle_e;
                }

                if(tri_sum_degree_val > 0) tri_sum_degree.push_back(tri_sum_degree_val);
                if(kappa_val > 0) kappa.push_back(kappa_val);
                triangles /= 3;
                
                fprintf(fout,"Graph: %s\n", graph_name.c_str());
		fprintf(fout,"Vertices: %d\n", (int)hash_map.size());
		fprintf(fout,"Edges: %d\n", (int)edges.size());
                fprintf(fout,"Wedges: %lld\n", wedges);
                fprintf(fout,"Ordered wedges: %lld\n", ordered_wedges);
                fprintf(fout,"Triangles: %lld\n", triangles);
                
                fprintf(fout,"Tri_sum_degree:");
                for(ll& val: tri_sum_degree) fprintf(fout," %lld + ", val);
                fprintf(fout,"\n");
                
                fprintf(fout,"Kappa:");
                for(ll& val: kappa) fprintf(fout," %lld + ", val);
                fprintf(fout,"\n");
		fflush(fout);
		fflush(stdout);
    }
    return 0;
}
