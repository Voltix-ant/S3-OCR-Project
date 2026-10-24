#include "activation.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double x) {
    double sig = sigmoid(x);
    return sig*(1.0-sig);
}

void softmax(const double *input, double *output, size_t size) {
	
	if (size == 0) {
		return;
	}
	
	double max = input[0];
	double sum = 0.0;

	for (size_t i = 1; i < size; i++) {
		if (input[i] > max) {
			max = input[i];	
		}
	}
	
	for (size_t i = 0; i < size; i++) {
		output[i] = exp(input[i] - max);
		sum += output[i];
	}
	
	for (size_t i = 0; i < size; i++) {
		output[i] /= sum;
	}
}


