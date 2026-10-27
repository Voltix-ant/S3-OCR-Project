#ifndef GRID_H
#define GRID_H

char *clean_line(char *line);
void free_grid(char **grid, int rows);
char **get_grid(const char *filename, int *rows, int *cols);
#endif // GRID_H
