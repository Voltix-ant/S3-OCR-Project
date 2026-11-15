#include "matrix.h"

struct matrix *create_char_matrix(void) {
    struct matrix *cm = calloc(1,sizeof(struct matrix));
    if (!cm)
        errx(1, "create_char_matrix : Couldn't allocate memory");
    cm->width = 28;
    cm->height = 28;
    cm->data = calloc(28,sizeof(unsigned char *));
    for (size_t y = 0; y < 28; y++) {
        cm->data[y] = calloc(28, sizeof(unsigned char));
        if (!cm->data[y])
            errx(1, "create_char_matrix : Couldn't allocate memory");
    }
    return cm;
}

void print_matrix(struct matrix *m) {
    printf("  ");
    for (size_t a = 0; a < 2*m->width +2; a++) printf("_");
    printf("\n");
    for (size_t y = 0; y < m->height; y++) {
        printf("  |");
        for (size_t x = 0; x < m->width; x++) {
            if (m->data[y][x] == 0) {
                printf("  ");
            } else {
                printf("##");
            }
        }
        printf("|\n");
    }
    printf("  ");
    for (size_t a = 0; a < 2*m->width +2; a++) printf("_");
    printf("\n");
}

void destroy_matrix(struct matrix *m) {
    if (m) {
        for (size_t i = 0; i < m->height; i++) {
            free(m->data[i]);
        }
        free(m->data);
        free(m);
    }
}

struct matrix *matrix_rotate90(struct matrix *m, int quarter_turns) {
    /* This function rotate the matrix by quarter_terns*90° clockwise,
        it destroys the source matrix*/
    quarter_turns = quarter_turns % 4;
    if (quarter_turns == 0) return m;
    struct matrix *res = malloc(sizeof(struct matrix));
    if (!res)
        errx(1, "matrix_rotate90 : Couldn't allocate memory");
    if (quarter_turns == 2) {
        res->width = m->width;
        res->height = m->height;
    } else {
        res->width = m->height;
        res->height = m->width;
    }
    res->data = calloc(res->height,sizeof(unsigned char *));
    for (size_t y = 0; y < res->height; y++) {
        res->data[y] = calloc(res->width, sizeof(unsigned char));
        for (size_t x = 0; x < res->width; x++) {
            if (quarter_turns == 2) {
                res->data[y][x] = m->data[res->height-1-y][res->width-1-x];
            } else {
                if (quarter_turns == 3)
                    res->data[y][x] = m->data[x][y];
                else
                    res->data[y][x] = m->data[res->width-1-x][res->height-1-y];
            }
        }
    }
    destroy_matrix(m);
    return res;
}
