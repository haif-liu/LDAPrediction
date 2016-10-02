/*
    ldapred.c
    LDAPRED, main driver.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ldapred.h"
#include "learn.h"
#include "writer.h"
#include "feature.h"
#include "dmatrix.h"
#include "imatrix.h"
#include "util.h"

int main(int argc, char **argv){
    document *data;
    FILE *tp, *hyperp; // for theta, hyperparameters
    char c;
    int nlex, dlenmax;
    int prelex, preclass, preiter, prehypers;
    int ndoc, nclass;
    int maxiter = MAXITER_DEFAULT;
    double **phi;
    double **theta;
    double **hypers;
    double alpha;
    
    while((c = getopt(argc, argv, "I:h")) != -1){
        switch(c){
            case 'I': maxiter = atoi(optarg); break;
            case 'h': usage(); break;
            default: usage(); break;
        }
    }
    if(!(argc - optind == 3))
        usage();
    
    // open data
    if((data = feature_matrix(argv[optind], &nlex, &dlenmax, &ndoc)) == NULL){
        fprintf(stderr, "ldapred:: cannot open training data.\n");
        exit(1);
    }
    
    // open phi
    if((phi = load_dmatrix(strconcat(argv[optind+1], ".phi"), &prelex, &preclass)) == NULL){
        fprintf(stderr, "ldapred:: cannot open phi.\n");
        exit(1);
    }
    nlex = prelex;
    nclass = preclass;
    
    // open hyperparameters
    if((hypers = load_dmatrix(strconcat(argv[optind+1], ".hyper"), &preiter, &prehypers)) == NULL){
        fprintf(stderr, "ldapred:: cannot open phi.\n");
        exit(1);
    }
    alpha = hypers[preiter-1][0];
    
    // open model output
    if((tp = fopen(strconcat(argv[optind+2], ".theta"),"w")) == NULL){
        fprintf(stderr, "ldapred:: cannot open model outputs.\n");
        exit(1);
    }
    
    // allocate parameters
    if((theta = dmatrix(ndoc, nclass)) == NULL){
        fprintf(stderr, "crm:: cannot allocate theta.\n");
        exit(1);
    }
    
    ldapred_learn(data, ndoc, nclass, theta, phi, alpha, maxiter);
    ldapred_write(tp, theta, ndoc, nclass);
    
    free_feature_matrix(data);
    free_dmatrix(phi, nlex);
    free_dmatrix(hypers, preiter);
    free_dmatrix(theta, ndoc);
    
    fclose(tp);
    
    exit(0);
}

void usage(void){
    printf("usage: %s [-I maxiter] doc premodel model\n", "ldapred");
    exit(0);
}
