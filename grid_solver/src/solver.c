#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

//This is the possible moves in the grid starting by north and going clockwise
static const int move_x[8] = {0,1,1,1,0,-1,-1,-1};
static const int move_y[8] = {-1,-1,0,1,1,1,0,-1};

//Test if the position is in the grid
static int in_grid(int x, int y, int rows, int cols){
      if (((x<0) || (x>=cols)) || ((y<0) || (y>=rows))){
            return 0;
      }
      return 1;
}

//Test if word is in grid starting at x,y going to dir
//Directions are 0=north, 1=north-east, 2=east, 
//3=south-east, 4=south, 5=south-west, 6=west, 7=north-west
static int match_direct(char **grid, int rows, int cols,const char *word,int x,
       int y, int dir){
      int len_word = (int)strlen(word);
      for (int i=0; i<len_word; i++){
            int check_x =x+ move_x[dir]*i;
            int check_y =y +move_y[dir]*i;
            if (!in_grid(check_x,check_y,rows,cols)){
                  return 0;}
            if (grid[check_y][check_x] !=word[i]){
                  return 0;}
            }
      return 1;
}


int solve(char **grid, int rows, int cols, const char *word,
      struct Position *start, struct Position *end){
      int len_word = (int)strlen(word);
      for (int x=0; x< cols; x++){
            for (int y=0; y<rows;y++){
                  if (grid[y][x] ==word[0]){
                        for (int dir=0;dir<8; dir++){
                              if (match_direct(grid,rows,cols,word,x,y,dir)){
                                    start->x=x;
                                    start->y =y;
                                    end->x =x+move_x[dir]* (len_word-1);
                                    end->y =y + move_y[dir]*(len_word-1);
                                    return 1;
                              }
                        }
                  }
            }
      }
    return 0;
}


