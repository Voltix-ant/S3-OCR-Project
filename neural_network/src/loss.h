#ifndef LOSS_H
#define LOSS_H

#include <stddef.h>

/*
 * Cross-entropy between the expected one-hot vector and the predicted
 * probabilities: -sum(expected[i] * log(predicted[i])). Predictions are
 * clamped to 1e-15 to avoid log(0).
 */
double cross_entropy(const double *expected, const double *predicted,
		     size_t size);

#endif
