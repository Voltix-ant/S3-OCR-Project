#include "neural_network.h"
#include "neural_save.h"
#include <stdio.h>
#include <stdlib.h>

#define SAVE_PATH "neural_network/saves/xnor.txt"
#define NB_EXAMPLES 4
#define NB_EXTRA_NETWORKS 7
#define EPOCHS 2000
#define LEARNING_RATE 0.1

static double input_0[] = {1.0, 0.0};
static double input_1[] = {0.0, 0.0};
static double input_2[] = {0.0, 1.0};
static double input_3[] = {1.0, 1.0};

static double expected_0[] = {1.0, 0.0};
static double expected_1[] = {0.0, 1.0};
static double expected_2[] = {1.0, 0.0};
static double expected_3[] = {0.0, 1.0};

static double *inputs[NB_EXAMPLES] = {input_0, input_1, input_2, input_3};
static const int xnor_results[NB_EXAMPLES] = {0, 1, 0, 1};

static void print_predictions(NeuralNetwork *network)
{
        double output[2] = {0.0, 0.0};

        for (size_t i = 0; i < NB_EXAMPLES; i++) {
                neural_network_predict(network, inputs[i], output);
                printf("%.1f == %.1f = %d\n", inputs[i][0], inputs[i][1],
                       translate_output_to_xnor(output));
        }
}

static int count_correct(NeuralNetwork *network)
{
        double output[2] = {0.0, 0.0};
        int correct = 0;

        for (size_t i = 0; i < NB_EXAMPLES; i++) {
                neural_network_predict(network, inputs[i], output);
                if (translate_output_to_xnor(output) == xnor_results[i])
                        correct++;
        }
        return correct;
}

int main(void)
{
        srand(42);

        const size_t input_size = 2;
        const size_t hidden_size = 4;
        const size_t output_size = 2;

        TrainingExample examples[] = {
            {input_0, expected_0},
            {input_1, expected_1},
            {input_2, expected_2},
            {input_3, expected_3},
        };

        Dataset dataset = {.examples = examples,
                           .size = NB_EXAMPLES,
                           .input_size = input_size,
                           .output_size = output_size};

        NeuralNetwork *network =
            neural_network_create(input_size, hidden_size, output_size);
        if (!network) {
                fprintf(stderr, "Error creating the network\n");
                return EXIT_FAILURE;
        }

        printf("Starting the training\n");
        neural_network_train(network, &dataset, EPOCHS, LEARNING_RATE);

        // saving and loading the network
        if (neural_network_save(network, SAVE_PATH) == -1)
                fprintf(stderr, "Error while saving\n");
        else
                printf("Settings saved at: %s\n", SAVE_PATH);

        NeuralNetwork *loaded = neural_network_load(SAVE_PATH);
        if (!loaded)
                fprintf(stderr, "Error while loading the network from: %s\n",
                        SAVE_PATH);

        printf("\nPrediction after training:\n");
        print_predictions(network);
        int successes = count_correct(network) == NB_EXAMPLES;

        if (loaded) {
                printf("\nPrediction after loading %s:\n", SAVE_PATH);
                print_predictions(loaded);

                // both networks last predicted input_3
                printf("\nRaw comparison\n");
                printf("Network 1: %.17f %.17f\n", network->output[0],
                       network->output[1]);
                printf("Network 2: %.17f %.17f\n", loaded->output[0],
                       loaded->output[1]);
                neural_network_destroy(loaded);
        }
        neural_network_destroy(network);

        // training more networks with other random weights
        printf("\nTraining %d other networks with random weights\n",
               NB_EXTRA_NETWORKS);
        for (int i = 0; i < NB_EXTRA_NETWORKS; i++) {
                NeuralNetwork *extra = neural_network_create(input_size,
                                                hidden_size, output_size);
                if (!extra) {
                        fprintf(stderr, "Error creating network %d\n", i + 2);
                        return EXIT_FAILURE;
                }
                neural_network_train(extra, &dataset, EPOCHS, LEARNING_RATE);
                int correct = count_correct(extra);
                printf("Network %d: %d/%d correct\n", i + 2, correct,
                       NB_EXAMPLES);
                if (correct == NB_EXAMPLES)
                        successes++;
                neural_network_destroy(extra);
        }

        printf("\n%d/%d networks learned XNOR\n", successes,
               NB_EXTRA_NETWORKS + 1);
        return EXIT_SUCCESS;
}
