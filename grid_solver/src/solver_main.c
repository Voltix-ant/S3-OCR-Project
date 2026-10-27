#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "solver.h"
#include "grid.h"

//copy the word in uppercase
static char *upper_word(const char *word) {
      size_t len = strlen(word);
      char *tmp = malloc(len + 1);
      if (!tmp)
            return NULL;
      for (size_t i = 0; i< len;i++) {
            tmp[i] = (char)toupper((unsigned char)word[i]);
      }
      tmp[len] = '\0';
      return tmp;
}

int main(int argc, char *argv[]) {
      if (argc != 3) {
            fprintf(stderr, "Usage: %s <grid_file> <word>\n", argv[0]);
            return 1;
      }
      int rows =0;
      int cols =0;
      char **grid = get_grid(argv[1], &rows, &cols);
      if (!grid) {
            fprintf(stderr, "Error reading grid from file: %s\n", argv[1]);
            return 1;
      }
      char *word = upper_word(argv[2]);
      if (!word) {
            free_grid(grid, rows);
            return 1;}
      struct Position start;
      struct Position end;
      if (solve(grid, rows, cols, word, &start, &end)) {
            printf("(%d,%d)(%d,%d)\n", start.x, start.y, end.x, end.y);
      } else {
            printf("Not found\n");
      }
      free(word);
      free_grid(grid, rows);
      return 0;
}
