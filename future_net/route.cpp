#include "route.h"
#include "force_search_solvor.h"
#include "lib_record.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <queue>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>
#include <math.h>
//#define SHOW_DEBUG_INFO
//#define SHOW_POINT
namespace ACO
{
	bool isv_node[650];
	std::vector<int> v_node;
	double alpha,beta,rou,QIN,QAS,QAR,QAC,ANTNUM,tms;
	int S,T;
	int head[650],tail[650];

	void init()
	{
		memset(head,-1,sizeof(head));
		memset(tail,-1,sizeof(tail));
		memset(isv_node,0,sizeof(isv_node));
		v_node.clear();
		alpha=0.5;
		beta=0.7;
		rou=0.999;
		QIN=1;
		QAS=0.001;
		QAR=0.1;
		QAC=100;
		ANTNUM=400;
		tms=10000;
	}

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
		memset(SPFA_visited,0,sizeof(SPFA_visited));
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
				while(true)
				{
					ACO_node[i].path.push_back(tmp);
#ifdef SHOW_DEBUG_INFO
					{
						printf("%d--->",tmp);
					}
#endif
					if(isv_node[tmp]) ACO_node[i].v_node_num++;
					if(tmp!=T) ACO_node[i].length+=ACO_edge[SPFA_path[tmp]].weight;
					else break;
					tmp=ACO_edge[SPFA_path[tmp]].to;
				}
#ifdef SHOW_DEBUG_INFO
				{
					printf("\n");
				}
#endif
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
			if(kind==1)
			{
				double sum=0;
				for(int i=head[now];i!=-1;i=ACO_edge[i].h_next)
					if(!visited[ACO_edge[i].to]&&(SPFA_path[ACO_edge[i].to]!=-1||ACO_edge[i].to==T))
					{
						sum+=pow(ACO_edge[i].inform,alpha)*pow(1.0/ACO_edge[i].weight,beta);
					}
				if(sum==0) Rebirth();
				double tmp=(double)rand()/RAND_MAX*sum*0.99;
				int i;
				for(i=head[now];i!=-1;i=ACO_edge[i].h_next)
					if(!visited[ACO_edge[i].to]&&((SPFA_path[ACO_edge[i].to]!=-1)||ACO_edge[i].to==T))
					{
						tmp-=pow(ACO_edge[i].inform,alpha)*pow(1.0/ACO_edge[i].weight,beta);
						if(tmp<0) break;
					}
				Ant_path.push_back(i);
				now=ACO_edge[i].to;
				length+=ACO_edge[i].weight;
				if(v_node_num+ACO_node[now].v_node_num==(int)v_node.size()) Check();
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
#ifdef SHOW_DEBUG_INFO
			printf("Ant NO.%d kind %d  :",id,kind);
			if(kind==1)
			{
				for(int i=0;i<(int)Ant_path.size();i++) printf("%d--->",ACO_edge[Ant_path[i]].from);
				printf("%d\n",now);
			}
			else
			{
				for(int i=0;i<(int)Ant_path.size();i++) printf("%d<---",ACO_edge[Ant_path[i]].to);
				printf("%d\n",now);
			}
#endif
			Update();
			Init(this->id, this->kind);
		}
		void Update()
		{
			if(kind==1)
			{
				if(now==T&&v_node_num==(int)v_node.size())
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
				if(now==S&&v_node_num==(int)v_node.size())
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
	void readtopo(char *topo[5000],int edge_num)
	{
		int edge_id;
		int source_id;
		int des_id;
		int weight;
		for(int i=0;i<edge_num;i++)
		{
			sscanf(topo[i],"%d,%d,%d,%d",&edge_id,&source_id,&des_id,&weight);
			addedge(edge_id,source_id,des_id,weight);
		}
	}
	void readdemand(char *demand)
	{
		char v_str[4000];
		sscanf(demand,"%d,%d,%s",&S,&T,v_str);
		int len=strlen(v_str);
		int now=0;
		for(int i=0;i<len;i++)
		{
			if(v_str[i]=='|') 
			{
				addvnode(now);
				now=0;
			}
			else now=now*10+v_str[i]-'0';
		}
		addvnode(now);
	}
	Ant ant[520],rant[520];
	void solve(char *topo[5000],int edge_num,char *demand)
	{
		init();
		readtopo(topo,edge_num);
		readdemand(demand);
#ifdef SHOW_DEBUG_INFO
		{
			for(int i=0;i==ACO_edge[i].num;i++)
				printf("....%d  %d  %d\n",ACO_edge[i].from,ACO_edge[i].to,ACO_edge[i].weight);
		}
#endif
		bool isna=SPFA(T);
#ifdef SHOW_DEBUG_INFO
		{
			printf("............SPFA/ok\n");
		}
#endif
		if(!isna) return;
		Edge_InformInit();
		for(int i=0;i<ANTNUM;i++) ant[i].Init(i,1);
		for(int i=0;i<ANTNUM;i++) rant[i].Init(i,2);
#ifdef SHOW_DEBUG_INFO
		{
			printf(".............AntInit/ok\n");
		}
#endif
		srand((int)time(NULL));
		for(int i=1;i<=tms;i++)
		{
			for(int j=0;j<ANTNUM;j++) ant[j].NextNode();
			for(int j=0;j<ANTNUM;j++) rant[j].NextNode();
			Edge_decrease();
#ifdef SHOW_DUBUG_INFO
			{
				printf(".............STEP %3d:answer=%4d\n",i,ACO_anspath.length);
			}
#endif
		}
		if(ACO_anspath.length==0) return;
		for(int i=0;i<(int)ACO_anspath.path.size()-1;i++)
			record_result(f[ACO_anspath.path[i]][ACO_anspath.path[i+1]]);
#ifdef SHOW_POINT
			{
				for(int i=0;i<(int)ACO_anspath.path.size();i++)
					printf("%d-->",ACO_anspath.path[i]);
				printf("%d\n",ACO_anspath.length);
			}
#endif
#ifdef SHOW_DEBUG_INFO
			{
				printf("answer is:%d\n",ACO_anspath.length);
			}
#endif
	}
};


//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{  
#ifdef SHOW_DEBUG_INFO
	{
		printf("............ACO starts");
	}
#endif
	ACO::solve(topo,edge_num,demand);
}

