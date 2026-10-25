#include <stdlib.h>
#include "neural_save.h"

/* Writes n values of array, per_line values per line.
   Returns 0 on success, -1 on error. */
static int write_array(FILE *f, const double *array, size_t n, size_t per_line);

/* Reads n values into array. Returns 0 if all n values were read, -1 otherwise. */
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
