import board_2048
import copy
import time
from multiprocessing import Process
import sys
import sqlite3
import numpy as np

def evaluate2(state,params):
    

    num_empty_cells_ = params[0]
    highest_num_ = params[1]
    num_movable_ = params[2]
    mul_ = params[3]

    num_empty_cells = len(board_2048.get_available_cells(state))
    num_movable = len(board_2048.get_movable_directions(state))
    highest_num = state[0][0]
    for i in state:
        for j in i:
            if j > highest_num:
                highest_num = 1.0*j

    mul = [[64.0,32.0,16.0,8.0],[0.5,1.0,2.0,4.0],[0.25,0.125,0.0625,0.03125],[0.001953125,0.00390625,0.0078125,0.015625]]

    temp = 0.0

    for i in range(0,4):
        for j in range(0,4):
            temp += mul[i][j]*state[i][j]    

    ret = num_empty_cells*num_empty_cells_ + highest_num*highest_num_ + num_movable*num_movable_ + temp*mul_
    return ret


def max_val(state,depth,params):
    if depth<=0:
        return evaluate2(state,params)

    v = float('-inf')
    moves = board_2048.get_movable_directions(state)
    for i in moves:
        copy_state = copy.deepcopy(state)
        board_2048.move(copy_state,i,False)
        v = max(v,min_val(copy_state,depth-1,params))
    return v


def min_val(state,depth,params):
    if depth<=0:
        return evaluate2(state,params)

    v = float('inf')
    check = board_2048.insert_new_item(state)
    if check:
        v = min(v,max_val(state,depth-1,params))
    return v


def min_max(state,depth,params):
    moves = board_2048.get_movable_directions(state)

    best_move = moves[0]
    score = float('-inf')
    
    for i in moves:
        copy_state = copy.deepcopy(state)
        board_2048.move(copy_state,i,False)
        temp_min = min_val(copy_state,depth-1,params)
        if temp_min>score:
            score = 1.0*temp_min
            best_move = i
    return best_move


def play(params):

    grid = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]
    board_2048.insert_new_item(grid)
    board_2048.insert_new_item(grid)

    no_of_moves = 0
    
    while True:
        #board_2048.print_grid(grid)
        #print "........................ ",no_of_moves
        if not board_2048.get_movable_directions(grid):
            break        
        
        move2 = min_max(grid,1,params)
        board_2048.move(grid,move2,False)
        board_2048.insert_new_item(grid)

        no_of_moves += 1

    flat_grid = [k for j in grid for k in j]

    tile_2048 = 0
    tile_1024 = 0

    for i in flat_grid:
        if i>=2047:
            tile_2048 = 1.0
            tile_1024 = 1.0
            break

    if tile_2048==0:
        for j in flat_grid:
            if j>=1023:
                tile_1024 = 1.0
                tile_2048 = 0.0
                break

    return [tile_2048,tile_1024,no_of_moves]
            

def range1(r):
    search_range = r[0]
    db_name = str(r[1]) + '.db'
    conn = sqlite3.connect(db_name)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS scores
             (tile_2048 text, tile_1024 text, moves text, param1 text, param2 text, param3 text, param4 text)''')

    INSERT_COMMAND = "INSERT INTO scores VALUES(?,?,?,?,?,?,?)"

    r1 = np.arange(search_range[0],search_range[0]+200,50)
    r2 = np.arange(search_range[1],search_range[1]+0.2,0.05)
    r3 = np.arange(search_range[2],search_range[2]+200,50)
    r4 = np.arange(search_range[3],search_range[3]+0.2,0.05)
    
    count = 0

    start = time.time()
    print "started ", db_name

    max_score1 = float('-inf')
    max_score2 = float('-inf')
    
    for i in r1:
        for j in r2:
            for k in r3:
                for l in r4:
                    score1 = 0
                    score2 = 0
                    score3 = 0
                    for m in range(0,100):
                        ret1,ret2,ret3 = play((i,j,k,l))
                        score1 += ret1
                        score2 += ret2
                        score3 += ret3
                    count += 1

                    c.execute(INSERT_COMMAND,(score1,score2,score3,i,j,k,l))

                    if score1>max_score1:
                        max_score1 = 1.0*score1

                    if score2>max_score2:
                        max_score2 = 1.0*score2

                    if count>10:
                        count = 0
                        conn.commit()
                        print (i,j,k),(score1,score2,score3),db_name,max_score1,max_score2

'''
range_list = [(0,0,0,0),(200,0.20,200,0.20),(400,0.4,400,0.4),(600,0.6,600,0.6),(800,0.8,800,0.80)]

process_array = []

for i in range(0,5):
    process_array.append(Process(target=range1,args=[(range_list[i],i)]))

for j in process_array:
    j.start()

'''
