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


//窗口为 3*3
//#define N_WINDOWS 3
//static int  N_WINDOWS = 3;
//NCC 的门限 小于该值的全部抛弃
//static double  NCC_Threshold = -0.5;

//refine step 使用 ，代表光滑度
static double WS = 0.005;

struct ImagePair
{
	
	IplImage* m_pImage[2]; //原始图像
	IplImage* m_pMask[2]; //mask图像

};
//pyramid image
struct PyrImagePair
{
	int level;//层数
	IplImage** m_pImage[2]; //原始图像 pyramid
	IplImage** m_pMask[2]; //mask图像 pyramid

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
	//void Match(int index);//匹配
	void clear();//清理数据

	void FindBestMatch(int level); //匹配
	//写3D点到ply文件中
	//void WritePly(int index=-1);//index=-1 表示写所有文件
	void PointRemove(int level, float percent);//删除点云

private:
	//N*N windows 默认N=3 NCC值计算

	void grabTex( IplImage* pImage,  CvPoint point, std::vector<float>& tex, int N);

	//获取指定行的mask 范围
	void getMask(IplImage* pMask, int y, int &Min_x, int &Max_x);

	//加载数据
	bool LoadData(int index);
	
	bool LoadData(int a, int b);

	//最佳匹配
	//void FindBestMatch();  
	
	//检查约束性： 光滑性 唯一性 有序性
	void check_Constraints(int level);

	void check_Smoothness(int level);
	void check_Uniqueness(int level);
	void check_Ordering(int level);

	//再匹配
	void rematch(int level);

	//Disparity Map Refinement
	void DisparityMap_Refine(int level);

	//测试用
	void saveDisparity(int level);

	//获取上一级匹配结果, index:代表左右方向
	void getSearchZone(CvPoint point, int level, int index, int &Min_x, int&Max_x);

	/* Triangulate two points */
	void Triangulate(CvPoint2D32f p, CvPoint2D32f q,  CvPoint3D32f &point3d, int level);

	// Triangulate all points
	void TriangulateAll(int level);

	//get point3d color
	void getPoint3dColor(CvPoint2D32f p, CvPoint2D32f q, CvPoint3D32f &color, int level);

	

	
public:
	std::string m_DirName;//默认 data/Rectify
	int m_PairwiseNum;//配对数
	//static int N_WINDOWS;

	std::vector<CvPoint3D32f> *m_points;
	std::vector<CvPoint3D32f> *m_colors;
	std::vector<CvPoint3D32f> *m_normals;
	

	int  N_WINDOWS;
//NCC 的门限 小于该值的全部抛弃
    double  NCC_Threshold;

	PyrImagePair m_pPyrImagePair; 
//private:
	CameraParam  m_CameraParam[2];//相机参数
	Disparity **m_Disparity[2];
	Disparity  *missMatch_Disparity[2];
	

};
