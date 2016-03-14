#include "route.h"

#include "force_search_solvor.h"
//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{   
    ForceSearchSolvor fs_solvor;
    fs_solvor.solve(topo, edge_num, demand);
}