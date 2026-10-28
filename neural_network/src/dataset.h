#ifndef DATASET_H
#define DATASET_H

/* One training example: an input vector and its expected one-hot output. */
typedef struct
{
    double *input;
    double *expected;
} TrainingExample;

/* A set of `size` examples sharing the same input and output sizes. */
typedef struct
{
    TrainingExample *examples;
    size_t size;
    size_t input_size;
    size_t output_size;
} Dataset;

#endif
