#include "neural_network.h"
#include "neural_save.h"
#include <stdio.h>
#include <stdlib.h>

#define SAVE_PATH "neural_network/saves/xnor.txt"

int main(void) {
	srand(42);

	const size_t input_size = 2;
	const size_t hidden_size = 4;
	const size_t output_size = 2;

	double input_0[] = {1.0, 0.0};
	double input_1[] = {0.0, 0.0};
	double input_2[] = {0.0, 1.0};
	double input_3[] = {1.0, 1.0};

	double expected_0[] = {1.0, 0.0};
	double expected_1[] = {0.0, 1.0};
	double expected_2[] = {1.0, 0.0};
	double expected_3[] = {0.0, 1.0};

	TrainingExample examples[] = {
	    {input_0, expected_0},
	    {input_1, expected_1},
	    {input_2, expected_2},
	    {input_3, expected_3},

	};

	Dataset dataset = {.examples = examples,
			   .size = 4,
			   .input_size = input_size,
			   .output_size = output_size};

	NeuralNetwork *network =
	    neural_network_create(input_size, hidden_size, output_size);
	
	if (!network) {
		printf("Error creating the network");
		return EXIT_FAILURE;
	}

	printf("Starting the training\n");

	neural_network_train(network, &dataset, 2000, 0.1);
	
	// saving and loading the network ---------------------
	int res = neural_network_save(network, SAVE_PATH);
	if (res == -1) {
		printf("error while saving\n");
	} else {
		printf("settings saved at : %s\n", SAVE_PATH);
	}

	NeuralNetwork *network2 = neural_network_load(SAVE_PATH);
	if (network2 == NULL) {
		printf("Error while loading the network from : %s\n",
		       SAVE_PATH);
	}
	// -----------------------------------------------------
	
	printf("\nPrediction after training:\n");
	double output[2] = {0.0, 0.0};

	neural_network_predict(network, input_0, output);
	printf("%.1f == %.1f = %d\n", input_0[0], input_0[1],
	       translate_output_to_xnor(output));

	neural_network_predict(network, input_1, output);
	printf("%.1f == %.1f = %d\n", input_1[0], input_1[1],
	       translate_output_to_xnor(output));

	neural_network_predict(network, input_2, output);
	printf("%.1f == %.1f = %d\n", input_2[0], input_2[1],
	       translate_output_to_xnor(output));

	neural_network_predict(network, input_3, output);
	printf("%.1f == %.1f = %d\n", input_3[0], input_3[1],
	       translate_output_to_xnor(output));

	if (network2) {
		printf("\nPrediction after loading %s:\n", SAVE_PATH);
		neural_network_predict(network2, input_0, output);
		printf("%.1f == %.1f = %d\n", input_0[0], input_0[1],
		       translate_output_to_xnor(output));

		neural_network_predict(network2, input_1, output);
		printf("%.1f == %.1f = %d\n", input_1[0], input_1[1],
		       translate_output_to_xnor(output));

		neural_network_predict(network2, input_2, output);
		printf("%.1f == %.1f = %d\n", input_2[0], input_2[1],
		       translate_output_to_xnor(output));

		neural_network_predict(network2, input_3, output);
		printf("%.1f == %.1f = %d\n", input_3[0], input_3[1],
		       translate_output_to_xnor(output));

		printf("\nRaw Comparison\n");
		printf("Network 1: %.17f %.17f\n", network->output[0],
		       network->output[1]);
		printf("Network 2: %.17f %.17f\n", network2->output[0],
		       network2->output[1]);
		neural_network_destroy(network2);
	}
	neural_network_destroy(network);

	
	// creating 5 more networks to prove the neural network works
	network2 =
	    neural_network_create(input_size, hidden_size, output_size);
	if (!network2) {

		printf("Error creating the network");
		return EXIT_FAILURE;
	}
	NeuralNetwork *network3 =
	    neural_network_create(input_size, hidden_size, output_size);
	if (!network3) {
		printf("Error creating the network");
		neural_network_destroy(network2);
		return EXIT_FAILURE;
	}
	NeuralNetwork *network4 =
	    neural_network_create(input_size, hidden_size, output_size);
	if (!network4) {
		printf("Error creating the network");
		neural_network_destroy(network2);
		neural_network_destroy(network3);
		return EXIT_FAILURE;
	}
	NeuralNetwork *network5 =
	    neural_network_create(input_size, hidden_size, output_size);
	if (!network5) {
		printf("Error creating the network");
		neural_network_destroy(network2);
		neural_network_destroy(network3);
		neural_network_destroy(network4);
		return EXIT_FAILURE;
	}
	NeuralNetwork *network6 =
	    neural_network_create(input_size, hidden_size, output_size);
	if (!network6) {
		printf("Error creating the network");
		neural_network_destroy(network2);
		neural_network_destroy(network3);
		neural_network_destroy(network4);
		neural_network_destroy(network5);
		return EXIT_FAILURE;
	}
	
	// training all the networks
	printf("Here's the result of the training of 5 other " 
			"networks with randomly initialized weights\n");
	printf("Network 2\n");	
	neural_network_train(network2, &dataset, 2000, 0.1);
	printf("Network 3\n");	
	neural_network_train(network3, &dataset, 2000, 0.1);
	printf("Network 4\n");	
	neural_network_train(network4, &dataset, 2000, 0.1);
	printf("Network 5\n");	
	neural_network_train(network5, &dataset, 2000, 0.1);
	printf("Network 6\n");	
	neural_network_train(network6, &dataset, 2000, 0.1);

	// releasing all the network allocated
	neural_network_destroy(network2);
	neural_network_destroy(network3);
	neural_network_destroy(network4);
	neural_network_destroy(network5);
	neural_network_destroy(network6);
}
