#pragma once
#include <fstream>

using namespace std;

#include "matrix.h"

/* Project a point onto an image */
static bool project(double *m_Pmatrix, double *p, double *proj) 
{
	double p4[4] = { p[0], p[1], p[2], 1.0 };
	double proj3[3];

	matrix_product(3, 4, 4, 1, (double *) m_Pmatrix, p4, proj3);

	if (proj3[2] == 0.0)
		return false;

	proj[0] = proj3[0] / proj3[2];
	proj[1] = proj3[1] / proj3[2];

	return true;
}

class CameraParam
{
public:
	CameraParam();
	~CameraParam();
	bool LoadCameraParam(string filename);
	bool IsLoadParam(){return isLoad;};
	bool SaveCameraParam(string filename);


	/* Compute projection matrix */
	void Finalize();
		 
	/*计算相机的朝向*/
	void ComputeCameraDirection();

	  /* Return the position of the camera */
    void inline GetPosition(double *pos) const {
        pos[0] = m_R[0] * m_T[0] + m_R[3] * m_T[1] + m_R[6] * m_T[2];
        pos[1] = m_R[1] * m_T[0] + m_R[4] * m_T[1] + m_R[7] * m_T[2];
        pos[2] = m_R[2] * m_T[0] + m_R[5] * m_T[1] + m_R[8] * m_T[2];

        pos[0] = -pos[0];
        pos[1] = -pos[1];
        pos[2] = -pos[2];
    }
public:
	double distortion[4];//畸变系数
	
	double m_K[9];//内参数矩阵，顺序：00,01,02,10,11,12,20,21,22
	double m_T[3];  //平移向量，t1,t2,t3
	double m_R[9];    //旋转矩阵，顺序：00,01,02,10,11,12,20,21,22

	double m_Pmatrix[12];//投影矩阵

	double frontDirection[3];	// 视线方向

	//Matrix3f cameraMatrix;	//内参矩阵
	//Vector3f transVects;//平移向量
	//Matrix3f rotMatrs;//旋转矩阵
	int CamNumber;//摄像机编号
	bool isLoad;//是否已经读入参数标志位
};
