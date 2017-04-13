/* 
 *  Copyright (c) 2008  Noah Snavely (snavely (at) cs.washington.edu)
 *    and the University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* matrix.c */
/* Matrix operations */

#include <assert.h>

#include <stdlib.h>
#include <stdio.h>

#include <float.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "defines.h"
#include "matrix.h"

//#define CLAMP(x,mn,mx) (((x) < mn) ? mn : (((x) > mx) ? mx : (x)))

/* Fill a given matrix with an n x n identity matrix */
void matrix_ident(int n, double *A) {
    int i, j;

    for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
	    if (i == j)
		A[i * n + j] = 1.0;
	    else
		A[i * n + j] = 0.0;
	}
    }
}

/* Fill a given matrix with an m x n matrix of zeroes */
void matrix_zeroes(int m, int n, double *A) {
    int i, j;

    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++) {
            A[i * n + j] = 0.0;
	}
    }
}

/* Transpose the m x n matrix A and put the result in the n x m matrix AT */
void matrix_transpose(int m, int n, double *A, double *AT) {
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            AT[j * m + i] = A[i * n + j];
}


/* Compute the matrix product R = AB */
void matrix_product(int Am, int An, int Bm, int Bn, 
                    const double *A, const double *B, double *R) {
    int r = Am;
    int c = Bn;
    int m = An;

//#if defined(WIN32) || defined(NO_CBLAS)
    int i, j, k;
//#endif

    if (An != Bm) {
        printf("[matrix_product] Error: the number of columns of A and the "
               "number of rows of B must be equal\n");
        return;
    }

//#if !defined(WIN32) && !defined(NO_CBLAS)
//    cblas_dgemm_driver(r, m, c, (double *) A, (double *) B, R);
//#else
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[i * An + k] * B[k * Bn + j];
            }
        }
    }
//#endif
}

void matrix_product_old(int Am, int An, int Bm, int Bn, 
                        const double *A, const double *B, double *R) {
    int i, j, k;

#if 0
    if (An != Bm) {
        printf("[matrix_product_old] Error: the number of columns of A "
               "and the number of rows of B must be equal\n");
        return;
    }
#endif

    for (i = 0; i < Am; i++) {
        for (j = 0; j < Bn; j++) {
            R[i * Bn + j] = 0.0;
            for (k = 0; k < An; k++) {
                R[i * Bn + j] += A[i * An + k] * B[k * Bn + j];
            }
        }
    }
}

/* Compute the power of a matrix */
void matrix_power(int n, double *A, int pow, double *R)
{
    int i;
    
    /* Slow way */
    double *curr = (double *) malloc(sizeof(double) * n * n);
    double *tmp = (double *) malloc(sizeof(double) * n * n);

    matrix_ident(n, curr);
    for (i = 0; i < pow; i++) {
	matrix_product(n, n, n, n, curr, A, tmp);
	memcpy(curr, tmp, sizeof(double) * n * n);
    }

    memcpy(R, curr, sizeof(double) * n * n);

    free(curr);
    free(tmp);
}

/* Compute the matrix sum R = A + B */
void matrix_sum(int Am, int An, int Bm, int Bn, 
                double *A,  double *B, double *R) {
    int r = Am;
    int c = An;
    int n = r * c, i;
    
    if (Am != Bm || An != Bn) {
	printf("[matrix_sum] Error: mismatched dimensions\n");
	return;
    }

    for (i = 0; i < n; i++) {
	R[i] = A[i] + B[i];
    }
}

/* Compute the matrix difference R = A - B */
void matrix_diff(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int r = Am;
    int c = An;
    int n = r * c, i;
    
    if (Am != Bm || An != Bn) {
	printf("[matrix_sum] Error: mismatched dimensions\n");
	return;
    }

    for (i = 0; i < n; i++) {
	R[i] = A[i] - B[i];
    }    
}

