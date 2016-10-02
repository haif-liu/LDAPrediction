/*
    learn.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include "learn.h"
#include "feature.h"
#include "imatrix.h"
#include "dmatrix.h"
#include "util.h"

void ldapred_learn(document *data, int ndocs, int nclass, double **theta, double **phi, double alpha, int maxiter){
    document *dp;
    int *n_m;
    int **n_mz;
    int ***topics;
    int word_index;
    int word_num;
    double *left;
    double *right;
    double *p_z;
    double *cum_sum_p_z;
    double sum_p_z;
    double **temp_theta;
    int z;
    int it;
    int m, w, t, i, j, k;
    const gsl_rng_type *T;
    gsl_rng *r;
    
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);
    
    
    // initialize buffers
    if((n_m = calloc(ndocs,sizeof(int))) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate n_m.\n");
        return;
    }
    if((n_mz = imatrix(ndocs, nclass)) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate n_mz.\n");
        return;
    }
    if((left = calloc(nclass,sizeof(double))) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate left.\n");
        return;
    }
    if((right = calloc(nclass,sizeof(double))) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate right.\n");
        return;
    }
    if((p_z = calloc(nclass,sizeof(double))) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate p_z.\n");
        return;
    }
    if((cum_sum_p_z = calloc((nclass+1),sizeof(double))) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate cum_sum_p_z.\n");
        return;
    }
    if((topics = calloc(ndocs,sizeof(int **))) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate topics.\n");
        return;
    }
    if((temp_theta = dmatrix(ndocs, nclass)) == NULL){
        fprintf(stderr,"ldapred_learn:: cannot allocate temp_theta.\n");
        exit(1);
    }
    
    printf("Number of documents          = %d\n",ndocs);
    printf("Number of latent classes     = %d\n",nclass);
    printf("Number of iteration          = %d\n",maxiter);
    printf("Hyperparameter Alpha         = %.2f\n",alpha);
    
    
    // choose an arbitrary topic as first topic for word
    gsl_rng_set(r, time(NULL));
    for(dp = data, m = 0;(dp->len) != -1;dp++, m++){
        if((topics[m] = calloc((dp->len), sizeof(int *))) == NULL){
            fprintf(stderr,"ldapred_learn:: cannot allocate topics[m].\n");
            return;
        }
        for(w = 0;w < (dp->len);w++){
            if((topics[m][w] = calloc((dp->cnt[w]), sizeof(int))) == NULL){
                fprintf(stderr,"ldapred_learn:: cannot allocate topics[m][w].\n");
                return;
            }
            word_index = dp->id[w];
            word_num = dp->cnt[w];
            for(i = 0;i < word_num;i++){
                z = (int)gsl_rng_uniform_int(r, nclass);
                n_mz[m][z] += 1;
                n_m[m] += 1;
                topics[m][w][i] = z;
            }
        }
    }
    
    // learning main
    for(it = 0;it < maxiter;it++){
        printf("iteration %2d/%3d..\r", it + 1, maxiter);
        fflush(stdout);
        for (dp = data, m = 0; (dp->len) != -1; dp++, m++){
            // for words
            for(w = 0;w < (dp->len);w++){
                word_index = dp->id[w];
                word_num = dp->cnt[w];
                for(i = 0;i < word_num;i++){
                    z = topics[m][w][i];
                    n_mz[m][z] -= 1;
                    n_m[m] -= 1;
                    
                    // compute conditional distribution p_z
                    // p_z left
                    for(k = 0;k < nclass;k++){
                        left[k] = phi[word_index][k];
                    }
                    // p_z right
                    for(k = 0;k < nclass;k++){
                        right[k] = (double)n_mz[m][k] + alpha;
                        right[k] /= ((double)n_m[m] + (double)nclass * alpha);
                    }
                    // conditional distribution p_z
                    sum_p_z = 0.0;
                    for(k = 0;k < nclass;k++){
                        p_z[k] = left[k] * right[k];
                        sum_p_z += p_z[k];
                    }
                    for(k = 0;k < nclass;k++){
                        p_z[k] /= sum_p_z; // normalize to obtain probabilities
                    }
                    // random sampling from p_z
                    z = sampling_multinomial(r, p_z, cum_sum_p_z, nclass);
                    // update buffers
                    n_mz[m][z] += 1;
                    n_m[m] += 1;
                    topics[m][w][i] = z;
                }
            }
        }
    }
    
    // compute matrix theta ([ndocs, nclass])
    for(m = 0;m < ndocs;m++)
        for(k = 0;k < nclass;k++)
            temp_theta[m][k] = (double)n_mz[m][k] + alpha;
    normalize_matrix_row(theta, temp_theta, ndocs, nclass);
    
    free(n_m);
    free(left);
    free(right);
    free(p_z);
    free(cum_sum_p_z);
    
    for(dp = data, m = 0;(dp->len) != -1;dp++, m++){
        for(w = 0;w < (dp->len);w++){
            free(topics[m][w]);
        }
        free(topics[m]);
    }
    free(topics);
    free_imatrix(n_mz, ndocs);
    free_dmatrix(temp_theta, ndocs);
    
    return;
}

int sampling_multinomial(gsl_rng *r, double *p, double *cum_sum_p, int len_p){
    int k, z;
    double sampling;
    
    cum_sum_p[0] = 0.0;
    for(k = 0;k < len_p;k++){
        cum_sum_p[k+1] = cum_sum_p[k] + p[k];
    }
    sampling = gsl_rng_uniform(r);
    for(k = 0;k < len_p;k++){
        if((sampling >= cum_sum_p[k]) && (sampling < cum_sum_p[k+1])){
            z = k;
            break;
        }
    }
    return z;
}
