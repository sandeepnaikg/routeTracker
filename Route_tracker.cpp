
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <queue>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Node
{
    string name;
    vector<pair<Node *, double>> neighbors; // (neighbor, distance)
    double g = numeric_limits<double>::infinity();
    double h = 0.0;
    double f = numeric_limits<double>::infinity();
    Node *parent = nullptr;
    double traffic = 0.0; // 0.0 (free) … 1.0 (jam)

    explicit Node(string n) : name(std::move(n)) {}
};

class Map
{
public:
    unordered_map<string, Node *> nodes;

    ~Map()
    {
        // Free allocated nodes
        for (auto &p : nodes)
            delete p.second;
    }

    static string to_lower(string s)
    {
        transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c)
                  { return std::tolower(c); });
        return s;
    }

    Node *addNode(const string &rawName)
    {
        string name = to_lower(rawName);
        if (nodes.find(name) == nodes.end())
            nodes[name] = new Node(name);
        return nodes[name];
    }

    void addEdge(const string &fromRaw, const string &toRaw, double distance)
    {
        Node *f = addNode(fromRaw);
        Node *t = addNode(toRaw);
        f->neighbors.push_back({t, distance});
        t->neighbors.push_back({f, distance}); // undirected
    }

    Node *getNode(const string &rawName)
    {
        string name = to_lower(rawName);
        auto it = nodes.find(name);
        return (it == nodes.end()) ? nullptr : it->second;
    }

    double get_Distance(Node *a, Node *b)
    {
        for (auto &n : a->neighbors)
            if (n.first == b)
                return n.second;
        return numeric_limits<double>::infinity();
    }
};

// Heuristic — set to 0 to make this equivalent to Dijkstra (always optimal with weights)
static inline double heuristic(Node *, Node *) { return 0.0; }

// A* (Dijkstra when heuristic==0)
vector<Node *> A_star(Map &map, Node *start, Node *goal)
{
    // Reset per-node state
    for (auto &p : map.nodes)
    {
        p.second->g = numeric_limits<double>::infinity();
        p.second->h = 0.0;
        p.second->f = numeric_limits<double>::infinity();
        p.second->parent = nullptr;
    }

    start->g = 0.0;
    start->h = heuristic(start, goal);
    start->f = start->g + start->h;

    auto cmp = [](Node *a, Node *b)
    { return a->f > b->f; };
    priority_queue<Node *, vector<Node *>, decltype(cmp)> open(cmp);
    open.push(start);

    unordered_set<Node *> closed;

    while (!open.empty())
    {
        Node *current = open.top();
        open.pop();

        if (closed.count(current))
            continue;
        closed.insert(current);

        if (current == goal)
        {
            vector<Node *> path;
            for (Node *n = current; n; n = n->parent)
                path.push_back(n);
            reverse(path.begin(), path.end());
            return path;
        }

        for (auto &p : current->neighbors)
        {
            Node *neighbor = p.first;
            double edge_dist = p.second;

            if (closed.count(neighbor))
                continue;

            // Skip neighbors with heavy traffic, but always allow the goal
            if (neighbor->traffic >= 0.8 && neighbor != goal)
                continue;

            double tentative_g = current->g + edge_dist;
            if (tentative_g < neighbor->g)
            {
                neighbor->parent = current;
                neighbor->g = tentative_g;
                neighbor->h = heuristic(neighbor, goal);
                neighbor->f = neighbor->g + neighbor->h;
                open.push(neighbor); // no decrease-key; duplicates are skipped via `closed`
            }
        }
    }
    return {}; // no path
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Map map;

    // Simple Delhi–NCR example graph
    map.addEdge("delhi", "noida", 10);
    map.addEdge("delhi", "gurgaon", 15);
    map.addEdge("noida", "ghaziabad", 7);
    map.addEdge("gurgaon", "faridabad", 8);
    map.addEdge("ghaziabad", "haryana", 20);
    map.addEdge("faridabad", "haryana", 12);

    // Seed random traffic levels
    srand(static_cast<unsigned int>(time(nullptr)));
    for (auto &pair : map.nodes)
    {
        pair.second->traffic = (rand() % 101) / 100.0; // 0.00–1.00
    }

    // Read input (case-insensitive)
    string start_name, end_name;
    cout << "Enter the name of your current location: ";
    getline(cin, start_name);
    cout << "Enter the name of your desired destination: ";
    getline(cin, end_name);

    Node *start = map.getNode(start_name);
    Node *goal = map.getNode(end_name);

    if (!start)
    {
        cerr << "Error: Location '" << start_name << "' not found in map.\n";
        return 1;
    }
    if (!goal)
    {
        cerr << "Error: Location '" << end_name << "' not found in map.\n";
        return 1;
    }

    // Ensure start/goal are never blocked by traffic
    start->traffic = min(start->traffic, 0.79);
    goal->traffic = min(goal->traffic, 0.79);

    vector<Node *> path = A_star(map, start, goal);

    if (path.empty())
    {
        cout << "\nNo available route found (likely due to heavy traffic on all alternatives).\n";
        cout << "Try again; traffic is randomized each run.\n";
        return 0;
    }

    cout << "\nBest route found:\n";
    double total_distance = 0.0;
    for (size_t i = 0; i + 1 < path.size(); ++i)
    {
        cout << path[i]->name << " -> ";
        total_distance += map.get_Distance(path[i], path[i + 1]);
    }
    cout << path.back()->name << "\n";

    cout << "\nTotal distance: " << fixed << setprecision(2) << total_distance << " units\n";

    cout << "\nTraffic levels (0 = free, 1 = jam):\n";
    for (auto &pair : map.nodes)
    {
        cout << setw(12) << left << pair.first << " : " << fixed << setprecision(2) << pair.second->traffic << "\n";
    }

    return 0;
}
