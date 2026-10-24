#include "loss.h"
#include <math.h>

double cross_entropy(const double *expected, const double *predicted, size_t size) {
	double loss = 0.0;
	const double epsilon = 1e-15;

	for (size_t i = 0; i < size; i++) {
		double predi = predicted[i];
		if (predi < epsilon) {
			predi = epsilon;
		}

		loss -= expected[i] * log(predi);
	}

	return loss;
}
