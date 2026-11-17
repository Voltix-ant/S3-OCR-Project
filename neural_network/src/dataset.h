#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>

typedef struct
{
    double *input;
    double *expected;
} TrainingExample;

typedef struct
{
    TrainingExample *examples;
    size_t size;
    size_t input_size;
    size_t output_size;
} Dataset;

char *labels_read_grid(const char *path, size_t *rows, size_t *cols);

#endif
