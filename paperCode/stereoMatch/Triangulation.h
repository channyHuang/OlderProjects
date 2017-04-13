#pragma once

#include <vnl/vnl_least_squares_function.h> 
#include <vnl/algo/vnl_levenberg_marquardt.h> 
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_matrix.h> 
#include <vnl/algo/vnl_matrix_inverse.h> 

#include "CameraParam.h"

//vnl_least_squares_function
static  double global_p[2];
static  double global_q[2];

static double *globa_Pmatrix0;//投影矩阵
static double *globa_Pmatrix1;//投影矩阵


class triangulation_residual: public vnl_least_squares_function
{
public:
	triangulation_residual::triangulation_residual():vnl_least_squares_function(3, 4, no_gradient)
	{
		/*unsigned int number_of_unknowns = 3;
		unsigned int number_of_residuals = 4;
		vnl_least_squares_function(number_of_unknowns,  number_of_residuals);*/
	}

	void f(vnl_vector<double> const& x, vnl_vector<double>& fx)
	{
		double _x[3], p[2], q[2];

		_x[0] = x[0];
		_x[1] = x[1];
		_x[2] = x[2];

		project(globa_Pmatrix0, _x, p);
		project(globa_Pmatrix1, _x, q);

		fx[0] =  global_p[0] - p[0];
		fx[1] =  global_p[1] - p[1];

		fx[2] =  global_q[0] - q[0];
		fx[3] =  global_q[1] - q[1];

		printf("fx: %lf  %lf %lf %lf\n", fx[0], fx[1], fx[2], fx[3]);
	}
};