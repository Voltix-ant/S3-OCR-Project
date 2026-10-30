# S3-OCR-Project

## User Interface module

This module handles the graphical interface of the application, built
with SDL2 to ensure performance and memory safety.

### Architecture & Features

- **Responsive layout.** The window is resizable (`SDL_WINDOW_RESIZABLE`).
The control panel and preview zones are calculated using dynamic formulas to
adapt to any screen.
- **Preview ratio preservation.** The `get_fitted_rect` function allows the
preview image and text textures to keep their original proportions.
- **Custom buttons.** Button text is rendered via pre-loaded PNG images
to bypass `SDL_ttf` constraints.
- **Buttons' behavior.** The event loop relies on an `update_buttons` function
returning the clicked button's index, centralizing execution logic in the
`main` function.
- **Manual rotation slider.** Manual image rotation uses `double` precision for
fluid, sub-degree adjustments.

## Pretreatment module

This module prepares the raw input images for the neural network by correcting
lighting, noise, and alignment.

### Features

- **Bradley-Roth Adaptive Thresholding:** Binarizes the image using a local
adaptive thresholding algorithm (integral image technique).
The tolerance parameter (`t = 0.10`) is set to preserve thin characters.

## Analysis module

This module takes a preprocessed image of a word search puzzle and finds the
letter grid and the word list. For each letter, it provides a 28x28 image that
can be given to the neural network. It does **not** call the neural network
itself.

### Requirements

- The input image must be **binarised** and **correctly rotated**.
- It is given as a `struct matrix` (defined in `analysis/src/matrix.h`), where
  `0` is a black pixel (ink) and `255` a white pixel.
- Current limits: one grid per image, one word per line in the word list,
  simple images (no title, instructions or illustration).

### Usage

1. Include the header `analysis/src/analysis.h`.
2. Call `analysis_run(mat)` with the matrix of the image. It returns a
   `struct analysis *` (called `a` below), or `NULL` if the analysis failed
   (for example if no grid was found).
3. Read the results with the functions below, always by giving `a` as the
   first parameter. Do not read the fields of the struct directly.
4. When you are done, call `analysis_destroy(a)`.

```c
struct matrix *mat = /* binarised and rotated image */;
struct analysis *a = analysis_run(mat);
if (!a) {
    /* analysis failed */
}

/* ... use a ... */

analysis_destroy(a);
destroy_matrix(mat);   /* the analysis does not own the matrix */
```

### Functions

#### Letter grid

| Function | Description |
|---|---|
| `analysis_grid_rows(a)` | Number of rows of the letter grid |
| `analysis_grid_cols(a)` | Number of columns of the letter grid |
| `analysis_grid_letter(a, y, x)` | 28x28 image of the letter at row `y`, column `x` |
| `analysis_grid_box(a, y, x)` | Position of this letter in the original image |

#### Words

All the word lists found in the image are seen as **one single list**, from
top to bottom.

| Function | Description |
|---|---|
| `analysis_nb_words(a)` | Number of words detected |
| `analysis_word_len(a, i)` | Number of letters of the word of index `i` |
| `analysis_word_letter(a, i, j)` | 28x28 image of the letter of index `j` in the word of index `i` |
| `analysis_word_box(a, i, j)` | Position of this letter in the original image |

#### Debug

`analysis_save_debug(a, s)` saves two images with the detected areas drawn on
them: `macro_zones.png` (grid and word list zones) and `letter_grid.png` (one
rectangle per letter). `s` is the `SDL_Surface` of the image, already rotated
the same way as the matrix given to `analysis_run`. It returns `0` on success
and `-1` on error.

### Things to know

- **Indices start at 0.** There is no bounds checking : `y < analysis_grid_rows(a)`,
  `x < analysis_grid_cols(a)`, `i < analysis_nb_words(a)` and
  `j < analysis_word_len(a, i)` must be respected.
- **Free the letter images.** `analysis_grid_letter()` and
  `analysis_word_letter()` return a new `struct matrix` each time. Free it with
  `destroy_matrix()` once it has been given to the neural network.
- **28x28 format.** The letter is resized keeping its aspect ratio (a thin
  letter like `I` stays thin) and centered in the 28x28 matrix, with a margin
  around it. Pixels are in grey levels, black ink (`0`) on white background
  (`255`).
- **The matrix must stay valid.** The matrix given to `analysis_run()` is not
  copied : it must not be destroyed before `analysis_destroy(a)`.
- **Letter positions.** `analysis_grid_box()` and `analysis_word_box()` return a
  `struct box` (defined in `components.h`) with the coordinates of the letter
  in the image, `x1`, `y1`, `x2`, `y2` included. It is useful to draw the solved
  grid on the image.
- **Warnings.** If a row of the grid does not have the same number of letters as
  the others, a warning is printed. It usually means a letter was missed or split
  in two.

## Neural network module

A multilayer perceptron with one hidden layer (sigmoid), a softmax output and
a cross-entropy loss, trained by stochastic gradient descent. Layer sizes are
parameters: the proof of concept uses 2 → 4 → 2, the letter recognizer will
use 784 → n → 26 (28x28 images from the analysis module).

### Proof of concept (XNOR)

```sh
make neuralnetwork
./neural_network_test   # run from the repository root
```

The program trains the network on the function not(A).not(B) + A.B for 2000
epochs, saves it to `neural_network/saves/xnor.txt`, reloads it and checks
that both networks give the same outputs.

### Usage

| Function | Description |
|---|---|
| `neural_network_create(in, hidden, out)` | New network with Xavier-initialized weights, or `NULL` |
| `neural_network_train(net, dataset, epochs, rate)` | Trains on a `Dataset` of one-hot examples |
| `neural_network_predict(net, input, output)` | Writes the output probabilities into `output` |
| `neural_network_save(net, path)` | Saves the network to a text file, returns `0` or `-1` |
| `neural_network_load(path)` | Loads a saved network, or `NULL` if the file is invalid |
| `neural_network_destroy(net)` | Frees the network |

### Save file format

Text file: a version header `NN 1`, the three layer sizes, then the hidden
weights, hidden biases, output weights and output biases, written with 17
significant digits so that reloading is exact.

## Solver

### Build :
```
make solver
```

### Usage
```
./solver <grid_file> <word>
```
The grid file contains one row of letters per line, all rows of the same
length. The program prints the position of the first and last letters of
the word, `(x0,y0)(x1,y1)`, or `Not found`.
