#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>
#include <limits>
#include <chrono>
#include <iomanip>
#include <cassert>


struct Node {
    double lon = 0.0; 
    double lat = 0.0; 
    std::vector<std::pair<Node*, double>> edges;
};


class Graph {
public:
    Node* addNode(double lon, double lat) {
        std::ostringstream keyStream;
        keyStream << std::fixed << std::setprecision(10) << lon << "," << lat;
        std::string key = keyStream.str();

        auto it = node_map.find(key);
        if (it != node_map.end()) {
            return it->second;
        }

        auto node = std::make_unique<Node>();
        node->lon = lon;
        node->lat = lat;
        Node* nodePtr = node.get(); 
        nodes.push_back(std::move(node));
        node_map[key] = nodePtr;
        return nodePtr;
    }

    Node* getNode(double lon, double lat) {
        std::ostringstream keyStream;
        keyStream << std::fixed << std::setprecision(10) << lon << "," << lat;
        std::string key = keyStream.str();

        auto it = node_map.find(key);
        if (it != node_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    void readFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Не удалось открыть файл: " << filename << "\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream lineStream(line);
            std::string parentData, edgesData;
            if (std::getline(lineStream, parentData, ':')) {
                std::replace(parentData.begin(), parentData.end(), ',', ' ');
                std::istringstream parentStream(parentData);

                double lon1, lat1;
                if (!(parentStream >> lon1 >> lat1)) {
                    std::cerr << "[ERROR] Ошибка парсинга узла: " << parentData << "\n";
                    continue; 
                }

                Node* parentNode = addNode(lon1, lat1);


                while (std::getline(lineStream, edgesData, ';')) {
                    std::replace(edgesData.begin(), edgesData.end(), ',', ' ');
                    std::istringstream edgeStream(edgesData);

                    double lon2, lat2, weight;
                    if (!(edgeStream >> lon2 >> lat2 >> weight)) {
                        std::cerr << "[ERROR] Ошибка парсинга ребра: " << edgesData << "\n";
                        continue;
                    }

                    Node* childNode = addNode(lon2, lat2);

                    parentNode->edges.emplace_back(childNode, weight);
                    childNode->edges.emplace_back(parentNode, weight);
                }
            }
        }

        file.close();
    }


    Node* findClosestNode(double lat, double lon) const {
        double minDist = std::numeric_limits<double>::max();
        Node* closest = nullptr;

        for (auto& up : const_cast<Graph*>(this)->nodes) {
            Node* n = up.get();
            double dx = n->lon - lon;
            double dy = n->lat - lat;
            double dist = std::sqrt(dx * dx + dy * dy);
            if (dist < minDist) {
                minDist = dist;
                closest = n;
            }
        }
        return closest;
    }

    void printPath(const std::vector<Node*>& path) const {
        if (path.empty()) {
            std::cout << "Путь не найден.\n";
            return;
        }
        double totalWeight = 0.0;
        std::cout << "Путь:\n";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << "(" << path[i]->lat << ", " << path[i]->lon << ")";
            if (i < path.size() - 1) {
                std::cout << " -> ";
                Node* cur = path[i];
                Node* nxt = path[i + 1];
                for (auto& edge : cur->edges) {
                    if (edge.first == nxt) {
                        totalWeight += edge.second;
                        break;
                    }
                }
            }
        }
        std::cout << "\nОбщая длина пути: " << totalWeight << "\n";
    }


private:
    std::vector<std::unique_ptr<Node>> nodes;
    std::unordered_map<std::string, Node*> node_map;
};

std::vector<Node*> BFS(Node* start, Node* goal) {
    if (!start || !goal) return {};

    std::unordered_map<Node*, bool> visited;     
    std::unordered_map<Node*, Node*> cameFrom;   
    std::queue<Node*> q;                         

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        Node* current = q.front();
        q.pop();

        if (current == goal) {
            std::vector<Node*> path;
            for (Node* p = goal; p; p = cameFrom[p]) {
                path.push_back(p);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto& edge : current->edges) {
            Node* neighbor = edge.first;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                cameFrom[neighbor] = current;
                q.push(neighbor);
            }
        }
    }
    return {};
}

std::vector<Node*> DFS(Node* start, Node* goal) {
    if (!start || !goal) return {};

    std::unordered_map<Node*, bool> visited;
    std::unordered_map<Node*, Node*> cameFrom;
    std::stack<Node*> st;

    st.push(start);
    visited[start] = true;

    while (!st.empty()) {
        Node* current = st.top();
        st.pop();

        if (current == goal) {
            std::vector<Node*> path;
            for (Node* p = goal; p; p = cameFrom[p]) {
                path.push_back(p);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto& edge : current->edges) {
            Node* neighbor = edge.first;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                cameFrom[neighbor] = current;
                st.push(neighbor);
            }
        }
    }
    return {};
}


