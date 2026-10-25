#include <stdlib.h>
#include <stdio.h>
#include "neural_save.h"

static int write_array(FILE *f, const double *array, size_t n, size_t per_line) {
	if (file == NULL || per_line == 0) {
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
	
}

NeuralNetwork *neural_network_load(const char *path) {
	(void)path;
}
