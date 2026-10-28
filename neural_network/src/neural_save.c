#include "neural_save.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Writes the `n` values of `array` to `f` with 17 significant digits,
 * `per_line` values per line. Returns 0 on success, -1 on error.
 */
static int write_array(FILE *f, const double *array, size_t n,
		       size_t per_line) {
	if (f == NULL || per_line == 0) {
		return -1;
	}
	int res = 0;
	for (size_t i = 0; i < n; i++) {
		if (i % per_line == per_line - 1) {
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

/*
 * Reads `n` values from `f` into `array`.
 * Returns 0 on success, -1 if a value is missing or invalid.
 */
static int read_array(FILE *f, double *array, size_t n) {
	if (f == NULL || array == NULL) {
		return -1;
	}
	int res;
	for (size_t i = 0; i < n; i++) {
		res = fscanf(f, "%lf", &array[i]);
		if (res != 1) {
			return -1;
		}
	}
	return 0;
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
	res = fprintf(file, "%zu %zu %zu\n", network->input_size,
		      network->hidden_size, network->output_size);
	if (res < 0) {
		fclose(file);
		return -1;
	}

	// we add the data saved
	// hidden weights
	res = write_array(file, network->hidden_weights,
			  network->hidden_size * network->input_size,
			  network->input_size);
	if (res) {
		fclose(file);
		return -1;
	}
	// hidden bias
	res = write_array(file, network->hidden_bias, network->hidden_size,
			  network->hidden_size);
	if (res) {
		fclose(file);
		return -1;
	}

	// output weights
	res = write_array(file, network->output_weights,
			  network->hidden_size * network->output_size,
			  network->hidden_size);
	if (res) {
		fclose(file);
		return -1;
	}

	// output bias
	res = write_array(file, network->output_bias, network->output_size,
			  network->output_size);
	if (res) {
		fclose(file);
		return -1;
	}

	fclose(file);
	return 0;
}

NeuralNetwork *neural_network_load(const char *path) {
	// verification of the file
	if (path == NULL) {
		return NULL;
	}
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		return NULL;
	}
	int version;
	int res = fscanf(file, "NN %d", &version);
	if (res != 1 || version != 1) {
		fclose(file);
		return NULL;
	}

	// reading the sizes and creating the network
	size_t input_size;
	size_t hidden_size;
	size_t output_size;

	res = fscanf(file, "%zu %zu %zu", &input_size, &hidden_size,
		     &output_size);
	if (res != 3) {
		fclose(file);
		return NULL;
	}

	NeuralNetwork *network =
	    neural_network_create(input_size, hidden_size, output_size);
	if (network == NULL) {
		fclose(file);
		return NULL;
	}

	// we can read the data and load the settings
	// hidden weights
	res =
	    read_array(file, network->hidden_weights, input_size * hidden_size);
	if (res == -1) {
		neural_network_destroy(network);
		fclose(file);
		return NULL;
	}
	// hidden bias
	res = read_array(file, network->hidden_bias, hidden_size);
	if (res == -1) {
		neural_network_destroy(network);
		fclose(file);
		return NULL;
	}
	// output weights
	res = read_array(file, network->output_weights,
			 hidden_size * output_size);
	if (res == -1) {
		neural_network_destroy(network);
		fclose(file);
		return NULL;
	}

	// output bias
	res = read_array(file, network->output_bias, output_size);
	if (res == -1) {
		neural_network_destroy(network);
		fclose(file);
		return NULL;
	}

	fclose(file);
	return network;
}
