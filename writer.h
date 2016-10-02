/*
    writer.h
    a header file of matrix writer
*/
#ifndef WRITER_H
#define WRITER_H
#include <stdio.h>

extern void ldapred_write(FILE *tp, double **theta, int ndoc, int nclass);
void write_matrix(FILE *fp, double **matrix, int rows, int cols);
void write_imatrix(FILE *fp, int **matrix, int rows, int cols);
void write_imatrix_transpose(FILE *fp, int **matrix, int rows, int cols);
#endif
