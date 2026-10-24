CC = gcc
CFLAGS = -I. -Wall -Wextra -fsanitize=address -g
LIBFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image -lm


# Common files :
COMMON_SRC  = $(wildcard common/src/*.c)
COMMON_OBJ  = $(COMMON_SRC:.c=.o)

# Analysis files :
ANALYSIS_MAIN = analysis/src/analysis_main.c
ANALYSIS_SRC  = $(filter-out $(ANALYSIS_MAIN),$(wildcard analysis/src/*.c))
ANALYSIS_OBJ  = $(ANALYSIS_MAIN:.c=.o) $(ANALYSIS_SRC:.c=.o)
# Do the same with the other modules of the project to keep your test main

# Neural Network files :
NEURAL_NETWORK_MAIN = neural_network/src/neural_network_main.c
NEURAL_NETWORK_SRC  = $(filter-out $(NEURAL_NETWORK_MAIN),$(wildcard neural_network/src/*.c))
NEURAL_NETWORK_OBJ  = $(NEURAL_NETWORK_MAIN:.c=.o) $(NEURAL_NETWORK_SRC:.c=.o)

# Whole project :
EXEC_NAME = ocr
SRC = main.c $(COMMON_SRC) $(ANALYSIS_SRC)
OBJ = $(SRC:.c=.o)

all: $(OBJ)
	# compile the whole project without the main in the different sections
	$(CC) $^ -o $(EXEC_NAME) $(CFLAGS) $(LIBFLAGS)

analysis: $(COMMON_OBJ) $(ANALYSIS_OBJ)
	# for analysis tests : 
	# compile the main function in analysis/src/analysis_main
	$(CC) $^ -o analysis_test $(CFLAGS) $(LIBFLAGS)

neuralnetwork: $(COMMON_OBJ) $(NEURAL_NETWORK_OBJ)
	$(CC) $^ -o neural_network_test $(CFLAGS) $(LIBFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LIBFLAGS)

clean:
	$(RM) $(EXEC_NAME) main.o $(COMMON_OBJ)
	# analysis :
	$(RM) $(ANALYSIS_OBJ) analysis_test

.PHONY: all analysis clean