std::vector<Node*> Dijkstra(Graph& graph, Node* start, Node* goal) {
    if (!start || !goal) return {};

    std::unordered_map<Node*, double> dist;      
    std::unordered_map<Node*, Node*> cameFrom; 
    std::unordered_map<Node*, bool> visited;

    dist[start] = 0.0; 

    using PQItem = std::pair<double, Node*>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<>> pq;
    pq.emplace(0.0, start);

    while (!pq.empty()) {
        PQItem topPair = pq.top();
        pq.pop();

        double currentDist = topPair.first;
        Node* currentNode = topPair.second;

        if (visited[currentNode]) {
            continue;
        }
        visited[currentNode] = true;

        if (currentNode == goal) {
            std::vector<Node*> path;
            for (Node* p = goal; p; p = cameFrom[p]) {
                path.push_back(p);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }


        for (auto& edge : currentNode->edges) {
            Node* neighbor = edge.first;
            double weight = edge.second;
            double alt = currentDist + weight;

            if (dist.find(neighbor) == dist.end()) {
                dist[neighbor] = std::numeric_limits<double>::infinity();
            }
            if (alt < dist[neighbor]) {
                dist[neighbor] = alt;
                cameFrom[neighbor] = currentNode;
                pq.emplace(alt, neighbor);
            }
        }
    }

    return {};
}


static double heuristic(Node* a, Node* b) {
    double dx = a->lon - b->lon;
    double dy = a->lat - b->lat;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<Node*> AStar(Graph& graph, Node* start, Node* goal) {
    if (!start || !goal) return {};

    std::unordered_map<Node*, double> gScore;
    std::unordered_map<Node*, double> fScore;
    std::unordered_map<Node*, Node*> cameFrom;
    std::unordered_map<Node*, bool> visited;

    gScore[start] = 0.0;
    fScore[start] = heuristic(start, goal);

    using PQItem = std::pair<double, Node*>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<>> openSet;
    openSet.emplace(fScore[start], start);

    while (!openSet.empty()) {
        PQItem topPair = openSet.top();
        openSet.pop();

        double currentF = topPair.first;
        Node* currentNode = topPair.second;

        if (currentNode == goal) {
            std::vector<Node*> path;
            for (Node* p = goal; p; p = cameFrom[p]) {
                path.push_back(p);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        visited[currentNode] = true;

        for (auto& edge : currentNode->edges) {
            Node* neighbor = edge.first;
            double weight = edge.second;
            if (visited[neighbor]) {
                continue;
            }

            double tentative_gScore = gScore[currentNode] + weight;

            if (gScore.find(neighbor) == gScore.end()) {
                gScore[neighbor] = std::numeric_limits<double>::infinity();
            }

            if (tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = currentNode;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = tentative_gScore + heuristic(neighbor, goal);
                openSet.emplace(fScore[neighbor], neighbor);
            }
        }
    }
    return {};
}


double computePathWeight(const std::vector<Node*>& path) {
    double total = 0.0;
    for (size_t i = 0; i + 1 < path.size(); i++) {
        Node* cur = path[i];
        Node* nxt = path[i + 1];
        for (auto& e : cur->edges) {
            if (e.first == nxt) {
                total += e.second;
                break;
            }
        }
    }
    return total;
}


int main() {
    setlocale(LC_ALL, "Russian");

    Graph graph;
    graph.readFromFile("spb_graph (1).txt");

    double startLat = 60.0318296;
    double startLon = 30.3645534;
    double endLat = 59.9572707;
    double endLon = 30.3082117;

    Node* startNode = graph.findClosestNode(startLat, startLon);
    Node* endNode = graph.findClosestNode(endLat, endLon);

    if (!startNode || !endNode) {
        std::cerr << "Не найдены узлы для старта/цели.\n";
        return 1;
    }

    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<Node*> pathBFS = BFS(startNode, endNode);
        auto t2 = std::chrono::high_resolution_clock::now();

        std::cout << "\n[BFS] путь:\n";
        graph.printPath(pathBFS);

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "время BFS: " << duration << " мс\n";
    }

    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<Node*> pathDFS = DFS(startNode, endNode);
        auto t2 = std::chrono::high_resolution_clock::now();

        std::cout << "\n[DFS] путь:\n";
        graph.printPath(pathDFS);

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "время DFS: " << duration << " мс\n";
    }

    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<Node*> pathDij = Dijkstra(graph, startNode, endNode);
        auto t2 = std::chrono::high_resolution_clock::now();

        std::cout << "\n[Dijkstra] путь:\n";
        graph.printPath(pathDij);

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "время Dijkstra: " << duration << " мс\n";
    }

    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<Node*> pathAStar = AStar(graph, startNode, endNode);
        auto t2 = std::chrono::high_resolution_clock::now();

        std::cout << "\n[A*] путь:\n";
        graph.printPath(pathAStar);

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::cout << "время A*: " << duration << " мс\n";
    }

    return 0;
}
