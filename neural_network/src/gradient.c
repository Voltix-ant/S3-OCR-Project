#include "gradient.h"
#include <stdlib.h>

Gradient *gradient_create(const NeuralNetwork *network) {
        Gradient *gradient = malloc(sizeof(Gradient));
        if (!gradient) {
                return NULL;
        }

        if (!gradient) {
                return NULL;
        }
        gradient->hidden_weights = calloc(network->hidden_size * network->input_size, sizeof(double));
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

        gradient->output_weights = calloc(network->output_size * network->hidden_size, sizeof(double));
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
