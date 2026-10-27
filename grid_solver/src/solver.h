#ifndef SOLVER_H
#define SOLVER_H

struct Position {
      int x;
      int y;
};

int solve(char **grid, int rows, int cols, const char *word,
      struct Position *start, struct Position *end);

#endif
