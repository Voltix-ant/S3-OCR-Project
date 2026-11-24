#include "activation.h"
#include "dataset.h"
#include "neural_network.h"
#include "neural_save.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Random weight in [-sqrt(6 / (n_in + n_out)), +sqrt(6 / (n_in + n_out))]. */
static double xavier_uniform(int n_in, int n_out) {
	double limit = sqrt(6.0 / (n_in + n_out));

	double u = (double)rand() / (double)RAND_MAX;
	return -limit + 2.0 * limit * u;
}

NeuralNetwork *neural_network_create(size_t input_size, size_t hidden_size,
				     size_t output_size) {
	if (input_size < 2 || hidden_size < 1 || output_size < 1) {
		return NULL;
	}
	NeuralNetwork *neural_network = calloc(1, sizeof(NeuralNetwork));
	if (!neural_network) {
		return NULL;
	}
	neural_network->input_size = input_size;
	neural_network->hidden_size = hidden_size;
	neural_network->output_size = output_size;

	neural_network->hidden_weights =
	    calloc(hidden_size * input_size, sizeof(double));
	if (!neural_network->hidden_weights) {
		free(neural_network);
		return NULL;
	}
	for (size_t i = 0; i < hidden_size * input_size; i++) {
		neural_network->hidden_weights[i] =
		    xavier_uniform(input_size, hidden_size);
	}

	neural_network->hidden_bias = calloc(hidden_size, sizeof(double));
	if (!neural_network->hidden_bias) {
		free(neural_network->hidden_weights);
		free(neural_network);
		return NULL;
	}
	neural_network->output_weights =
	    calloc(hidden_size * output_size, sizeof(double));
	if (!neural_network->output_weights) {
		free(neural_network->hidden_weights);
		free(neural_network->hidden_bias);
		free(neural_network);
		return NULL;
	}
	for (size_t i = 0; i < hidden_size * output_size; i++) {
		neural_network->output_weights[i] =
		    xavier_uniform(hidden_size, output_size);
	}

	neural_network->output_bias = calloc(output_size, sizeof(double));
	if (!neural_network->output_bias) {
		free(neural_network->hidden_weights);
		free(neural_network->hidden_bias);
		free(neural_network->output_weights);
		free(neural_network);
		return NULL;
	}
	neural_network->hidden = calloc(hidden_size, sizeof(double));
	if (!neural_network->hidden) {
		free(neural_network->hidden_weights);
		free(neural_network->hidden_bias);
		free(neural_network->output_weights);
		free(neural_network->output_bias);
		free(neural_network);
		return NULL;
	}
	neural_network->output = calloc(output_size, sizeof(double));
	if (!neural_network->output) {
		free(neural_network->hidden_weights);
		free(neural_network->hidden_bias);
		free(neural_network->output_weights);
		free(neural_network->output_bias);
		free(neural_network->hidden);
		free(neural_network);
		return NULL;
	}

	return neural_network;
}

void neural_network_destroy(NeuralNetwork *neural_network) {
	if (!neural_network) {
		return;
	}
	free(neural_network->hidden_weights);
	free(neural_network->hidden_bias);
	free(neural_network->output_weights);
	free(neural_network->output_bias);
	free(neural_network->hidden);
	free(neural_network->output);
	free(neural_network);
}

void neural_network_forward(NeuralNetwork *network, const double *input) {
	size_t input_size = network->input_size;
	size_t hidden_size = network->hidden_size;
	size_t output_size = network->output_size;

	for (size_t i = 0; i < hidden_size; i++) {
		double sum = network->hidden_bias[i];

		for (size_t j = 0; j < input_size; j++) {
			double weight =
			    network->hidden_weights[i * input_size + j];

			sum += weight * input[j];
		}

		network->hidden[i] = sigmoid(sum);
	}

	for (size_t k = 0; k < output_size; k++) {
		double sum = network->output_bias[k];

		for (size_t i = 0; i < hidden_size; i++) {
			double weight =
			    network->output_weights[k * hidden_size + i];

			sum += weight * network->hidden[i];
		}

		network->output[k] = sum;
	}

	softmax(network->output, network->output, network->output_size);
}