void matrix_product33(double *A, double *B, double *R)
{
    R[0] = A[0] * B[0] + A[1] * B[3] + A[2] * B[6];
    R[1] = A[0] * B[1] + A[1] * B[4] + A[2] * B[7];
    R[2] = A[0] * B[2] + A[1] * B[5] + A[2] * B[8];
    
    R[3] = A[3] * B[0] + A[4] * B[3] + A[5] * B[6];
    R[4] = A[3] * B[1] + A[4] * B[4] + A[5] * B[7];
    R[5] = A[3] * B[2] + A[4] * B[5] + A[5] * B[8];

    R[6] = A[6] * B[0] + A[7] * B[3] + A[8] * B[6];
    R[7] = A[6] * B[1] + A[7] * B[4] + A[8] * B[7];
    R[8] = A[6] * B[2] + A[7] * B[5] + A[8] * B[8];
}

void matrix_product44(double *A, double *B, double *R)
{
    R[0] = A[0] * B[0] + A[1] * B[4] + A[2] * B[8] + A[3] * B[12];
    R[1] = A[0] * B[1] + A[1] * B[5] + A[2] * B[9] + A[3] * B[13];
    R[2] = A[0] * B[2] + A[1] * B[6] + A[2] * B[10] + A[3] * B[14];
    R[3] = A[0] * B[3] + A[1] * B[7] + A[2] * B[11] + A[3] * B[15];
    
    R[4] = A[4] * B[0] + A[5] * B[4] + A[6] * B[8] + A[7] * B[12];
    R[5] = A[4] * B[1] + A[5] * B[5] + A[6] * B[9] + A[7] * B[13];
    R[6] = A[4] * B[2] + A[5] * B[6] + A[6] * B[10] + A[7] * B[14];
    R[7] = A[4] * B[3] + A[5] * B[7] + A[6] * B[11] + A[7] * B[15];

    R[8] = A[8] * B[0] + A[9] * B[4] + A[10] * B[8] + A[11] * B[12];
    R[9] = A[8] * B[1] + A[9] * B[5] + A[10] * B[9] + A[11] * B[13];
    R[10] = A[8] * B[2] + A[9] * B[6] + A[10] * B[10] + A[11] * B[14];
    R[11] = A[8] * B[3] + A[9] * B[7] + A[10] * B[11] + A[11] * B[15];

    R[12] = A[12] * B[0] + A[13] * B[4] + A[14] * B[8] + A[15] * B[12];
    R[13] = A[12] * B[1] + A[13] * B[5] + A[14] * B[9] + A[15] * B[13];
    R[14] = A[12] * B[2] + A[13] * B[6] + A[14] * B[10] + A[15] * B[14];
    R[15] = A[12] * B[3] + A[13] * B[7] + A[14] * B[11] + A[15] * B[15];
}

void matrix_product121(double *A, double *b, double *r)
{
    r[0] = A[0] * b[0] + A[1] * b[1];
}

void matrix_product131(double *A, double *b, double *r)
{
    r[0] = A[0] * b[0] + A[1] * b[1] + A[2] * b[2];
}

void matrix_product331(double *A, double *b, double *r)
{
    r[0] = A[0] * b[0] + A[1] * b[1] + A[2] * b[2];
    r[1] = A[3] * b[0] + A[4] * b[1] + A[5] * b[2];
    r[2] = A[6] * b[0] + A[7] * b[1] + A[8] * b[2];
}

void matrix_product341(double *A, double *b, double *r)
{
    r[0] = A[0] * b[0] + A[1] * b[1] + A[2] * b[2] + A[3] * b[3];
    r[1] = A[4] * b[0] + A[5] * b[1] + A[6] * b[2] + A[7] * b[3];
    r[2] = A[8] * b[0] + A[9] * b[1] + A[10] * b[2] + A[11] * b[3];
}

void matrix_product441(double *A, double *b, double *r)
{
    r[0] = A[0] * b[0] + A[1] * b[1] + A[2] * b[2] + A[3] * b[3];
    r[1] = A[4] * b[0] + A[5] * b[1] + A[6] * b[2] + A[7] * b[3];
    r[2] = A[8] * b[0] + A[9] * b[1] + A[10] * b[2] + A[11] * b[3];
    r[3] = A[12] * b[0] + A[13] * b[1] + A[14] * b[2] + A[15] * b[3];
}

void matrix_transpose_product_old(int Am, int An, int Bm, int Bn, 
                                  double *A, double *B, double *R) 
{
    int r = An;
    int c = Bn;
    int m = Am;

    int i, j, k;

    if (Am != Bm) {
        printf("Error: the number of rows of A and the "
               "number of rows of B must be equal\n");
        return;
    }

    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[k * An + i] * B[k * Bn + j];
            }
        }
    }
}

