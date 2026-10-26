#ifndef NEURAL_SAVE_H
#define NEURAL_SAVE_H

#include "neural_network.h"

int neural_network_save(const NeuralNetwork *network, const char *path);
NeuralNetwork *neural_network_load(const char *path);

#endif
