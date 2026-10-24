#ifndef GRADIENT_H
#define GRADIENT_H


typedef struct {
    double *hidden_weights;
    double *hidden_bias;
    double *output_weights;
    double *output_bias;
} Gradient;
#include "neural_network.h"


#endif