void neural_network_backward_add(const NeuralNetwork *network,
				  const double *input, const double *expected,
				  Gradient *gradient) {
	size_t input_size = network->input_size;
	size_t hidden_size = network->hidden_size;
	size_t output_size = network->output_size;

	// output layer: delta_k = output_k - expected_k (softmax + entropy)
	for (size_t k = 0; k < output_size; k++) {
		double delta = network->output[k] - expected[k];

		gradient->output_bias[k] += delta;
		for (size_t i = 0; i < hidden_size; i++) {
			gradient->output_weights[k * hidden_size + i] +=
			    delta * network->hidden[i];
		}
	}

	// hidden layer: error back from the outputs, times the sigmoid slope
	for (size_t i = 0; i < hidden_size; i++) {
		double error = 0.0;

		for (size_t k = 0; k < output_size; k++) {
			error += (network->output[k] - expected[k]) *
				 network->output_weights[k * hidden_size + i];
		}
		double delta =
		    error * network->hidden[i] * (1.0 - network->hidden[i]);

		gradient->hidden_bias[i] += delta;
		for (size_t j = 0; j < input_size; j++) {
			gradient->hidden_weights[i * input_size + j] +=
			    delta * input[j];
		}
	}
}

Gradient *neural_network_backward(NeuralNetwork *network, const double *input,
				  const double *expected) {
	Gradient *gradient = gradient_create(network);
	if (!gradient) {
		return NULL;
	}
	neural_network_backward_add(network, input, expected, gradient);
	return gradient;
}

void neural_network_gradient_descent(NeuralNetwork *network,
				     const Gradient *gradient,
				     double learning_rate) {
	for (size_t i = 0; i < network->hidden_size * network->input_size;
	     i++) {
		network->hidden_weights[i] -=
		    learning_rate * gradient->hidden_weights[i];
	}

	for (size_t i = 0; i < network->hidden_size; i++) {
		network->hidden_bias[i] -=
		    learning_rate * gradient->hidden_bias[i];
	}

	for (size_t i = 0; i < network->output_size * network->hidden_size;
	     i++) {
		network->output_weights[i] -=
		    learning_rate * gradient->output_weights[i];
	}

	for (size_t i = 0; i < network->output_size; i++) {
		network->output_bias[i] -=
		    learning_rate * gradient->output_bias[i];
	}
}

/* Shuffles the `size` indices of `order` (Fisher-Yates). */
static void shuffle(size_t *order, size_t size) {
	for (size_t i = size - 1; i > 0; i--) {
		size_t j = (size_t)rand() % (i + 1);
		size_t tmp = order[i];

		order[i] = order[j];
		order[j] = tmp;
	}
}

void neural_network_train(NeuralNetwork *network, Dataset *dataset,
			  size_t epochs, double learning_rate,
			  size_t batch_size) {
	if (!network || !dataset || dataset->size == 0 || epochs == 0 ||
	    batch_size == 0) {
		return;
	}

	size_t size = dataset->size;
	size_t *order = malloc(size * sizeof(size_t));
	Gradient *gradient = gradient_create(network);
	if (!order || !gradient) {
		free(order);
		gradient_destroy(gradient);
		return;
	}
	for (size_t i = 0; i < size; i++) {
		order[i] = i;
	}

	// print the loss about 4 times, at least once per epoch if epochs < 4
	size_t log_step = epochs / 4 > 0 ? epochs / 4 : 1;

	for (size_t epoch = 0; epoch < epochs; epoch++) {
		double total_loss = 0.0;

		// the dataset itself is never reordered (see dataset_free)
		shuffle(order, size);

		for (size_t start = 0; start < size; start += batch_size) {
			size_t count = size - start < batch_size
					   ? size - start
					   : batch_size;

			gradient_reset(gradient, network);
			for (size_t i = start; i < start + count; i++) {
				TrainingExample *example =
				    &dataset->examples[order[i]];

				neural_network_forward(network,
						       example->input);
				total_loss += cross_entropy(
				    example->expected, network->output,
				    network->output_size);
				neural_network_backward_add(
				    network, example->input,
				    example->expected, gradient);
			}

			// one update per batch, with the mean gradient
			neural_network_gradient_descent(
			    network, gradient, learning_rate / (double)count);
		}

		if (epoch % log_step == 0 || epoch == epochs - 1) {
			printf("Epoch %zu/%zu - loss: %.6f\n", epoch + 1,
			       epochs, total_loss / (double)size);
		}
	}

	free(order);
	gradient_destroy(gradient);
}

void neural_network_predict(NeuralNetwork *network, const double *input,
			    double *output) {
	neural_network_forward(network, input);
	for (size_t i = 0; i < network->output_size; i++) {
		output[i] = network->output[i];
	}
}

int translate_output_to_xnor(double *output) {
	if (output[0] >= 0.5) {
		return 0;
	} else {
		return 1;
	}
}
