#include <iostream>
#include <vector>
#include <set>

using namespace std;


const long long INF = 1e13 + 7;

struct TVert {
    TVert() = default;
    TVert(int to, int weight): to(to), weight(weight) {}
    int to;
    int weight;
};

void Dijkstra(int n, int m, int start, int finish, vector<vector<TVert>> graph) {
    vector<long long> d(n, INF);
    d[start] = 0;

    set<int> used;
    used.insert(start);

    while (!used.empty()) {
        int cVert = *used.begin();

        used.erase(used.begin());

        for (int i = 0; i < graph[cVert].size(); i++) {
            if (d[graph[cVert][i].to] > d[cVert] + graph[cVert][i].weight) {
                used.erase(graph[cVert][i].to);
                d[graph[cVert][i].to] = d[cVert] + graph[cVert][i].weight;
                used.insert(graph[cVert][i].to);
            }
        } 
    }

    if (d[finish] != INF) {
        cout << d[finish] << '\n';
    } else {
        cout << "No solution\n";
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    int n;
    int m;
    int start;
    int finish;

    while (cin >> n >> m >> start >> finish) {
        vector<vector<TVert>> graph(n);

        int v1, v2, weight;
        
        for (int i = 0; i < m; i++) {
            cin >> v1 >> v2 >> weight;

            v1--;
            v2--;

            graph[v1].push_back({v2, weight});
            graph[v2].push_back({v1, weight});
        }

        start--;
        finish--;

        Dijkstra(n, m, start, finish, graph);
    }

    return 0;
}
