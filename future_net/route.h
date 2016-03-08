#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <vector>
#include <unordered_map>

struct Edge;
struct Node 
{
    explicit Node(int _id) : id(_id) 
    {
        visited = false;
    }
    int id;
    bool visited;
    std::vector<Edge*> outters;
    std::vector<Edge*> inners;
};

struct Edge 
{
    Edge(int _id, Node* _from, Node* _to, int _weight)
            :id(_id), from(_from), to(_to), weight(_weight) {}
    int id;
    Node* from;
    Node* to;
    int weight;
};

void search_route(char *graph[5000], int edge_num, char *condition);

// 读取 graph 到 node_set 和 edge_set
bool read_topo(char * topo[5000], 
        int edge_num,
        std::unordered_map<int, Node*>& node_set,
        std::unordered_map<int, Edge*>& edge_set);

// 读取 demand 到 v_node_set
bool read_demand(char* demand, 
        std::unordered_map<int, Node*>& v_node_set);

#endif
