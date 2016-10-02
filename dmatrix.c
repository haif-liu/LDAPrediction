/*
    dmatrix.c
    an implementation of double matrix
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dmatrix.h"
#define BUFFSIZE 65536
static int file_lines(FILE *fp);
static int n_fields(FILE *fp);
static int isspaces(char *s);

double **load_dmatrix(char *filename, int *rows, int *cols){
    FILE *fp;
    double **matrix;
    int i;
    char line[BUFFSIZE];
    
    if((fp = fopen(filename,"r")) == NULL)
        return NULL;
    
    *rows = file_lines(fp);
    *cols = n_fields(fp);
    matrix = (double **)calloc(*rows, sizeof(double *));
    if(matrix == NULL)
        return NULL;
    for(i = 0;i < *rows;i++){
        matrix[i] = (double *)calloc(*cols, sizeof(double));
        if(matrix[i] == NULL)
            return NULL;
    }
    
    i = 0;
    while(fgets(line,sizeof(line),fp)){
        char *sp, *lp = line;
        int j = 0;
        if(isspaces(line))
            continue;
        while(*lp){
            if((sp = strpbrk(lp, " \t\n")) == NULL)
                break;
            *sp = '\0';
            matrix[i][j] = atof(lp);
            lp = sp + 1;
            j++;
        }
        i++;
    }
    fclose(fp);
    return matrix;
}

double **dmatrix(int rows, int cols){
    double **matrix;
    int i;
    
    matrix = (double **)calloc(rows, sizeof(double *));
    if(matrix == NULL)
        return NULL;
    for(i = 0;i < rows;i++){
        matrix[i] = (double *)calloc(cols, sizeof(double));
        if(matrix[i] == NULL)
            return NULL;
    }
    
    return matrix;
}

void free_dmatrix(double **matrix, int rows){
    int i;
    for(i = 0;i < rows;i++){
        free(matrix[i]);
    }
    free(matrix);
}

static int file_lines(FILE *fp){
    int n = 0;
    char buff[BUFFSIZE];
    
    while(fgets(buff,sizeof(buff),fp)){
        if(!isspaces(buff))
            n++;
    }
    rewind(fp);
    return n;
}

static int n_fields(FILE *fp){
    int n = 0;
    char buff[BUFFSIZE];
    
    while(fgets(buff,sizeof(buff),fp)){
        if(isspaces(buff))
            continue;
        char *sp, *lp = buff;
        while(*lp){
            if(isspace(*lp)){
                lp++;
                continue;
            }
            if((sp = strpbrk(lp," \t\n")) == NULL)
                break;
            lp = sp + 1;
            n++;
        }
        break;
    }
    rewind(fp);
    return n;
}

static int isspaces(char *s){
    char *c = s;
    while(*c){
        if(!isspace(*c))
            return 0;
        c++;
    }
    return 1;
}
