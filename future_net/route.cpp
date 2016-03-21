#include "route.h"
#include "lib_record.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <queue>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//#define SHOW_DEBUG_INFO

std::unordered_map<int, Node*> node_set;
std::unordered_map<int, Edge*> edge_set;
std::unordered_map<int, Node*> v_node_set;
int S;
int T;
int answer = 999999;
std::vector<Edge*> answer_path;

bool new_line_flag = true;

struct ACO
{
	ACO()
	{
		memset(head,-1,sizeof(head));
		memset(tail,-1,sizeof(tail));
		memset(isv_node,false,sizeof(isv_node));
		v_node.clear();
		alpha=0.5;
		beta=0.7;
		rou=0.999;
		QIN=1;
		QAS=0.001;
		QAR=0.1;
		QAC=100;
		ANTNUM=100;
		tms=1000000;
	}
	double alpha,beta,rou,QIN,QAS,QAR,QAC,ANTNUM,tms;
	struct ACO_Anspath
	{
		ACO_Anspath()
		{
			length=0;
			path.clear();
		}
		std::vector<int> path;
		int length;
	};
	ACO_Anspath ACO_anspath;
	int S,T;
	int head[650],tail[650];
	bool isv_node[650];
	std::vector<int> v_node;
	struct ACO_Edge
	{
		int num;
		int from, to;
		int h_next, t_next;
		int weight;
		double inform;
		void init(int Edge_id,int source_id,int des_id,int weight)
		{
			num=Edge_id;
			from=source_id;
			to=des_id;
			this->weight=weight;
		}
	}ACO_edge[5000];
	struct ACO_Node
	{
		int Node_id;
		std::vector<int> path;
		int v_node_num;
		int length;
	}ACO_node[650];
	void addedge(int Edge_id,int source_id,int des_id,int weight)
	{
		ACO_edge[Edge_id].init(Edge_id,source_id,des_id,weight);
		ACO_edge[Edge_id].h_next=head[source_id];
		head[source_id]=Edge_id;
		ACO_edge[Edge_id].t_next=tail[des_id];
		tail[des_id]=Edge_id;
	}
	void addvnode(int v_node_id)
	{
		isv_node[v_node_id]=true;
		v_node.push_back(v_node_id);
	}
	std::queue<int> SPFA_q;
	int SPFA_des[650];
	bool SPFA_visited[650];
	int SPFA_path[650];
	bool SPFA(int t)
	{
		while(!SPFA_q.empty()) SPFA_q.pop();
		memset(SPFA_des,0x3f,sizeof(SPFA_des));
		memset(SPFA_path,-1,sizeof(SPFA_path));
		memset(SPFA_visited,false,sizeof(SPFA_visited));
		SPFA_q.push(t);
		SPFA_des[t]=0;
		SPFA_visited[t]=true;
		while(!SPFA_q.empty())
		{
			int now=SPFA_q.front();
			SPFA_q.pop();
			for(int i=tail[now];i!=-1;i=ACO_edge[i].t_next)
			{
				int nxt=ACO_edge[i].from;
				int weight=ACO_edge[i].weight;
				if(SPFA_des[nxt]>SPFA_des[now]+weight)
				{
					SPFA_des[nxt]=SPFA_des[now]+weight;
					SPFA_path[nxt]=i;
					if(!SPFA_visited[nxt])
					{
						SPFA_visited[nxt]=true;
						SPFA_q.push(nxt);
					}
				}
			}
			SPFA_visited[now]=false;
		}
		if(SPFA_path[S]==-1) return false;
		for(int i=0;i<int(v_node.size());i++)
			if(SPFA_path[v_node[i]]==-1) return false;
		for(int i=0;i<610;i++)
			if(SPFA_path[i]!=-1)
			{
				int tmp=i;
				ACO_node[i].Node_id=i;
				ACO_node[i].v_node_num=0;
				ACO_node[i].path.clear();
				ACO_node[i].length=0;
				while(tmp!=-1)
				{
					ACO_node[i].path.push_back(tmp);
					if(isv_node[tmp]) ACO_node[i].v_node_num++;
					if(tmp!=T) ACO_node[i].length+=ACO_edge[SPFA_path[tmp]].weight;
					tmp=ACO_edge[SPFA_path[tmp]].to;
				}
			}
		return true;
	}
	int f[650][650];
	void Edge_InformInit()
	{
		memset(f,-1,sizeof(f));
		for(int i=0;ACO_edge[i].num==i;i++)
		{
			int u=ACO_edge[i].from;
			int v=ACO_edge[i].to;
			int w=ACO_edge[i].weight;
			if(f[u][v]==-1||w<ACO_edge[f[u][v]].weight) f[u][v]=i;
		}
		for(int i=0;ACO_edge[i].num==i;i++)
		{
			int u=ACO_edge[i].from;
			int v=ACO_edge[i].to;
			if(f[u][v]==i) ACO_edge[i].inform=QIN;
		}
	}
	void Edge_decrease()
	{
		for(int i=0;ACO_edge[i].num==i;i++)
			ACO_edge[i].inform*=rou;
	}
	struct Ant
	{
		int kind;
		int id;
		int start, end;
		bool visited[1000];
		int now;
		int v_node_num;
		int length;
		std::vector<int> Ant_path;
		void Init(int _id,int _kind)
		{
			kind=_kind;
			id=_id;
			length=0;
			if(kind==1) start=S,end=T;
			else start=T,end=S;
			memset(visited,false,sizeof(visited));
			v_node_num=0;
			now=start;
			visited[now]=true;
			Ant_path.clear();
		}
		void NextNode()
		{
			srand((int)time(0));
			if(kind==1)
			{
				double sum=0;
				for(int i=head[now];i!=-1;i=ACO_edge[i].h_next)
					if(!visited[ACO_edge[i].to]&&SPFA_path[ACO_edge[i].to]!=-1)
					{
						sum+=pow(ACO_edge[i].inform,alpha)*pow(1.0/ACO_edge[i].weight,beta);
					}
				if(sum==0) Rebirth();
				double tmp=(double)rand()/RAND_MAX*sum*0.99;
				int i;
				for(i=head[now];i!=-1;i=ACO_edge[i].h_next)
					if(!visited[ACO_edge[i].to]&&(SPFA_path[ACO_edge[i].to]!=-1))
					{
						tmp-=pow(ACO_edge[i].inform,alpha)*pow(1.0/ACO_edge[i].weight,beta);
						if(tmp<0) break;
					}
				Ant_path.push_back(i);
				now=ACO_edge[i].to;
				length+=ACO_edge[i].weight;
				if(v_node_num+ACO_node[now]==(int)v_node.size()) Check();
				if(isv_node[now]) v_node_num++;
				visited[now]=true;
				if(now==T) Rebirth();
			}
			else
			{
				double sum=0;
				for(int i=tail[now];i!=-1;i=ACO_edge[i].t_next)
					if(!visited[ACO_edge[i].from])
					{
						sum+=pow(ACO_edge[i].inform,alpha)*pow(1.0/ACO_edge[i].weight,beta);
					}
				if(sum==0) Rebirth();
				double tmp=(double)rand()/RAND_MAX*sum*0.99;
				int i;
				for(i=tail[now];i!=-1;i=ACO_edge[i].t_next)
					if(!visited[ACO_edge[i].from])
					{
						tmp-=pow(ACO_edge[i].inform,alpha)*pow(1.0/ACO_edge[i].weight,beta);
						if(tmp<0) break;
					}
				Ant_path.push_back(i);
				now=ACO_edge[i].from;
				length+=ACO_edge[i].weight;
				if(isv_node[now]) v_node_num++;
				visited[now]=true;
				if(now==S) Rebirth();
			}
		}
		void Check()
		{
			bool flag=true;
			if(length+ACO_node[now].length>=ACO_anspath.length&&ACO_anspath.length>0) return;
			for(int i=0;i<(int)ACO_node[now].path.size();i++)
			{
				if(visited[ACO_node[now].path[i]])
				{
					flag=false;
					break;
				}
			}
			if(flag)
			{
				ACO_anspath.length=length+ACO_node[now].length;
				ACO_anspath.path.clear();
				for(int i=0;i<(int)Ant_path.size();i++) ACO_anspath.path.push_back(ACO_edge[Ant_path[i]].from);
				for(int i=0;i<(int)ACO_node[now].path.size();i++) ACO_anspath.path.push_back(ACO_node[now].path[i]);
			}
		}
		void Rebirth()
		{
			Update();
			Init();
		}
		void Update()
		{
			if(kind==1)
			{
				if(now==T)
				{
					for(int i=0;i<(int)Ant_path.size();i++)
						ACO_edge[Ant_path[i]].inform+=QAC/length;
				}
				else
				{
					bool flag=false;
					for(int i=(int)Ant_path.size()-1;i>=0;i--)
						if(!flag)
						{
							ACO_edge[Ant_path[i]].inform+=QAS;
							if(isv_node[ACO_edge[Ant_path[i]].from]) flag=true;
						}
						else
						{
							ACO_edge[Ant_path[i]].inform+=QAR/ACO_edge[Ant_path[i]].weight;
						}
				}
			}
			else
			{
				if(now==S)
				{
					for(int i=0;i<(int)Ant_path.size();i++)
						ACO_edge[Ant_path[i]].inform+=QAC/length;
				}
				else
				{
					for(int i=0;i<(int)Ant_path.size();i++)
						ACO_edge[Ant_path[i]].inform+=QAR/ACO_edge[Ant_path[i]].weight;
				}
			}
		}
	};
	Ant ant[520],rant[520];
	void solve()
	{
		bool isna=SPFA(T);
		if(!isna) return;
		Edge_InformInit();
		for(int i=0;i<ANTNUM;i++) ant[i].Init(i,1);
		for(int i=0;i<ANTNUM;i++) rant[i].Init(i,2);
		for(int i=1;i<=tms;i++)
		{
			for(int i=0;i<ANTNUM;i++) ant[i].NextNode();
			for(int i=0;i<ANTNUM;i++) rant[i].NextNode();
			Edge_decrease();
		}
		if(ACO_anspath.length==0) return;
		for(int i=0;i<(int)ACO_anspath.path.size();i++)
			record_result(ACO_anspath.path[i]);
	}
};

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
ACO aco;
void search_route(char *topo[5000], int edge_num, char *demand)
{   
	if (!read_topo(topo, edge_num, node_set, edge_set) ||
        !read_demand(demand, v_node_set))
    {
        return;
    }

    std::vector<Edge*> path;
	aco.solve();
    //naive_search(node_set[S], 0, path);
    
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

    //for (size_t i = 0; i < answer_path.size(); i++)
   // {
    //    record_result(answer_path[i]->id);
    //}
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
        aco.addedge(edge_id, source_id, des_id, weight);
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
    aco.S=S;aco.T=T;
	int v_node_id;
    std::string id_str;

    std::stringstream ss(v_str);
    while (getline(ss, id_str, '|')) {
        v_node_id = atoi(id_str.c_str());
        aco.addvnode(v_node_id);
		if (node_set.find(v_node_id) == node_set.end()) {
            std::cout << "v-node not in node_set" << std::endl;
            return false;
        }
        v_node_set[v_node_id] = node_set[v_node_id];
    }
    return true;
}


