#include <stddef.h>

#ifndef ACTIVATION_H
#define ACTIVATION_H

/* Logistic sigmoid: returns 1 / (1 + e^-x), a value in (0, 1). */
double sigmoid(double x);

/* Derivative of the sigmoid at x: sigmoid(x) * (1 - sigmoid(x)). */
double sigmoid_derivative(double x);

/*
 * Softmax: turns the `size` values of `input` into probabilities that sum
 * to 1 and writes them into `output`. The maximum is subtracted before the
 * exponentials to avoid overflow. `input` and `output` may be the same array.
 */
void softmax(const double *input, double *output, size_t size);

#endif
