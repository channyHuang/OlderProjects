#pragma once
#include "MathAction.h"
#include "SpotMath.h"

using namespace std;

class CCalibrateParameters
{
public:
	CCalibrateParameters(void);
	~CCalibrateParameters(void);

public:
	float distortion[4];
	float cameraMatrix[9];//内参数矩阵，顺序：00,01,02,10,11,12,20,21,22
	float transVects[3];  //平移向量，t1,t2,t3
	float rotMatrs[9];    //旋转矩阵，顺序：00,01,02,10,11,12,20,21,22
	CPoint3D eyePose;
	float rotMarsInverse[9];
	float f;

public:
	CCalibrateParameters& operator= (const CCalibrateParameters& rhs)
	{
		for (int i=0;i<4;i++)
			distortion[i] = rhs.distortion[i];
		for (int i=0;i<9;i++)
			cameraMatrix[i] = rhs.cameraMatrix[i];
		for (int i=0;i<3;i++)
			transVects[i] = rhs.transVects[i];
		for (int i=0;i<9;i++)
			rotMatrs[i] = rhs.rotMatrs[i];

		return *this;
	}
	void computeEyePos()
	{
		MatrixGao<float> rotMat(3,3);
		MatrixGao<float> Inverse_rotMat(3,3);
		vector<float> transMat;

		for (int i = 0;i < 3;i++)
		{
			for (int j = 0;j < 3;j++)
			{
				rotMat[i][j] = rotMatrs[i*3+j];//A的R
			}
		}
		for (int i = 0;i < 3;i++)
		{
			transMat.push_back(transVects[i]);
		}

		MathAction worker;
		Inverse_rotMat=worker.MatrixInverse(rotMat);

		for (int i = 0;i < 3;i++)
		{
			for (int j = 0;j < 3;j++)
			{
				rotMarsInverse[i*3+j] = Inverse_rotMat[i][j];
			}
		}

		vector<float> result=worker.MatrixMutiplyVector(Inverse_rotMat,transMat);

		eyePose = CPoint3D(-result[0],-result[1],-result[2]);
		//Rx+T=0
	}
};

