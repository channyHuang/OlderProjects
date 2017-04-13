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

/* matrix.h */
/* Various linear algebra routines */

#ifndef __matrix_h__
#define __matrix_h__

#ifdef __cplusplus
extern "C" {
#endif

/* Fill a given matrix with an n x n identity matrix */
void matrix_ident(int n, double *A);

/* Fill a given matrix with an m x n matrix of zeroes */
void matrix_zeroes(int m, int n, double *A);
    
/* Transpose the m x n matrix A and put the result in the n x m matrix AT */
void matrix_transpose(int m, int n, double *A, double *AT);

/* Compute the matrix product R = AB */
void matrix_product(int Am, int An, int Bm, int Bn, 
                    const double *A, const double *B, double *R);

void matrix_product_old(int Am, int An, int Bm, int Bn, 
                        const double *A, const double *B, double *R);
void matrix_transpose_product_old(int Am, int An, int Bm, int Bn, 
                                  double *A, double *B, double *R);
void matrix_transpose_product2_old(int Am, int An, int Bm, int Bn, 
                                   double *A, double *B, double *R);

void matrix_product_ipp(int Am, int An, int Bn, 
                        const double *A, const double *B, double *R);
void matrix_transpose_product_ipp(int Am, int An, int Bn, 
                                  const double *A, const double *B, double *R);
void matrix_transpose_product2_ipp(int Am, int An, int Bm, 
                                   const double *A, const double *B, 
                                   double *R);
void matrix_array_product_ipp(int count, int Am, int An, int Bn,
                              const double *A, const double *B, double *R);

void matrix_product33(double *A, double *B, double *R);
void matrix_product121(double *A, double *b, double *r);
void matrix_product131(double *A, double *b, double *r);
void matrix_product331(double *A, double *b, double *r);
void matrix_product341(double *A, double *b, double *r);    
void matrix_product44(double *A, double *B, double *R);
void matrix_product441(double *A, double *b, double *r);
    
/* Compute the power of a matrix */
void matrix_power(int n, double *A, int pow, double *R);

/* Compute the matrix product R = A B^T */
void matrix_transpose_product2(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the matrix sum R = A + B */
void matrix_sum(int Am, int An, int Bm, int Bn, 
                double *A, double *B, double *R);

/* Compute the matrix difference R = A - B */
void matrix_diff(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the determinant of a 3x3 matrix */
double matrix_determinant3(double *A);

/* Compute the matrix product R = A^T B */
void matrix_transpose_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);


    

/* Invert the n-by-n matrix A, storing the result in Ainv */
void matrix_invert(int n, double *A, double *Ainv);
void matrix_invert_inplace(int n, double *A);
    
/* Get the norm of the matrix */
double matrix_norm(int m, int n, double *A);

/* Get the [squared] norm of the matrix */
double matrix_normsq(int m, int n, double *A);

/* Scale a matrix by a scalar */
void matrix_scale(int m, int n, double *A, double s, double *R);

/* Print the given m x n matrix */
void matrix_print(int m, int n, double *A);

/* Read a matrix from a file */
void matrix_read_file(int m, int n, double *matrix, char *fname);

/* Write a matrix to a file */
void matrix_write_file(int m, int n, double *matrix, char *fname);

/* Return the product x**T A x */
double matrix_double_product(int n, double *A, double *x);

/* Compute the cross product of two 3 x 1 vectors */
void matrix_cross(const double *u, const double *v, double *w);
void matrix_cross4(const double *u, const double *v, const double *w, 
		   double *x);
    
/* Create the 3x3 cross product matrix from a 3-vector */
void matrix_cross_matrix(double *v, double *v_cross);

/* Convert a rotation matrix to axis and angle representation */
void matrix_to_axis_angle(double *R, double *axis, double *angle);
void axis_angle_to_matrix(double *axis, double angle, double *R);
void axis_angle_to_matrix4(double *axis, double angle, double *R);

/* Convert a matrix to a normalize quaternion */
void matrix_to_quaternion(double *R, double *q);
/* Convert a normalized quaternion to a matrix */
void quaternion_to_matrix(double *q, double *R);
    

/* Find the unit vector that minimizes ||Ax|| */
void matrix_minimum_unit_norm_solution(int m, int n, double *A, double *x);

void slerp(double *v1, double *v2, double t, double *v3);
    
#ifdef __cplusplus
}
#endif

#endif /* __matrix_h__ */
