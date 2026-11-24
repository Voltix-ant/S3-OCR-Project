#include "analysis.h"
#include "dataset.h"
#include "letters.h"
#include "neural_save.h"
#include "pretreatment/src/pretreatment.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_IMAGES 3
#define SAVE_PATH "neural_network/saves/last_train.txt"
#define SEED 42

#define DEFAULT_EPOCHS 30
#define DEFAULT_LEARNING_RATE 0.5
#define DEFAULT_BATCH_SIZE 16

static const char *images[NB_IMAGES] = {
    "img_test/lvl1-img1.png",
    "img_test/lvl1-img2.png",
    "img_test/lvl1-img3.png",
};

static const char *label_files[NB_IMAGES] = {
    "img_test/lvl1-img1.txt",
    "img_test/lvl1-img2.txt",
    "img_test/lvl1-img3.txt",
};

static struct matrix *load_binarised(const char *path) {
	SDL_Surface *image = IMG_Load(path);
	if (!image) {
		warnx("%s: %s", path, IMG_GetError());
		return NULL;
	}

	SDL_Surface *surface =
	    SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(image);
	if (!surface) {
		warnx("%s: %s", path, SDL_GetError());
		return NULL;
	}

	grayscale(surface);
	adaptive_threshold(surface);
	struct matrix *matrix = surface_to_matrix(surface);
	SDL_FreeSurface(surface);
	return matrix;
}

static Dataset *image_dataset(const char *image_path,
			      const char *label_path) {
	size_t rows = 0;
	size_t cols = 0;
	char *labels = labels_read_grid(label_path, &rows, &cols);
	if (!labels) {
		return NULL;
	}

	struct matrix *matrix = load_binarised(image_path);
	if (!matrix) {
		free(labels);
		return NULL;
	}

	Dataset *dataset = NULL;
	struct analysis *analysis = analysis_run(matrix);
	if (!analysis) {
		warnx("%s: the analysis found no grid", image_path);
	} else {
		dataset = dataset_from_grid(analysis, labels, rows, cols);
		analysis_destroy(analysis);
	}

	// the matrix must outlive the analysis
	destroy_matrix(matrix);
	free(labels);

	if (dataset) {
		printf("%s: %zu letters\n", image_path, dataset->size);
	} else {
		warnx("%s: cannot build the dataset", image_path);
	}
	return dataset;
}

static size_t parse_size(const char *text, const char *name) {
	char *end = NULL;
	unsigned long value = strtoul(text, &end, 10);

	if (*text == '\0' || *end != '\0' || value == 0) {
		errx(EXIT_FAILURE, "%s must be a positive integer: %s", name,
		     text);
	}
	return (size_t)value;
}

static double parse_rate(const char *text) {
	char *end = NULL;
	double value = strtod(text, &end);

	if (*text == '\0' || *end != '\0' || value <= 0.0) {
		errx(EXIT_FAILURE, "learning_rate must be positive: %s", text);
	}
	return value;
}

static void print_results(double rate, const size_t *correct,
			  const size_t *total, size_t size) {
	size_t recognized = 0;
	for (int k = 0; k < LETTER_OUTPUT_SIZE; k++) {
		recognized += correct[k];
	}
	printf("\nRecognition on the training data: %.2f %% (%zu/%zu)\n",
	       rate * 100.0, recognized, size);
	printf("\nLetter  recognized  rate\n");

	for (int k = 0; k < LETTER_OUTPUT_SIZE; k++) {
		if (total[k] == 0) {
			printf("  %c        0/0       -\n", 'A' + k);
		} else {
			printf("  %c     %3zu/%-3zu   %6.1f %%\n", 'A' + k,
			       correct[k], total[k],
			       100.0 * (double)correct[k] /
				   (double)total[k]);
		}
	}
}

int main(int argc, char **argv) {
	if (argc > 5) {
		errx(EXIT_FAILURE, "usage: %s [hidden_size] [epochs] "
				   "[learning_rate] [batch_size]",
		     argv[0]);
	}
	size_t hidden_size = argc > 1 ? parse_size(argv[1], "hidden_size")
				      : LETTER_HIDDEN_SIZE;
	size_t epochs = argc > 2 ? parse_size(argv[2], "epochs")
				 : DEFAULT_EPOCHS;
	double learning_rate =
	    argc > 3 ? parse_rate(argv[3]) : DEFAULT_LEARNING_RATE;
	size_t batch_size = argc > 4 ? parse_size(argv[4], "batch_size")
				     : DEFAULT_BATCH_SIZE;

	// same seed every run, so that two trainings can be compared
	srand(SEED);

	Dataset *parts[NB_IMAGES] = {NULL};
	int failed = 0;
	for (size_t i = 0; i < NB_IMAGES && !failed; i++) {
		parts[i] = image_dataset(images[i], label_files[i]);
		failed = parts[i] == NULL;
	}
	Dataset *dataset = failed ? NULL : dataset_concat(parts, NB_IMAGES);
	for (size_t i = 0; i < NB_IMAGES; i++) {
		dataset_free(parts[i]);
	}
	IMG_Quit();
	if (!dataset) {
		errx(EXIT_FAILURE, "cannot build the training dataset");
	}

	NeuralNetwork *network = neural_network_create(
	    LETTER_INPUT_SIZE, hidden_size, LETTER_OUTPUT_SIZE);
	if (!network) {
		dataset_free(dataset);
		errx(EXIT_FAILURE, "cannot create the network");
	}

	printf("\nNetwork %d -> %zu -> %d, %zu epochs, learning rate %g, "
	       "batch size %zu, %zu examples\n",
	       LETTER_INPUT_SIZE, hidden_size, LETTER_OUTPUT_SIZE, epochs,
	       learning_rate, batch_size, dataset->size);

	clock_t start = clock();
	neural_network_train(network, dataset, epochs, learning_rate,
			     batch_size);
	printf("Training time: %.1f s\n",
	       (double)(clock() - start) / CLOCKS_PER_SEC);

	size_t correct[LETTER_OUTPUT_SIZE];
	size_t total[LETTER_OUTPUT_SIZE];
	double rate = letter_evaluate(network, dataset, correct, total);
	print_results(rate, correct, total, dataset->size);

	if (neural_network_save(network, SAVE_PATH) == -1) {
		warnx("cannot save the network to %s", SAVE_PATH);
	} else {
		printf("\nNetwork saved to %s\n", SAVE_PATH);
	}

	neural_network_destroy(network);
	dataset_free(dataset);
	return EXIT_SUCCESS;
}
