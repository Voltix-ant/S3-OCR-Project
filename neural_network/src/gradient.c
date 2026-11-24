#include "gradient.h"
#include <stdlib.h>
#include <string.h>

Gradient *gradient_create(const NeuralNetwork *network) {
	Gradient *gradient = malloc(sizeof(Gradient));
	if (!gradient) {
		return NULL;
	}

	gradient->hidden_weights =
	    calloc(network->hidden_size * network->input_size, sizeof(double));
	if (!gradient->hidden_weights) {
		free(gradient);
		return NULL;
	}

	gradient->hidden_bias = calloc(network->hidden_size, sizeof(double));
	if (!gradient->hidden_bias) {
		free(gradient->hidden_weights);
		free(gradient);
		return NULL;
	}

	gradient->output_weights =
	    calloc(network->output_size * network->hidden_size, sizeof(double));
	if (!gradient->output_weights) {
		free(gradient->hidden_weights);
		free(gradient->hidden_bias);
		free(gradient);
		return NULL;
	}

	gradient->output_bias = calloc(network->output_size, sizeof(double));
	if (!gradient->output_bias) {
		free(gradient->hidden_weights);
		free(gradient->hidden_bias);
		free(gradient->output_weights);
		free(gradient);
		return NULL;
	}

	return gradient;
}

void gradient_destroy(Gradient *gradient) {
	if (!gradient) {
		return;
	}

	free(gradient->hidden_weights);
	free(gradient->hidden_bias);
	free(gradient->output_weights);
	free(gradient->output_bias);
	free(gradient);
}

void gradient_reset(Gradient *gradient, const NeuralNetwork *network) {
	size_t hidden = network->hidden_size;

	memset(gradient->hidden_weights, 0,
	       hidden * network->input_size * sizeof(double));
	memset(gradient->hidden_bias, 0, hidden * sizeof(double));
	memset(gradient->output_weights, 0,
	       network->output_size * hidden * sizeof(double));
	memset(gradient->output_bias, 0, network->output_size * sizeof(double));
}
