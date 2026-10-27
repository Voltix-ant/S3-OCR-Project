#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"


//Delete \n or \r at the end of the line
char *clean_line(char *line) { 
    if (line == NULL) 
        return NULL;
    size_t len = strlen(line);
    while(len >0 && (line[len-1] == '\n' || line[len-1] =='\r')){
        line[len- 1]= '\0';
        len--;
    }
    return line;
}

//Free the grid
void free_grid(char **grid, int rows) {
    for (int i = 0; i< rows; i++) {
        free(grid[i]);
    }
    free(grid);
}

//get the grid from a file and return it as a array of arrays
char **get_grid(const char *filename, int *rows, int *cols) {
   FILE *file = fopen(filename, "r");
   if (!file) { 
      return NULL;
   }
   char *line = NULL;
   size_t buffer_line = 0;
   *rows=0;
   *cols=0;
   char **grid = NULL;
   while ((getline(&line, &buffer_line, file))>0) {
      line = clean_line(line);
      size_t len_line = strlen(line);
      if (len_line >0){
        if (*rows >0 && (int)len_line != *cols) {
            free(line);
            fclose(file);
            free_grid(grid, *rows);
            return NULL;
        }
        *cols = (int)len_line;
        char **tmp = realloc(grid, sizeof(char *)*(size_t)(*rows + 1));
        if (!tmp) {
            free(line);
            fclose(file);
            free_grid(grid, *rows);
            return NULL;
        }
        grid = tmp;
        grid[*rows] = malloc(sizeof(char) * (len_line + 1));
        if (!(grid[*rows])) {
            free(line);
            fclose(file);
            free_grid(grid, *rows);
            return NULL;
        }
        strcpy(grid[*rows], line);
        (*rows)++;
      }
    }
    free(line);
    fclose(file);
    return grid;
}
