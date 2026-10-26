#include "neural_network.h"
#include "neural_save.h"
#include <stdio.h>
#include <stdlib.h>

#define SAVE_PATH "neural_network/saves/xor.txt"

int main(void) {
	srand(42);

	const size_t input_size = 2;
	const size_t hidden_size = 4;
	const size_t output_size = 2;

	double input_0[] = {1.0, 0.0};
	double input_1[] = {0.0, 0.0};
	double input_2[] = {0.0, 1.0};
	double input_3[] = {1.0, 1.0};

	double expected_0[] = {0.0, 1.0};
	double expected_1[] = {1.0, 0.0};
	double expected_2[] = {0.0, 1.0};
	double expected_3[] = {1.0, 0.0};

	TrainingExample examples[] = {
		{input_0, expected_0},
		{input_1, expected_1},
		{input_2, expected_2},
		{input_3, expected_3},
	
	};

	Dataset dataset = {
		.examples = examples,
		.size = 4,
		.input_size = input_size,
		.output_size = output_size
	};
	
	NeuralNetwork *network = neural_network_create(input_size, hidden_size, output_size);

	if (!network) {
		printf("Error creating the network");
		return EXIT_FAILURE;
	}

	printf("Starting the training\n");

	neural_network_train(network, &dataset, 2000, 0.1);
	
	// to save the training
	/*
	int res = neural_network_save(network, SAVE_PATH);
	if (res == -1) {
		printf("error while saving\n");
	} else {
		printf("settings saved at : %s\n", SAVE_PATH);
	}
	*/

	// to load the training
	NeuralNetwork *network2 = neural_network_load(SAVE_PATH);
	if (network2 == NULL) {
		printf("Error while loading the network from : %s\n", SAVE_PATH);
	}


	printf("\nPrediction after training:\n");
	double output[2] = {0.0, 0.0};

	neural_network_predict(network, input_0, output);
	printf("%.1f xor %.1f = %d\n", input_0[0], input_0[1], translate_output_to_xor(output));

	neural_network_predict(network, input_1, output);
	printf("%.1f xor %.1f = %d\n", input_1[0], input_1[1], translate_output_to_xor(output));

	neural_network_predict(network, input_2, output);
	printf("%.1f xor %.1f = %d\n", input_2[0], input_2[1], translate_output_to_xor(output));

	neural_network_predict(network, input_3, output);
	printf("%.1f xor %.1f = %d \n", input_3[0], input_3[1], translate_output_to_xor(output));
	
	if (network2) {
		printf("\nPrediction after loading:\n");
		neural_network_predict(network2, input_0, output);
		printf("%.1f xor %.1f = %d\n", input_0[0], input_0[1], translate_output_to_xor(output));

		neural_network_predict(network2, input_1, output);
		printf("%.1f xor %.1f = %d\n", input_1[0], input_1[1], translate_output_to_xor(output));

		neural_network_predict(network2, input_2, output);
		printf("%.1f xor %.1f = %d\n", input_2[0], input_2[1], translate_output_to_xor(output));

		neural_network_predict(network2, input_3, output);
		printf("%.1f xor %.1f = %d \n", input_3[0], input_3[1], translate_output_to_xor(output));
		
		printf("\nRaw Comparison\n");
		printf("Network 1: %.17f %.17f\n", network->output[0], network->output[1]);
		printf("Network 2: %.17f %.17f\n", network2->output[0], network2->output[1]);
		neural_network_destroy(network2);
	}
	neural_network_destroy(network);
}

