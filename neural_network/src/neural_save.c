#include <stdlib.h>
#include <stdio.h>
#include "neural_save.h"

static int write_array(FILE *f, const double *array, size_t n, size_t per_line) {
	if (f == NULL || per_line == 0) {
		return -1;
	}
	int res = 0;
	for (size_t i = 0; i < n; i++) {
		if (i % per_line == per_line-1) {
			res = fprintf(f, "%.17g\n", array[i]);
		} else {
			res = fprintf(f, "%.17g ", array[i]);	
		}
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}


static int read_array(FILE *f, double *array, size_t n) {
	(void)f;
	(void)array;
	(void)n;
}

int neural_network_save(const NeuralNetwork *network, const char *path) {
	if (network == NULL || path == NULL) {
		return -1;
	}
	FILE *file = fopen(path, "w");
	if (file == NULL) {
		return -1;
	}
	int res = fprintf(file, "NN 1\n");
	if (res < 0) {
		fclose(file);
		return -1;
	}
	res = fprintf(file, "%zu %zu %zu\n", network->input_size, network->hidden_size, network->output_size);
	if (res < 0) {
		fclose(file);
		return -1;
	}
	
	// we add the data saved
	// hidden weights
	res = write_array(file, network->hidden_weights, network->hidden_size*network->input_size, network->input_size);
	if (res) {
		fclose(file);
		return -1;
	}
	// hidden bias
	res = write_array(file, network->hidden_bias, network->hidden_size, network->hidden_size);
	if (res) {
		fclose(file);
		return -1;
	}
	
	// output weights
	res = write_array(file, network->output_weights, network->hidden_size*network->output_size, network->hidden_size);
	if (res) {
		fclose(file);
		return -1;
	}
	
	// output bias
	res = write_array(file, network->output_bias, network->output_size, network->output_size);
	if (res) {
		fclose(file);
		return -1;
	}


	fclose(file);
	return 0;
}

NeuralNetwork *neural_network_load(const char *path) {
	(void)path;
}
