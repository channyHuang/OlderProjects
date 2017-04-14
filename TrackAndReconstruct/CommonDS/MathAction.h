#pragma once


#include "MatrixGao.h"
//#include "Point3D.h"
class MathAction
{
public:
	MathAction(void);
public:
	~MathAction(void);
public:
	void CrossProduct(float x1, float y1 , float z1 , float x2 , float y2 , float z2 , float & x3 , float & y3 , float & z3);

	//vector 1 is the original vector and the second vector 2 is the axis the third one is the result.
	void RotateVector(float x1, float y1 , float z1 , float x2 , float y2 , float z2 , float theta, float & x3 , float & y3 , float & z3);

public:
	float DotProduct(float x1, float y1, float z1, float x2, float y2, float z2);
public:
	float Normalize(float &x1, float &y1, float &z1);
public:
	float BsplineBasis(int i, int k, float t);
public:
	//template <class T>
	bool LUcmp(MatrixGao<float>& A, int n, int* indx, float * d);
	//template <class T>
	const vector<float> MatrixMutiplyVector(MatrixGao<float> A, vector<float> x);
	//template <class T>
	const vector<float> VectorMutiplyMatrix(vector<float> x,MatrixGao<float> A);

public:
	void OrdinateConvert1to2(float p[3], float q[9]);
	//template <class T>
	void LUcmpSolve(MatrixGao<float> A, float b[]);
	void LUcmpSolve_FromLUM(MatrixGao<float> A, int * indx, float b[]);
	//template <class T>
	const MatrixGao<float> MatrixInverse(MatrixGao<float> A);
public:
	//template <class T>
	MatrixGao<float> MutiplyMatrix(MatrixGao<float> A, MatrixGao<float> B);
	MatrixGao<float> AddMatrix(MatrixGao<float> A, MatrixGao<float> B);
public:
	float Distance3D(float x1, float y1, float z1, float x2, float y2, float z2);
public:
	vector<float> LeastSquareSollve(MatrixGao<float> A, int n ,float* b);
public:
	void jacobiEigenSystem(MatrixGao<float> A, int n, float* d, MatrixGao<float>&  v, int* nrot);

	//void ComputeRigidParameter(vector<Point3D>& arrayL,vector<Point3D>& arrayR,float translate[3],float rotate[3][3]);// calculate the transformation for the best registration of two group vertex


private:
	//void InitData(vector<Point3D>& arrayL,vector<Point3D>& arrayR,vector<Point3D>& arrayL1,
	//vector<Point3D>& arrayR1,Point3D& averageL,Point3D& averageR,size_t numpoints);
	//float ComputeScale(vector<Point3D>& arrayL1,vector<Point3D>& arrayR1,size_t numpoints)
	;
	//float ComputeEigenvalue(vector<Point3D>& arrayL1,vector<Point3D>& arrayR1,size_t numpoints,float eigenvector[4]);
	void ComputeRotateMatrix(float rotate[3][3],float eigenvector[4]);
	//Point3D ComputeTranslate(float translate[3],float scale,float rotate[3][3],
	//Point3D& averageL,Point3D& averageR);

	void eigsrt(float d[] , MatrixGao<float>v,int n);
public:	
	float Distance3D_PnttoFace(float A, float B, float C, float D, float x, float y, float z);
};