/* Compute the matrix product R = A^T B */
void matrix_transpose_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int r = An;
    int c = Bn;
    int m = Am;

//#if defined(WIN32) || defined(NO_CBLAS)
    int i, j, k;
//#endif

    if (Am != Bm) {
        printf("Error: the number of rows of A and the "
               "number of rows of B must be equal\n");
        return;
    }

//#if !defined(WIN32) && !defined(NO_CBLAS)
//    cblas_dgemm_driver_transpose(r, m, c, A, B, R);
//#else
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[k * An + i] * B[k * Bn + j];
            }
        }
    }
//#endif
}

/* Compute the matrix product R = A B^T */
void matrix_transpose_product2_old(int Am, int An, int Bm, int Bn, 
                                   double *A, double *B, double *R) 
{
    int r = Am;
    int c = Bm;
    int m = An;

    int i, j, k;

    if (An != Bn) {
        printf("Error: the number of columns of A and the "
               "number of columns of B must be equal\n");
        return;
    }
    
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[i * An + k] * B[j * Bn + k];
            }
        }
    }
}

/* Compute the matrix product R = A B^T */
void matrix_transpose_product2(int Am, int An, int Bm, int Bn, double *A, double *B, double *R) {
    int r = Am;
    int c = Bm;
    int m = An;

//#if defined(WIN32) || defined(NO_CBLAS)
    int i, j, k;
//#endif

    if (An != Bn) {
        printf("Error: the number of columns of A and the "
               "number of columns of B must be equal\n");
        return;
    }
    
//#if !defined(WIN32) && !defined(NO_CBLAS)
//    cblas_dgemm_driver_transpose2(r, m, c, A, B, R);
//#else
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            R[i * c + j] = 0.0;
            for (k = 0; k < m; k++) {
                R[i * c + j] += A[i * An + k] * B[j * Bn + k];
            }
        }
    }
//#endif
}

/* Return the product x**T A x */
double matrix_double_product(int n, double *A, double *x) 
{
    double *tmp = (double*)malloc(sizeof(double) * n);
    double result;
    
    matrix_product(n, n, n, 1, A, x, tmp);
    matrix_product(1, n, n, 1, x, tmp, &result);

    free(tmp);

    return result;
}




/* Compute the determinant of a 3x4 matrix */
double matrix_determinant3(double *A)
{
#if 0
    double *Q = (double *) malloc(sizeof(double) * n * n);
    double *R = (double *) malloc(sizeof(double) * n * n);
    dgeqrf_driver(n, n, A, Q, R);
#endif

    /* 0 1 2
     * 3 4 5
     * 6 7 8 */

    return 
	A[0] * (A[4] * A[8] - A[5] * A[7]) -
	A[1] * (A[3] * A[8] - A[5] * A[6]) +
	A[2] * (A[3] * A[7] - A[4] * A[6]);
}

/* Get the norm of the matrix */
double matrix_norm(int m, int n, double *A) {
    double sum = 0.0;
    int i;
    int entries = m * n;

    for (i = 0; i < entries; i++) {
	sum += A[i] * A[i];
    }
    
    return sqrt(sum);
}

/* Get the [squared] norm of the matrix */
double matrix_normsq(int m, int n, double *A) 
{
    double sum = 0.0;
    int i;
    int entries = m * n;

    for (i = 0; i < entries; i++) {
	sum += A[i] * A[i];
    }

    return sum;
}

/* Cross two 3x1 vectors */
void matrix_cross(const double *u, const double *v, double *w) {
    w[0] = u[1] * v[2] - u[2] * v[1];
    w[1] = u[2] * v[0] - u[0] * v[2];
    w[2] = u[0] * v[1] - u[1] * v[0];
}

/* Create the 3x3 cross product matrix from a 3-vector */
void matrix_cross_matrix(double *v, double *v_cross) {
    v_cross[0] = 0.0;   v_cross[1] = -v[2]; v_cross[2] = v[1];
    v_cross[3] = v[2];  v_cross[4] = 0.0;   v_cross[5] = -v[0];
    v_cross[6] = -v[1]; v_cross[7] = v[0];  v_cross[8] = 0.0;
}

