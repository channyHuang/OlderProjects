#include "CameraParam.h"


CameraParam::CameraParam(void)
{
	CamNumber = 1;
	isLoad=false;
	int DisCount=4;
	for (int i=0;i<DisCount;i++)
	{
		distortion[i]=0;
	}
	
}

CameraParam::~CameraParam(void)
{
}

bool CameraParam::LoadCameraParam(string filename)
{
	FILE* f = fopen( filename.c_str(), "r" );
	if (!f)
		return false ;

	//读入畸变系数
	for (int i=0;i<4;i++)	{
		distortion[i] = 0;
	}
	//读入相机内参矩阵
	//int MatrixRows=3,MatrixCols=3;
	int data[] = { 481.20, 0.0, 319.5,
		0.0, 480.0, 239.5,
		0.0, 0.0, 1.0 }; //摄像机内参K

	for (int i=0;i<9;i++)
	{
		m_K[i] = data[i];
	}
	//读入旋转矩阵
	for (int i = 0; i < 9; i++)
	{

		fscanf(f, "%lf", &m_R[i]);


	}
	for (int i=0;i<3;i++)
	{
		fscanf(f,"%lf",&m_T[i]);
	}
	
	fclose(f);

	Finalize();//计算投影矩阵
	isLoad=true;
	return true;
}
bool CameraParam::SaveCameraParam(string filename)
{
	FILE* f = fopen( filename.c_str(), "w" );
	if (!f)
		return false ;
	fprintf(f,"%d\n",CamNumber);

	//写入畸变系数
	for (int i=0;i<4;i++)
	{
		fprintf(f,"%lf\n",distortion[i]);
	}
	//写入相机内参矩阵

	for (int i=0;i<9;i++)
	{

		fprintf(f,"%f\n",m_K[i]);


	}
	//写入相机平移向量


	for (int i=0;i<3;i++)
	{
		fprintf(f,"%f\n",m_T[i]);
	}
	//写入旋转矩阵
	for (int i=0;i<9;i++)
	{

		fprintf(f,"%f\n",m_R[i]);

	}
	fclose(f);
	return true;
}
 /* Compute projection matrix */
void CameraParam::Finalize()
{
    double Ptmp[12] = 
	{ m_R[0], m_R[1], m_R[2], m_T[0],
	  m_R[3], m_R[4], m_R[5], m_T[1],
	  m_R[6], m_R[7], m_R[8], m_T[2] };
	    
     

    matrix_product(3, 3, 3, 4, m_K, Ptmp, m_Pmatrix);
}

/*计算相机的朝向*/
void CameraParam::ComputeCameraDirection()
{
	double tranposedRotationMatrix[9];
	matrix_transpose(3, 3, this->m_R, tranposedRotationMatrix);

	double defaultFrontDirection[3] = {0, 0, 1};
	matrix_product(3, 3, 3, 1, tranposedRotationMatrix, defaultFrontDirection, this->frontDirection);	
}