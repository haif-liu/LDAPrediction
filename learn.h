/*
    learn.h
*/
#ifndef LEARN_H
#define LEARN_H
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include "feature.h"

int sampling_multinomial(gsl_rng *r, double *p, double *cum_sum_p, int len_p);

extern void ldapred_learn(document *data, int ndocs, int nclass, double **theta, double **phi, double alpha, int maxiter);

#endif