/* Cross three 4x1 vectors */
void matrix_cross4(const double *u, const double *v, const double *w, 
		   double *x) 
{
    double sub1[9] = 
	{ u[1], u[2], u[3],
	  v[1], v[2], v[3],
	  w[1], w[2], w[3] };
    	
    double sub2[9] = 
	{ u[0], u[2], u[3],
	  v[0], v[2], v[3],
	  w[0], w[2], w[3] };

    double sub3[9] = 
	{ u[0], u[1], u[3],
	  v[0], v[1], v[3],
	  w[0], w[1], w[3] };

    double sub4[9] = 
	{ u[0], u[1], u[2],
	  v[0], v[1], v[2],
	  w[0], w[1], w[2] };

    double det1 = matrix_determinant3(sub1);
    double det2 = matrix_determinant3(sub2);
    double det3 = matrix_determinant3(sub3);
    double det4 = matrix_determinant3(sub4);

    x[0] = det1;
    x[1] = det2;
    x[2] = det3;
    x[3] = det4;
}

/* Scale a matrix by a scalar */
void matrix_scale(int m, int n, double *A, double s, double *R) {
    int i;
    int entries = m * n;
    
    for (i = 0; i < entries; i++) {
	R[i] = A[i] * s;
    }
}

/* Print the given n x m matrix */
void matrix_print(int m, int n, double *A) {
    int i, j;
    
    for (i = 0; i < m; i++) {
        printf("  ");
        for (j = 0; j < n; j++) {
            printf(" %0.6e ", A[i * n + j]);
        }
        printf("\n");
    }
}

/* Read a matrix from a file */
void matrix_read_file(int m, int n, double *matrix, char *fname) {
    FILE *f = fopen(fname, "r");
    int i;

    if (f == NULL) {
	printf("[matrix_read_file] Error reading matrix %s\n", fname);
	return;
    }

    for (i = 0; i < m * n; i++) {
	fscanf(f, "%lf", matrix + i);
    }
    
    fclose(f);
}

/* Write a matrix to a file */
void matrix_write_file(int m, int n, double *matrix, char *fname) {
    FILE *f = fopen(fname, "w");
    int i, j, idx;

    if (f == NULL) {
	printf("[matrix_write_file] Error writing matrix to %s\n", fname);
	return;
    }

    idx = 0;
    for (i = 0; i < m; i++) {
	for (j = 0; j < n; j++, idx++) {
	    fprintf(f, "%0.16e ", matrix[idx]);
	}
	fprintf(f, "\n");
    }

    fclose(f);
}


/* Convert a rotation matrix to axis and angle representation */
void matrix_to_axis_angle(double *R, double *axis, double *angle) {
    double d1 = R[7] - R[5];
    double d2 = R[2] - R[6];
    double d3 = R[3] - R[1];

    double norm = sqrt(d1 * d1 + d2 * d2 + d3 * d3);
    double x = (R[7] - R[5]) / norm;
    double y = (R[2] - R[6]) / norm;
    double z = (R[3] - R[1]) / norm;

    *angle = acos((R[0] + R[4] + R[8] - 1.0) * 0.5);

    axis[0] = x;
    axis[1] = y;
    axis[2] = z;
}

void axis_angle_to_matrix(double *axis, double angle, double *R) {
    double ident[9];
    double n[9] = { 0.0, -axis[2], axis[1],
		    axis[2], 0.0, -axis[0],
		    -axis[1], axis[0], 0.0 };

    double nsq[9], sn[9], cnsq[9], tmp[9];

    double c, s;
    
    c = cos(angle);
    s = sin(angle);

    matrix_ident(3, ident);
    matrix_product33(n, n, nsq);
    matrix_scale(3, 3, n, s, sn);
    matrix_scale(3, 3, nsq, (1 - c), cnsq);

    matrix_sum(3, 3, 3, 3, ident, sn, tmp);
    matrix_sum(3, 3, 3, 3, tmp, cnsq, R);
}

