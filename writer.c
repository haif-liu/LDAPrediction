/*
    writer.c
    an implementation of matrix writer
*/
#include <stdio.h>
#include "writer.h"

void ldapred_write(FILE *tp, double **theta, int ndoc, int nclass){
    printf("writing model...\n"); fflush(stdout);
    write_matrix(tp, theta, ndoc, nclass);
    printf("done.\n"); fflush(stdout);
}

void write_matrix(FILE *fp, double **matrix, int rows, int cols){
    int i, j;
    for(i = 0;i < rows;i++)
        for(j = 0;j < cols;j++)
            fprintf(fp, "%.7e%s", matrix[i][j], (j == cols - 1)? "\n":" ");
}

void write_imatrix(FILE *fp, int **matrix, int rows, int cols){
    int i, j;
    for(i = 0;i < rows;i++)
        for(j = 0;j < cols;j++)
            fprintf(fp, "%d%s", matrix[i][j], (j == cols - 1)? "\n":" ");
}

void write_imatrix_transpose(FILE *fp, int **matrix, int rows, int cols){
    int i, j;
    for(j = 0;j < cols;j++)
        for(i = 0;i < rows;i++)
            fprintf(fp, "%d%s", matrix[i][j], (i == rows -1)? "\n":" ");
}
