/*********************************************************/
/* date: 2011-4-7 */
/* author: zhangzhipeng */
/* paper: <High-Quality Single-Shot Capture of Facial Geometry> */
/*        <A taxonomy and evaluation of dense two-frame stereo correspondence algorithms> */


#pragma once
#include <QPointF>
#include <QHash>

#include <cvaux.h>
#include <highgui.h>
//#include <QImage>

#include "CameraParam.h"

#include <VECTOR>


//����Ϊ 3*3
//#define N_WINDOWS 3
//static int  N_WINDOWS = 3;
//NCC ������ С�ڸ�ֵ��ȫ������
//static double  NCC_Threshold = -0.5;

//refine step ʹ�� ������⻬��
static double WS = 0.005;

struct ImagePair
{
	
	IplImage* m_pImage[2]; //ԭʼͼ��
	IplImage* m_pMask[2]; //maskͼ��

};
//pyramid image
struct PyrImagePair
{
	int level;//����
	IplImage** m_pImage[2]; //ԭʼͼ�� pyramid
	IplImage** m_pMask[2]; //maskͼ�� pyramid

};


//hash
inline uint qHash(const CvPoint& p)
{
   return qHash(p.y*1600+p.x);
}
inline bool  operator== ( const CvPoint & p1, const CvPoint & p2 ) 
{
	if(p1.x==p2.x&&p1.y==p2.y)
		return true;
	return false;
}


typedef QHash<CvPoint, float> Disparity;   


class PixelMatching
{
public:
	PixelMatching(void);
	~PixelMatching(void);

	void init(int index);

	void init(int a, int b);
	//void Match(int index);//ƥ��
	void clear();//��������

	void FindBestMatch(int level); //ƥ��
	//д3D�㵽ply�ļ���
	//void WritePly(int index=-1);//index=-1 ��ʾд�����ļ�
	void PointRemove(int level, float percent);//ɾ������

private:
	//N*N windows Ĭ��N=3 NCCֵ����

	void grabTex( IplImage* pImage,  CvPoint point, std::vector<float>& tex, int N);

	//��ȡָ���е�mask ��Χ
	void getMask(IplImage* pMask, int y, int &Min_x, int &Max_x);

	//��������
	bool LoadData(int index);
	
	bool LoadData(int a, int b);

	//���ƥ��
	//void FindBestMatch();  
	
	//���Լ���ԣ� �⻬�� Ψһ�� ������
	void check_Constraints(int level);

	void check_Smoothness(int level);
	void check_Uniqueness(int level);
	void check_Ordering(int level);

	//��ƥ��
	void rematch(int level);

	//Disparity Map Refinement
	void DisparityMap_Refine(int level);

	//������
	void saveDisparity(int level);

	//��ȡ��һ��ƥ����, index:�������ҷ���
	void getSearchZone(CvPoint point, int level, int index, int &Min_x, int&Max_x);

	/* Triangulate two points */
	void Triangulate(CvPoint2D32f p, CvPoint2D32f q,  CvPoint3D32f &point3d, int level);

	// Triangulate all points
	void TriangulateAll(int level);

	//get point3d color
	void getPoint3dColor(CvPoint2D32f p, CvPoint2D32f q, CvPoint3D32f &color, int level);

	

	
public:
	std::string m_DirName;//Ĭ�� data/Rectify
	int m_PairwiseNum;//�����
	//static int N_WINDOWS;

	std::vector<CvPoint3D32f> *m_points;
	std::vector<CvPoint3D32f> *m_colors;
	std::vector<CvPoint3D32f> *m_normals;
	

	int  N_WINDOWS;
//NCC ������ С�ڸ�ֵ��ȫ������
    double  NCC_Threshold;

	PyrImagePair m_pPyrImagePair; 
//private:
	CameraParam  m_CameraParam[2];//�������
	Disparity **m_Disparity[2];
	Disparity  *missMatch_Disparity[2];
	

};
