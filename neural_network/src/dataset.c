#include "dataset.h"
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