void axis_angle_to_matrix4(double *axis, double angle, double *R) {
    double ident[9];
    double n[9] = { 0.0, -axis[2], axis[1],
		    axis[2], 0.0, -axis[0],
		    -axis[1], axis[0], 0.0 };

    double nsq[9], sn[9], cnsq[9], tmp[9];
    double R3[9];

    double c, s;
    
    c = cos(angle);
    s = sin(angle);

    matrix_ident(3, ident);
    matrix_product33(n, n, nsq);
    matrix_scale(3, 3, n, s, sn);
    matrix_scale(3, 3, nsq, (1 - c), cnsq);

    matrix_sum(3, 3, 3, 3, ident, sn, tmp);
    matrix_sum(3, 3, 3, 3, tmp, cnsq, R3);

    matrix_ident(4, R);
    memcpy(R, R3, 3 * sizeof(double));
    memcpy(R + 4, R3 + 3, 3 * sizeof(double));
    memcpy(R + 8, R3 + 6, 3 * sizeof(double));
}



/* Convert a matrix to a normalize quaternion */
void matrix_to_quaternion(double *R, double *q) {
    double n4; // the norm of quaternion multiplied by 4 
    double tr = R[0] + R[4] + R[8]; // trace of martix
    double factor;

    if (tr > 0.0) {
        q[1] = R[5] - R[7];
        q[2] = R[6] - R[2];
        q[3] = R[1] - R[3];
        q[0] = tr + 1.0;
        n4 = q[0];
    } else if ((R[0] > R[4]) && (R[0] > R[8])) {
        q[1] = 1.0 + R[0] - R[4] - R[8];
        q[2] = R[3] + R[1];
        q[3] = R[6] + R[2];
        q[0] = R[5] - R[7];
        n4 = q[1];
    } else if (R[4] > R[8]) {
        q[1] = R[3] + R[1];
        q[2] = 1.0 + R[4] - R[0] - R[8];
        q[3] = R[7] + R[5];
        q[0] = R[6] - R[2]; 
        n4 = q[2];
    } else {
        q[1] = R[6] + R[2];
        q[2] = R[7] + R[5];
        q[3] = 1.0 + R[8] - R[0] - R[4];
        q[0] = R[1] - R[3];
        n4 = q[3];
    }

    factor = 0.5 / sqrt(n4);
    q[0] *= factor;
    q[1] *= factor;
    q[2] *= factor;
    q[3] *= factor;
}

/* Convert a normalized quaternion to a matrix */
void quaternion_to_matrix(double *q, double *R) {
    double sqw = q[0] * q[0];
    double sqx = q[1] * q[1]; 
    double sqy = q[2] * q[2];
    double sqz = q[3] * q[3];

    double tmp1, tmp2;

    R[0] =  sqx - sqy - sqz + sqw; // since sqw + sqx + sqy + sqz = 1
    R[4] = -sqx + sqy - sqz + sqw;    
    R[8] = -sqx - sqy + sqz + sqw;

    tmp1 = q[1] * q[2];
    tmp2 = q[3] * q[0];

    R[1] = 2.0 * (tmp1 + tmp2);    
    R[3] = 2.0 * (tmp1 - tmp2);        

    tmp1 = q[1] * q[3];    
    tmp2 = q[2] * q[0];    

    R[2] = 2.0 * (tmp1 - tmp2);    
    R[6] = 2.0 * (tmp1 + tmp2);    

    tmp1 = q[2] * q[3];    
    tmp2 = q[1] * q[0];    

    R[5] = 2.0 * (tmp1 + tmp2);   
    R[7] = 2.0 * (tmp1 - tmp2);
}


void slerp(double *v1, double *v2, double t, double *v3)
{
    double dot, angle, sin1t, sint, sina;
    matrix_product(1, 3, 3, 1, v1, v2, &dot);

    angle = acos(CLAMP(dot, -1.0 + 1.0e-8, 1.0 - 1.0e-8));

    sin1t = sin((1.0-t) * angle);
    sint = sin(t * angle);
    sina = sin(angle);
    
    v3[0] = (sin1t / sina) * v1[0] + (sint / sina) * v2[0];
    v3[1] = (sin1t / sina) * v1[1] + (sint / sina) * v2[1];
    v3[2] = (sin1t / sina) * v1[2] + (sint / sina) * v2[2];
}
