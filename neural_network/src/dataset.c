#include "dataset.h"
#include "analysis.h"
#include "letters.h"
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t line_length(const char *line) {
	return strcspn(line, "\r\n");
}

static int only_letters(const char *line, size_t length) {
	for (size_t i = 0; i < length; i++) {
		if (!isalpha((unsigned char)line[i])) {
			return 0;
		}
	}
	return 1;
}

static int grid_dimensions(FILE *file, const char *path, size_t *width,
			   size_t *height) {
	char *line = NULL;
	size_t capacity = 0;
	size_t line_nb = 0;
	int empty_seen = 0;
	int result = 0;

	*width = 0;
	*height = 0;
	while (result == 0 && getline(&line, &capacity, file) != -1) {
		line_nb++;
		size_t length = line_length(line);

		if (length == 0) {
			empty_seen = 1;
		} else if (empty_seen) {
			warnx("%s:%zu: letters after an empty line", path,
			      line_nb);
			result = -1;
		} else if (!only_letters(line, length)) {
			warnx("%s:%zu: the line must only contain letters",
			      path, line_nb);
			result = -1;
		} else if (*height > 0 && length != *width) {
			warnx("%s:%zu: %zu letters instead of %zu", path,
			      line_nb, length, *width);
			result = -1;
		} else {
			*width = length;
			(*height)++;
		}
	}
	free(line);

	if (result == 0 && *height == 0) {
		warnx("%s: the grid is empty", path);
		result = -1;
	}
	return result;
}

static int grid_fill(FILE *file, char *grid, size_t width, size_t height) {
	char *line = NULL;
	size_t capacity = 0;

	for (size_t y = 0; y < height; y++) {
		if (getline(&line, &capacity, file) == -1) {
			free(line);
			return -1;
		}
		for (size_t x = 0; x < width; x++) {
			grid[y * width + x] =
			    (char)toupper((unsigned char)line[x]);
		}
	}
	free(line);
	return 0;
}

char *labels_read_grid(const char *path, size_t *rows, size_t *cols) {
	FILE *file = fopen(path, "r");
	if (!file) {
		warn("%s", path);
		return NULL;
	}

	size_t width = 0;
	size_t height = 0;
	if (grid_dimensions(file, path, &width, &height) == -1) {
		fclose(file);
		return NULL;
	}

	char *grid = malloc(width * height);
	if (!grid) {
		fclose(file);
		return NULL;
	}

	rewind(file);
	if (grid_fill(file, grid, width, height) == -1) {
		warnx("%s: read error", path);
		free(grid);
		fclose(file);
		return NULL;
	}
	fclose(file);

	*rows = height;
	*cols = width;
	return grid;
}


static Dataset *dataset_create(size_t size, size_t input_size,
			       size_t output_size) {
	if (size == 0) {
		return NULL;
	}

	Dataset *dataset = malloc(sizeof(Dataset));
	if (!dataset) {
		return NULL;
	}
	dataset->examples = malloc(size * sizeof(TrainingExample));
	double *inputs = calloc(size * input_size, sizeof(double));
	double *expected = calloc(size * output_size, sizeof(double));
	if (!dataset->examples || !inputs || !expected) {
		free(dataset->examples);
		free(inputs);
		free(expected);
		free(dataset);
		return NULL;
	}

	for (size_t i = 0; i < size; i++) {
		dataset->examples[i].input = inputs + i * input_size;
		dataset->examples[i].expected = expected + i * output_size;
	}
	dataset->size = size;
	dataset->input_size = input_size;
	dataset->output_size = output_size;
	return dataset;
}

Dataset *dataset_from_grid(const struct analysis *analysis,
			   const char *labels, size_t rows, size_t cols) {
	if (!analysis || !labels) {
		return NULL;
	}

	size_t found_rows = analysis_grid_rows(analysis);
	size_t found_cols = analysis_grid_cols(analysis);
	if (found_rows != rows || found_cols != cols) {
		warnx("dataset: grid of %zux%zu letters found, %zux%zu "
		      "expected",
		      found_rows, found_cols, rows, cols);
		return NULL;
	}

	Dataset *dataset = dataset_create(rows * cols, LETTER_INPUT_SIZE,
					  LETTER_OUTPUT_SIZE);
	if (!dataset) {
		return NULL;
	}

	for (size_t y = 0; y < rows; y++) {
		for (size_t x = 0; x < cols; x++) {
			TrainingExample *example =
			    &dataset->examples[y * cols + x];
			struct matrix *letter =
			    analysis_grid_letter(analysis, y, x);

			int error =
			    letter_matrix_to_input(letter, example->input);
			if (error == 0) {
				error = letter_to_expected(labels[y * cols + x],
							   example->expected);
			}
			destroy_matrix(letter);

			if (error == -1) {
				warnx("dataset: invalid letter at row %zu, "
				      "column %zu",
				      y, x);
				dataset_free(dataset);
				return NULL;
			}
		}
	}
	return dataset;
}

Dataset *dataset_concat(Dataset **datasets, size_t count) {
	if (!datasets || count == 0 || !datasets[0]) {
		return NULL;
	}

	size_t input_size = datasets[0]->input_size;
	size_t output_size = datasets[0]->output_size;
	size_t total = 0;
	for (size_t i = 0; i < count; i++) {
		if (!datasets[i] || datasets[i]->input_size != input_size ||
		    datasets[i]->output_size != output_size) {
			warnx("dataset: cannot concatenate dataset %zu", i);
			return NULL;
		}
		total += datasets[i]->size;
	}

	Dataset *result = dataset_create(total, input_size, output_size);
	if (!result) {
		return NULL;
	}

	// copy the values: the source datasets can be freed afterwards
	size_t k = 0;
	for (size_t i = 0; i < count; i++) {
		for (size_t j = 0; j < datasets[i]->size; j++) {
			memcpy(result->examples[k].input,
			       datasets[i]->examples[j].input,
			       input_size * sizeof(double));
			memcpy(result->examples[k].expected,
			       datasets[i]->examples[j].expected,
			       output_size * sizeof(double));
			k++;
		}
	}
	return result;
}

void dataset_free(Dataset *dataset) {
	if (!dataset) {
		return;
	}
	// the examples point into two blocks starting at example 0
	free(dataset->examples[0].input);
	free(dataset->examples[0].expected);
	free(dataset->examples);
	free(dataset);
}
