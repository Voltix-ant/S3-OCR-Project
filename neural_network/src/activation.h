#include <stddef.h>

#ifndef ACTIVATION_H
#define ACTIVATION_H


double sigmoid(double x);
double sigmoid_derivative(double x);

void softmax(const double *input, double *output, size_t size);

#endif
