# -*- coding: utf-8 -*-

import random

def rand_int(a, b):
    '''
    Return the integer From [a,b]
    '''
    return random.randint(a, b)


conf = dict(file_graph = 'topo.csv', file_solve = 'demand.csv', point_num = 20, edge_num = 80, point_v_num = 10)

if __name__ == '__main__':

    file_graph = conf['file_graph']
    file_solve = conf['file_solve']
    # 所有点的个数 [0, point_num-1]
    point_num  = conf['point_num']
    # 所有边的个数 [0, edge_num-1]
    edge_num   = conf['edge_num']
    # V'中点的个数
    point_v_num = conf['point_v_num']

    # 生成图数据
    file_handle = open(file_graph, 'w')
    for i in range(0, edge_num):
        link_id = i
        source_id = rand_int(0, point_num - 1)
        dest_id = rand_int(0, point_num - 1)
        while dest_id == source_id:
            dest_id = rand_int(0, point_num - 1)
        cost = rand_int(1, 20)
        write_str = str(link_id) + ',' + str(source_id) + ',' + str(dest_id) + ',' + str(cost) + '\n'
        file_handle.write(write_str)
    file_handle.close()

    # 生成计算数据
    file_handle = open(file_solve, 'w')
    start_id = rand_int(0, point_num - 1)
    end_id = rand_int(0, point_num - 1)
    while start_id == end_id:
        end_id = rand_int(0, point_num - 1)
    including_set = []
    while len(including_set) <= point_v_num:
        tmp = rand_int(0, point_num - 1)
        if tmp not in including_set:
            including_set.append(tmp)
    write_str = str(start_id) + ',' + str(end_id) + ','
    First = True
    for i in including_set:
        if First:
            write_str += str(i)
            First = False
        else:
            write_str += '|'+str(i)
    write_str += '\n'
    file_handle.write(write_str)
    file_handle.close()