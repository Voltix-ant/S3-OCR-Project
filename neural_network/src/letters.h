#ifndef LETTERS_H
#define LETTERS_H

#include "matrix.h"
#include "neural_network.h"

#define LETTER_SIDE 28
#define LETTER_INPUT_SIZE (LETTER_SIDE * LETTER_SIDE)
#define LETTER_HIDDEN_SIZE 300
#define LETTER_OUTPUT_SIZE 26

NeuralNetwork *letter_network_create(void);

int letter_matrix_to_input(const struct matrix *image, double *input);

int letter_to_expected(char letter, double *expected);

char letter_from_output(const double *output);

char letter_recognize(NeuralNetwork *network, const struct matrix *image,
		      double *confidence);

#endif
