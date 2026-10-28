#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "dataset.h"
#include "gradient.h"
#include "loss.h"
#include <stddef.h>

/*
 * Multilayer perceptron with one hidden layer (sigmoid) and a softmax
 * output. Weights are stored row by row in 1D arrays:
 * hidden_weights[i * input_size + j] links input j to hidden neuron i,
 * output_weights[k * hidden_size + i] links hidden neuron i to output k.
 * `hidden` and `output` keep the activations of the last forward pass.
 */
typedef struct {
	size_t input_size;
	size_t hidden_size;
	size_t output_size;

	double *hidden_weights;
	double *hidden_bias;

	double *output_weights;
	double *output_bias;

	double *hidden;
	double *output;
} NeuralNetwork;

/* Allocates a zeroed gradient matching the sizes of `network`, or NULL. */
Gradient *gradient_create(const NeuralNetwork *network);
/* Frees a gradient. Does nothing if `gradient` is NULL. */
void gradient_destroy(Gradient *gradient);

/*
 * Creates a network with the given layer sizes. Weights are initialized
 * with Xavier uniform initialization and biases are set to 0.
 * Returns NULL if a size is invalid or an allocation fails.
 */
NeuralNetwork *neural_network_create(size_t input_size, size_t hidden_size,
				     size_t output_size);
/* Frees a network and all its arrays. Does nothing if it is NULL. */
void neural_network_destroy(NeuralNetwork *neural_network);

/*
 * Forward pass: computes the hidden activations and the output
 * probabilities for `input`, stored in network->hidden and network->output.
 */
void neural_network_forward(NeuralNetwork *network, const double *input);

/*
 * Backpropagation for one example. Must be called right after
 * neural_network_forward on the same input. Returns the gradient of the
 * cross-entropy loss (to free with gradient_destroy), or NULL on error.
 */
Gradient *neural_network_backward(NeuralNetwork *network, const double *input,
				  const double *expected);

/*
 * Trains the network with stochastic gradient descent: for each epoch,
 * every example is passed forward, backpropagated, and the weights are
 * updated with the given learning rate. Prints the mean loss per epoch.
 */
void neural_network_train(NeuralNetwork *network, Dataset *dataset,
			  size_t epochs, double learning_rate);

/* Runs the network on `input` and copies the probabilities into `output`. */
void neural_network_predict(NeuralNetwork *network, const double *input,
			    double *output);

/* Converts the 2 outputs of the XNOR network into its result, 0 or 1. */
int translate_output_to_xnor(double *output);

#endif
