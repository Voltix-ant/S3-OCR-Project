CC = gcc
CFLAGS = -I. -Icommon/src -Ianalysis/src -Ineural_network/src -Isolver/src \
	-Wall -Wextra -fsanitize=address -g
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
NEURAL_NETWORK_SRC  = \
	$(filter-out $(NEURAL_NETWORK_MAIN),$(wildcard neural_network/src/*.c))
NEURAL_NETWORK_OBJ  = $(NEURAL_NETWORK_MAIN:.c=.o) $(NEURAL_NETWORK_SRC:.c=.o)

# Solver files :
SOLVER_MAIN = solver/src/solver_main.c
SOLVER_SRC= $(filter-out $(SOLVER_MAIN),$(wildcard solver/src/*.c))
SOLVER_OBJ = $(SOLVER_MAIN:.c=.o) $(SOLVER_SRC:.c=.o)

# UI files :
UI_SRC = $(wildcard ui/src/*.c)
UI_OBJ = $(UI_SRC:.c=.o)

# Pretreatment file :
PRETREATMENT_SRC = $(wildcard pretreatment/src/*.c)
PRETREATMENT_OBJ = $(PRETREATMENT_SRC:.c=.o)

# Whole project :
EXEC_NAME = ocr
SRC = main.c $(COMMON_SRC) $(ANALYSIS_SRC) $(NEURAL_NETWORK_SRC) \
	  $(PRETREATMENT_SRC) $(UI_SRC) $(SOLVER_SRC)
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

solver: $(COMMON_OBJ) $(SOLVER_OBJ)
	$(CC) $^ -o solver_test $(CFLAGS) $(LIBFLAGS)

solver_tests: $(SOLVER_SRC:.c=.o) solver/tests/test_solver.o
	$(CC) $^ -o solver_tests $(CFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LIBFLAGS)

clean:
	$(RM) $(EXEC_NAME) main.o $(COMMON_OBJ)
	# analysis :
	$(RM) $(ANALYSIS_OBJ) analysis_test
	# neural network :
	$(RM) $(NEURAL_NETWORK_OBJ) neural_network_test
	# solver :
	$(RM) $(SOLVER_OBJ) solver_test
	$(RM) solver/tests/test_solver.o solver_tests
	# ui :
	$(RM) $(UI_OBJ)
	# pretreatment
	$(RM) $(PRETREATMENT_OBJ)

.PHONY: all analysis neuralnetwork solver clean
