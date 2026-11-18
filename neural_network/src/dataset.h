#ifndef DATASET_H
#define DATASET_H

#include <stddef.h>

/* Defined in analysis/src/analysis.h, only used through a pointer here. */
struct analysis;

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

Dataset *dataset_from_grid(const struct analysis *analysis,
			   const char *labels, size_t rows, size_t cols);

Dataset *dataset_concat(Dataset **datasets, size_t count);

void dataset_free(Dataset *dataset);

#endif
