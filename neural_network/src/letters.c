#include "letters.h"
#include <ctype.h>
#include <string.h>

NeuralNetwork *letter_network_create(void) {
	return neural_network_create(LETTER_INPUT_SIZE, LETTER_HIDDEN_SIZE,
				     LETTER_OUTPUT_SIZE);
}

int letter_matrix_to_input(const struct matrix *image, double *input) {
	if (!image || image->width != LETTER_SIDE ||
	    image->height != LETTER_SIDE) {
		return -1;
	}

	for (size_t y = 0; y < LETTER_SIDE; y++) {
		for (size_t x = 0; x < LETTER_SIDE; x++) {
			input[y * LETTER_SIDE + x] =
			    1.0 - image->data[y][x] / 255.0;
		}
	}
	return 0;
}

int letter_to_expected(char letter, double *expected) {
	if (!isalpha((unsigned char)letter)) {
		return -1;
	}
	int index = toupper((unsigned char)letter) - 'A';

	for (int k = 0; k < LETTER_OUTPUT_SIZE; k++) {
		expected[k] = k == index ? 1.0 : 0.0;
	}
	return 0;
}

/* Index of the highest of the LETTER_OUTPUT_SIZE outputs. */
static int best_output(const double *output) {
	int best = 0;

	for (int k = 1; k < LETTER_OUTPUT_SIZE; k++) {
		if (output[k] > output[best]) {
			best = k;
		}
	}
	return best;
}

char letter_from_output(const double *output) {
	return (char)('A' + best_output(output));
}

char letter_recognize(NeuralNetwork *network, const struct matrix *image,
		      double *confidence) {
	double input[LETTER_INPUT_SIZE];

	if (!network || letter_matrix_to_input(image, input) == -1) {
		return '?';
	}

	neural_network_forward(network, input);
	int best = best_output(network->output);

	if (confidence) {
		*confidence = network->output[best];
	}
	return (char)('A' + best);
}

double letter_evaluate(NeuralNetwork *network, const Dataset *dataset,
		       size_t correct[LETTER_OUTPUT_SIZE],
		       size_t total[LETTER_OUTPUT_SIZE]) {
	if (!network || !dataset || dataset->size == 0) {
		return 0.0;
	}

	size_t good[LETTER_OUTPUT_SIZE] = {0};
	size_t count[LETTER_OUTPUT_SIZE] = {0};
	size_t recognized = 0;

	for (size_t i = 0; i < dataset->size; i++) {
		const TrainingExample *example = &dataset->examples[i];

		neural_network_forward(network, example->input);
		char expected = letter_from_output(example->expected);
		char found = letter_from_output(network->output);

		count[expected - 'A']++;
		if (found == expected) {
			good[expected - 'A']++;
			recognized++;
		}
	}

	if (correct) {
		memcpy(correct, good, sizeof(good));
	}
	if (total) {
		memcpy(total, count, sizeof(count));
	}
	return (double)recognized / (double)dataset->size;
}
