#include "orientation.h"

static int cmp_double_asc(const void *a, const void *b) {
    double x = *(const double *)a;
    double y = *(const double *)b;
    return (x > y) - (x < y);
}

double median_letter_aspect(struct matrix *m) {
    /* This function calculate the median aspect ratio (height / width) of
       the letter boxes*/
    struct box_list *comps = find_components(m,0,0,
            (int)m->width-1,(int)m->height-1);
    int char_h = median_box_height(comps);
    filter_letter_boxes(comps, char_h);

    const size_t min_boxes = 20;
    if (char_h == 0 || comps->count < min_boxes) {
        destroy_box_list(comps);
        errx(1, "median_letter_aspect : Not enough information");
    }
    
    size_t n = comps->count;
    double *aspects = malloc(n*sizeof(double));
    if (!aspects)
        errx(1, "median_letter_aspect : Could not allocate memory");
    for (size_t i = 0; i < n; i++) {
        aspects[i] = (comps->items[i].y2 - comps->items[i].y1) /
                    (comps->items[i].x2 - comps->items[i].x1);
    }
    double result = 0.0;
    if (n > 0) {
        qsort(aspects, n, sizeof(double), cmp_double_asc);
        result = aspects[n / 2];
    }
    free(aspects);
    return result;
}

int is_text_sideways(struct matrix *m) {
    /* This function return wether the text must be rotated 90° because it's
       sidways */
    double median_aspect = median_letter_aspect(m);
    // if median_aspect < 1 -> heith < width -> letters ar sideways
    return median_aspect < 1.0;
}
