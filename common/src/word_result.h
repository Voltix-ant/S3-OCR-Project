#ifndef WORD_RESULT_H
#define WORD_RESULT_H

/* This file defines the struct found_word which is filled aftere calling the
   solver and used by the render module to create the image */

struct found_word {
    int found;               // 1 if the word was found, 0 otherwise
    size_t r1, c1;           // cell of the first letter
    size_t r2, c2;           // cell of the last letter
};

#endif
