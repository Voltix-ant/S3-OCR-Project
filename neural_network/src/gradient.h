#ifndef GRADIENT_H
#define GRADIENT_H

/*
 * Gradient of the loss for every parameter of a network, with the same
 * layout as the NeuralNetwork arrays.
 */
typedef struct {
    double *hidden_weights;
    double *hidden_bias;
    double *output_weights;
    double *output_bias;
} Gradient;
#include "neural_network.h"


#endif
