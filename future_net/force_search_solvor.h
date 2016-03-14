#ifndef FORCE_SEARCH_SOLVER_H
#define FORCE_SEARCH_SOLVER_H

#include <vector>
#include <unordered_map>

struct Edge;
// 节点
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

// 边
struct Edge 
{
    Edge(int _id, Node* _from, Node* _to, int _weight)
            :id(_id), from(_from), to(_to), weight(_weight) {}
    int id;
    Node* from;
    Node* to;
    int weight;
};

class ForceSearchSolvor
{   
public:
    ForceSearchSolvor(){}

    // 算法入口
    void solve(char *graph[5000], int edge_num, char *condition);

private:
    // 搜索
    void force_search(Node* node, int distance, std::vector<Edge*>& path);

    // 读取 graph 到 node_set 和 edge_set
    bool read_topo(char * topo[5000], 
            int edge_num,
            std::unordered_map<int, Node*>& node_set,
            std::unordered_map<int, Edge*>& edge_set);

    // 读取 demand 到 v_node_set
    bool read_demand(char* demand, 
            std::unordered_map<int, Node*>& v_node_set);

    // 节点集合
    std::unordered_map<int, Node*> node_set;
    
    // 边集合
    std::unordered_map<int, Edge*> edge_set;
    
    // 必须经过节点集合
    std::unordered_map<int, Node*> v_node_set;
    
    // 源点id
    int S;

    // 汇点id
    int T;

    int answer = 999999;
    std::vector<Edge*> answer_path;

    bool new_line_flag;
};

#endif  //FORCE_SEARCH_SOLVER_H