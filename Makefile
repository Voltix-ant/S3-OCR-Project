CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g
LIBFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image -lm

# Analysis files :
ANALYSIS_MAIN = analysis/src/analysis_main.o
ANALYSIS_SRC  = $(filter-out $(ANALYSIS_MAIN),$(wildcard analysis/src/*.c))
ANALYSIS_OBJ  = $(ANALYSIS_SRC:.c=.o)
# Do the same with the other modules of the project to keep your test main

# Whole project :
EXEC_NAME = ocr
SRC = main.c $(ANALYSIS_SRC)
OBJ = $(SRC:.c=.o)

all: $(SRC)
	# compile the whole project without the main in the different sections
	$(CC) $^ -o $(EXEC_NAME) $(CFLAGS) $(LIBFLAGS)

analysis: $(ANALYSIS_MAIN) $(ANALYSIS_OBJ)
	# for analysis tests : 
	# compile the main fucntion in analysis/src/analysis_main
	$(CC) $^ -o analysis_test $(CFLAGS) $(LIBFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LIBFLAGS)

clean:
	$(RM) $(EXEC_NAME) main.o
	# analysis :
	$(RM) $(ANALYSIS_MAIN) $(ANALYSIS_OBJ) analysis_test

.PHONY: all analysis clean
