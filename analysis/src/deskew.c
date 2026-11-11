#include "deskew.h"
#include <stdio.h>

static double skew_score(const double *cxs, const double *cys, size_t n,
                        double a_deg, double bin_w,
                        double *yps, int *hist, size_t nb_bins) {
 /*Score of an angle : the higher it is, the more the centers are
    aligned in sharp lines when seen in the direction of this angle.
    cxs (resp cys) is an array of double corresponding to the x (resp y) coord
    of the center of each component.
    yps and hist are scratch buffers given by the caller.*/
    double a_rad = a_deg * M_PI / 180.0;
    double s = sin(a_rad);
    double c = cos(a_rad);

    double yp_min = 0;
    for (size_t i = 0; i < n; i++) {
        yps[i] = -cxs[i] * s + cys[i] * c;
        if (i == 0 || yps[i] < yp_min)
            yp_min = yps[i];
    }

    for (size_t i = 0; i < nb_bins; i++) {
        hist[i] = 0;
    }
    for (size_t i = 0; i < n; i++) {
        int bin = (int)((yps[i] - yp_min) / bin_w);
        if ((size_t)bin < nb_bins)
            hist[bin]++;
    }
    double sum = 0;
    for (size_t i = 0; i < nb_bins; i++) sum += hist[i]*hist[i];
    return sum;
}

double find_deskew_angle(struct matrix *m) {
    /*Finds the skew angle of the image, in degrees, between -45 and +45.
    Returns 0 if there is not enough information to decide.*/
    struct box_list *comps = find_components(m,0,0,
            (int)m->width-1,(int)m->height-1);
    int char_h = median_box_height(comps);
    filter_letter_boxes(comps, char_h);

    // If not enough information, return 0.0;
    const size_t min_boxes = 20;
    if (char_h == 0 || comps->count < min_boxes) {
        destroy_box_list(comps);
        errx(1, "find_deskew_angle : Not enough information");
    }

    // Compute the center of each component in 2 arrays :
    size_t n = comps->count;
    double *cxs = malloc(n*sizeof(double));
    double *cys = malloc(n*sizeof(double));
    double *yps = malloc(n*sizeof(double));
    if (!cxs || !cys || !yps) {
        errx(1, "find_deskew_angle : Could not allocate memory");
    }
    for (size_t i = 0; i < n; i++) {
        cxs[i] = (comps->items[i].x1 + comps->items[i].x2) / 2.0;
        cys[i] = (comps->items[i].y1 + comps->items[i].y2) / 2.0;
    }
    destroy_box_list(comps);

    double bin_w = 0.25 * char_h;
    if (bin_w < 1.0)
        bin_w = 1.0;
    double w = (double)m->width;
    double h = (double)m->height;
    // the centers projected on any direction fit in the image diagonal
    size_t nb_bins = (size_t)(sqrt(w * w + h * h) / bin_w) + 2;
    int *hist = malloc(nb_bins * sizeof(int));
    if (!hist)
        errx(1, "find_deskew_angle : Could not allocate memory");

    // Coarse sweep :
    //printf("------------------------- COARSE SWEEP -------------------\n");
    double coarse_scores[91];
    double best = -45.0;
    for (double a_deg = -45.0; a_deg <= 45.0; a_deg++) {
        size_t i = (size_t)(a_deg+45.0);
        coarse_scores[i] = skew_score(cxs,cys,n,a_deg,bin_w,yps,hist,nb_bins);
        //printf("Angle %.2f -> score = %f\n",a_deg,coarse_scores[i]);
        if (coarse_scores[i] > coarse_scores[(size_t)(best+45.0)])
            best = a_deg;
    }
    //printf("Best angle : %.2f\n",best);

    // Fine sweep :
    //printf("------------------------- FINE SWEEP ---------------------\n");
    const int half = 10;
    double fine_scores[2 * half + 1];
    size_t best_ind = 0;
    for (int k = 0; k <= 2 * half; k++) {
        double a_deg = best + (k - half) * 0.1;
        fine_scores[k] = skew_score(cxs,cys,n,a_deg,bin_w,yps,hist,nb_bins);
        // printf("Angle %.2f -> score = %f\n",a_deg,fine_scores[k]);
        if (fine_scores[k] > fine_scores[best_ind])
            best_ind = k;
    }

    double sum = 0.0;
    int den = 0;
    double threshold = fine_scores[best_ind] * 0.95;
    for (int k = 0; k <= 2 * half; k++) {
        if (fine_scores[k] >= threshold) {
            sum += best + (k - half) * 0.1;
            den++;
        }
    }
    double final_res = sum / den;
    //printf("Final best : %.2f\n",final_res);

    free(hist);
    free(yps);
    free(cys);
    free(cxs);
    if (final_res >= 0.5 && final_res <= 0.5) return 0.0;
    return final_res;
}
