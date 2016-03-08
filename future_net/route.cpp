#include "route.h"
#include "lib_record.h"

#include <iostream>
#include <sstream>
#include <cstdio>

//#define SHOW_DEBUG_INFO

std::unordered_map<int, Node*> node_set;
std::unordered_map<int, Edge*> edge_set;
std::unordered_map<int, Node*> v_node_set;
int S;
int T;
int answer = 999999;
std::vector<Edge*> answer_path;

bool new_line_flag = true;

void naive_search(Node* node, int distance, std::vector<Edge*>& path) {
    
    #ifdef SHOW_DEBUG_INFO
    {    
        if (!new_line_flag) {
            std::cout << "--";
        }

        if (new_line_flag)
        {
            std::cout << std::endl;
            for (int j = 0; j < path.size(); j++) 
                std::cout << "├--";
            new_line_flag = false;
        }
        std::cout << node->id;
    }
    #endif

    if (node->id == T) {
        for (std::unordered_map<int, Node*>::iterator it = v_node_set.begin();
                it != v_node_set.end(); it++) {
            if (!it->second->visited)
                return;
        }
        if (distance < answer) { 
            #ifdef SHOW_DEBUG_INFO
            {   
                std::cout << " // new answer : " << distance << "[";
                for (int i = 0; i < path.size(); i++) {
                    std::cout << path[i]->from->id << "-->";
                }
                std::cout << T << "]";
            }
            #endif
            answer = distance;
            answer_path = path;
        }
    }

    for (int i = 0; i < node->outters.size(); i++) {
        Node* to = node->outters[i]->to;
        if (to->visited) {
            continue;
        }
        to->visited = true;
        path.push_back(node->outters[i]);
        naive_search(to, distance + node->outters[i]->weight, path);
        to->visited = false;
        path.pop_back();
        new_line_flag = true;
    }
}

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{   
    if (!read_topo(topo, edge_num, node_set, edge_set) ||
        !read_demand(demand, v_node_set))
    {
        return;
    }

    std::vector<Edge*> path;
    naive_search(node_set[S], 0, path);
    
    #ifdef SHOW_DEBUG_INFO
    {
        std::cout << std::endl;
        std::cout << "answer : " << answer << std::endl;
        for (size_t i = 0; i + 1 < answer_path.size(); i++)
        {
            std::cout << answer_path[i]->from->id << "->";
        }
        std::cout << answer_path[answer_path.size() - 1]->from->id
                << "->"
                << answer_path[answer_path.size() - 1]->to->id << std::endl;
    }
    #endif

    for (size_t i = 0; i < answer_path.size(); i++)
    {
        record_result(answer_path[i]->id);
    }
}

bool read_topo(char* topo[5000], 
        int edge_num,
        std::unordered_map<int, Node*>& node_set,
        std::unordered_map<int, Edge*>& edge_set)
{
    int edge_id;
    int source_id;
    int des_id;
    int weight;

    for (int line_index = 0; line_index < edge_num; line_index++)
    {
        sscanf(topo[line_index], "%d,%d,%d,%d", &edge_id, &source_id, &des_id, &weight);
        if (node_set.find(source_id) == node_set.end())
        {
            node_set[source_id] = new Node(source_id);
        }

        if (node_set.find(des_id) == node_set.end()) 
        {
            node_set[des_id] = new Node(des_id);
        }

        if (edge_set.find(edge_id) != edge_set.end()) 
        {
            std::cout << "dumplicted edge id" << std::endl;
            return false;
        } 
        else 
        {
            edge_set[edge_id] = 
                new Edge(edge_id, node_set[source_id], node_set[des_id], weight);
        }
        node_set[source_id]->outters.push_back(edge_set[edge_id]);
        node_set[des_id]->inners.push_back(edge_set[edge_id]);
    }
    return true;
}

bool read_demand(char* demand, 
        std::unordered_map<int, Node*>& v_node_set)
{
    char v_str[4000];
    sscanf(demand, "%d,%d,%s", &S, &T, v_str);
    int v_node_id;
    std::string id_str;

    std::stringstream ss(v_str);
    while (getline(ss, id_str, '|')) {
        v_node_id = atoi(id_str.c_str());
        if (node_set.find(v_node_id) == node_set.end()) {
            std::cout << "v-node not in node_set" << std::endl;
            return false;
        }
        v_node_set[v_node_id] = node_set[v_node_id];
    }
    return true;
}


