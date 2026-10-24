#ifndef LOSS_H
#define LOSS_H

#include <stddef.h>

double cross_entropy(const double *expected, const double *predicted, size_t size);

#endif
