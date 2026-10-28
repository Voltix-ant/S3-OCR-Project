#ifndef NEURAL_SAVE_H
#define NEURAL_SAVE_H

#include "neural_network.h"

/*
 * Saves a network to a text file: a "NN 1" version header, the three layer
 * sizes, then the hidden weights, hidden biases, output weights and output
 * biases, written with 17 significant digits so that loading is exact.
 * Returns 0 on success, -1 on error.
 */
int neural_network_save(const NeuralNetwork *network, const char *path);

/*
 * Loads a network saved by neural_network_save. Returns NULL if the file
 * cannot be opened, has a wrong header or version, or is incomplete.
 */
NeuralNetwork *neural_network_load(const char *path);

#endif
