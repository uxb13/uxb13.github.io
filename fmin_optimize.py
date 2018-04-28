import board_2048
import copy
import time
from multiprocessing import Process
import sys
import sqlite3
import numpy as np
from scipy import optimize

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


def opt(params):

    score = 0.0
    for i in range(0,200):
        ret1,ret2,ret3 = play(params)
        score += ret3

    score /= 200

    print score,params
    return -score

init_guess = (200.0, 0.25, 300.0, 0.35)
#init_guess = (50.0, 0.15, 150.0, 0.1)
#init_guess = (7.49749240e+02,6.19706169e-01,9.00083726e+02,9.28155219e-01)

minimum = optimize.fmin(opt,init_guess,xtol=30,ftol=3,full_output=True,disp=True)  
