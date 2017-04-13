#include "PixelMatching.h"
#include "ImageProcess.h"
#include <vnl/vnl_least_squares_function.h> 
#include <vnl/algo/vnl_levenberg_marquardt.h> 
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_matrix.h> 
#include <vnl/algo/vnl_matrix_inverse.h> 

#include "ply.h"

#include <time.h>
#include "Triangulation.h"

#include "vector.h"



PixelMatching::PixelMatching(void)
{
	 m_DirName = "data";

	 m_PairwiseNum = 0;

	 m_Disparity[0] = NULL;
	 m_Disparity[1] = NULL;

	 N_WINDOWS = 3;
//NCC 的门限 小于该值的全部抛弃
     NCC_Threshold = -0.5;

	 m_points = NULL;
	 m_colors = NULL;
	 m_normals = NULL;
}

PixelMatching::~PixelMatching(void)
{
	
}
//清理数据
void PixelMatching::clear() 
{

	m_Disparity[0][0]->clear();
	m_Disparity[1][0]->clear();

	delete m_Disparity[0];
	delete m_Disparity[1];

	//释放数据

	for(int level=0; level<m_pPyrImagePair.level; level++)
	{
		if(m_points)
			m_points[level].clear();
		if(m_colors)
			m_colors[level].clear();

		if(m_normals)
			m_normals[level].clear();

	}
	
	
}
void PixelMatching::init(int index)
{
	if(index>=m_PairwiseNum)
	{
		printf("匹配序号超出\n");
		return;
	}


	//加载数据
	if(!LoadData(index))
	{
		printf("加载数据错\n");
		return;
	}

	m_points = new std::vector<CvPoint3D32f>[m_pPyrImagePair.level];
	m_colors = new std::vector<CvPoint3D32f>[m_pPyrImagePair.level];
	m_normals = new std::vector<CvPoint3D32f>[m_pPyrImagePair.level];
}

void PixelMatching::init(int a, int b) {
	//加载数据
	if (!LoadData(a, b))	{
		printf("加载数据错\n");
		return;
	}

	m_points = new std::vector<CvPoint3D32f>[m_pPyrImagePair.level];
	m_colors = new std::vector<CvPoint3D32f>[m_pPyrImagePair.level];
	m_normals = new std::vector<CvPoint3D32f>[m_pPyrImagePair.level];
}

//查找最佳匹配 winner-take-all block-matching algorithm
void PixelMatching::FindBestMatch(int level)
{
	CvPoint point[2];
	

	if(level==m_pPyrImagePair.level-1)
	{
		m_Disparity[0] = (Disparity**)calloc(m_pPyrImagePair.level, sizeof( Disparity* ));
		m_Disparity[1] = (Disparity**)calloc(m_pPyrImagePair.level, sizeof( Disparity* ));

		for(int level=0; level<m_pPyrImagePair.level; level++)
		{
			m_Disparity[0][level] = new Disparity();
			m_Disparity[1][level] = new Disparity();
		}
	}

	

	int N = 2*(m_pPyrImagePair.level-1-level + N_WINDOWS/2)+1;
	if(N>7)
		N = 7;
	

	printf("\n matching:  ");

	//int dd=0;
	for(int y=0; y<m_pPyrImagePair.m_pImage[0][level]->height; y++)
	{
		/*if(dd==0)
		{
		  printf("\b");
		  printf("*");
		}
		else
		{
		  printf("\b");
		  printf("|");

		  dd=-1;
		}
		dd++;*/
		//获取mask的搜索范围
		int Min_x[2], Max_x[2];
		getMask(m_pPyrImagePair.m_pMask[0][level], y, Min_x[0], Max_x[0]);
		getMask(m_pPyrImagePair.m_pMask[1][level], y, Min_x[1], Max_x[1]);


		if(Max_x[0]==0||Max_x[1]==0)
			continue;//该扫描线上没有前景物体

		point[0].y = y;
		point[1].y = y;

		//printf("y = %d \n",y);

		//0:从左往右匹配  1:从右往左匹配
		for(int index=0; index<2; index++) 
		{
			int t_index = (index+1)%2;

			//从 mask 限制范围进行搜索
			for(int x=Min_x[index]; x<=Max_x[index]; x++)
			{
				point[0].x = x;
				double MAX_NCC = NCC_Threshold; //小于该门限的拒绝 加入匹配结果
				int Match_x=-1;

				//从 上一级的匹配结果进行搜索 提高速度

				int Zone_min = Min_x[t_index];
				int Zone_max = Max_x[t_index];

				if(level!=m_pPyrImagePair.level-1)
				{
					getSearchZone(point[0], level, index, Zone_min, Zone_max);

					//printf("level=%d: (Min_x=%d , Max_x=%d) -- (Zone_min=%d, Zone_max=%d)\n", level, Min_x[t_index], Max_x[t_index], Zone_min, Zone_max);
				}

				std::vector<float> tex0, tex1;
				double ncc_value;

				grabTex( m_pPyrImagePair.m_pImage[index][level], point[0], tex0, N);
				Normalize(tex0);


				for(int x1=Zone_min; x1<=Zone_max; x1++)
				{
					point[1].x = x1;
					//ncc_value = NCC(m_pPyrImagePair.m_pImage[index][level], m_pPyrImagePair.m_pImage[t_index][level], point[0], point[1], N);
					grabTex( m_pPyrImagePair.m_pImage[t_index][level], point[1], tex1, N );
					Normalize(tex1);

					ncc_value = RGB_NCC(tex0, tex1);

					if(MAX_NCC<ncc_value)
					{
						MAX_NCC = ncc_value;
						Match_x = x1;
					}

				}

				if(Match_x!=-1)
				{
					float d = Match_x-x;
					//if(d<-1000)
					//	printf("[d=%f]\n", d);
					m_Disparity[index][level]->insert(point[0], d); //插入匹配对

				}

			}
		}

	}

	printf("\n");

	//检查约束性： 光滑性 唯一性 有序性
	check_Constraints( level);

	//refine disparity
	//DisparityMap_Refine(level);

	//调整级别
	if(level!=m_pPyrImagePair.level-1)
	{
		m_Disparity[0][level+1]->clear();
		m_Disparity[1][level+1]->clear();
		delete m_Disparity[0][level+1];
		delete m_Disparity[1][level+1];
	}

	//saveDisparity( level);

	//point2d ->point3d
	TriangulateAll(level);

//	cvReleaseImage(&m_pPyrImagePair.m_pImage[0][level]);
//	cvReleaseImage(&m_pPyrImagePair.m_pImage[1][level]);
	cvReleaseImage(&m_pPyrImagePair.m_pMask[0][level]);
	cvReleaseImage(&m_pPyrImagePair.m_pMask[1][level]);
	
}
//检查约束性： 光滑性 唯一性 有序性
void PixelMatching::check_Constraints( int level)
{
	//Disparity  *missMatch_Disparity[2];
	missMatch_Disparity[0] = new Disparity();//不满足条件的匹配
	missMatch_Disparity[1] = new Disparity();

	//检查约束性
	printf("check Constraints \n");

	printf("before check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	printf("before check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());
	check_Smoothness(level);

	int sum[2];	
	do
	{
		sum[0] = m_Disparity[0][level]->size();
		sum[1] = m_Disparity[1][level]->size();

		check_Uniqueness(level);
		//printf("after check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	   // printf("after check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());

		check_Ordering(level);
		//printf("after check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	   // printf("after check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());
		
	}while(sum[0]> m_Disparity[0][level]->size()|| sum[1]>m_Disparity[1][level]->size());
	//printf("after check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	//printf("after check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());
	
	/******************************************************************************/

	//rematch 不符合约束条件的
	rematch(level);
	printf("after rematch Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	printf("after rematch Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());

	check_Smoothness(level);
	sum[2];	
	do
	{
		sum[0] = m_Disparity[0][level]->size();
		sum[1] = m_Disparity[1][level]->size();

		check_Uniqueness(level);
		//printf("after check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	   // printf("after check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());

		check_Ordering(level);
	//	printf("after check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	 //   printf("after check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());
		
	}while(sum[0]> m_Disparity[0][level]->size()|| sum[1]>m_Disparity[1][level]->size());


	printf("after check Disparity[%d].count = %d\n", 0,  m_Disparity[0][level]->size());
	printf("after check Disparity[%d].count = %d\n", 1,  m_Disparity[1][level]->size());

	

	//释放资源
	missMatch_Disparity[0]->clear();
	missMatch_Disparity[1]->clear();
	delete missMatch_Disparity[0];
	delete missMatch_Disparity[1];
}
//检查约束性： 光滑性 
void PixelMatching::check_Smoothness(int level)
{
	//printf("1 check Smoothness Constraint\n");
	for(int index=0; index<2; index++) //index 0:left -> right  index 1: right->left
	{

		for(Disparity::iterator itr = m_Disparity[index][level]->begin();itr != m_Disparity[index][level]->end();itr++ )
		{
			CvPoint point0 = itr.key();
			float d = itr.value();
			
		
			int num = 0;
			int less = 0;

			CvPoint point;
			//3*3 窗口内检查 
			for(int y=-1; y<=1; y++)
			{
				point.y = point0.y + y;

				for(int x=-1; x<=1; x++)
				{
					point.x = point0.x + x;
					Disparity::iterator itr1 = m_Disparity[index][level]->find(point);
					if(itr1!=m_Disparity[index][level]->end())
					{
						
						num++;

						float d1 = itr1.value();
						if(abs(d1-d)<1)
							less++;

					}

				}
			}

			if(less<num/2)
			{//不满足条件

				missMatch_Disparity[index]->insert(point0, d);//添加到missmatch中
				//printf("not fill Smoothness Constraint\n");

				//m_Disparity[index][level]->erase(itr-1);
				
			}
		}



		//remove not fill constraints
		for(Disparity::iterator itr = missMatch_Disparity[index]->begin();itr != missMatch_Disparity[index]->end();itr++ )
		{
			CvPoint point0 = itr.key();
			m_Disparity[index][level]->remove(point0);

		}
	}
}
//检查约束性： 唯一性
void PixelMatching::check_Uniqueness(int level)
{
	Disparity *pDisparity[2];

	//printf("2 check Uniqueness Constraint\n");
	for(int index=0; index<2; index++) //index 0:left -> right  index 1: right->left
	{
		int t_index = (index+1)%2;

		pDisparity[0] = m_Disparity[index][level];
		pDisparity[1] = m_Disparity[t_index][level];


		for(Disparity::iterator itr = pDisparity[0]->begin();itr != pDisparity[0]->end();itr++ )
		{
			CvPoint point0 = itr.key();
			float d = itr.value();

			bool b_fill = false;

			for(int dx=-1; dx<=1; dx++)
			{
				int x = point0.x+d+dx;
				Disparity::iterator itr1 = pDisparity[1]->find(cvPoint(x, point0.y));

				if(itr1!= pDisparity[1]->end())
				{
					float d1 = itr1.value();

					int abs_dx = d+dx+d1;

					if(abs(abs_dx)<=1)
					{//满足条件
						/*if(level==0&&point0.y==num_y)
						   printf("point0=(%d, %d, %f)  point1=(%d, %d, %f)\n", point0.x, point0.y, d, x, point0.y, d1);*/
						b_fill = true;
						break;

					}

				}

			}

			if(!b_fill)
			{//不满足条件
				//printf("not fill Uniqueness Constraint\n");
				missMatch_Disparity[index]->insert(point0, d);
				
			}
		}

		//remove not fill constraints
		for(Disparity::iterator itr = missMatch_Disparity[index]->begin();itr != missMatch_Disparity[index]->end();itr++ )
		{
			CvPoint point0 = itr.key();
			m_Disparity[index][level]->remove(point0);

		}

	}

		
}
//检查约束性： 有序性
void PixelMatching::check_Ordering(int level)
{
	int width = m_pPyrImagePair.m_pImage[0][level]->width;
	//printf("3 check Ordering Constraint\n");
	for(int index=0; index<2; index++) //index 0:left -> right  index 1: right->left
	{
		
		for(Disparity::iterator itr = m_Disparity[index][level]->begin(); itr != m_Disparity[index][level]->end();itr++ )
		{
			CvPoint point0 = itr.key();
			float d = itr.value();


			int w = width - point0.x ;
			for(int dx=1; dx<w; dx++)
			{
				Disparity::iterator itr1 = m_Disparity[index][level]->find(cvPoint(point0.x+dx, point0.y));
				if(itr1!=m_Disparity[index][level]->end())
				{
					float d1 = itr1.value();
					if((d-dx-d1)>0)
					{//不满足条件
						missMatch_Disparity[index]->insert(point0, d);//添加到missmatch中
						//m_Disparity[index][level]->erase(itr-1);

						//printf("not fill Ordering Constraint\n");	

					}

					break;

				}
			}
		}

		////remove not fill constraints
		for(Disparity::iterator itr = missMatch_Disparity[index]->begin();itr != missMatch_Disparity[index]->end();itr++ )
		{
			CvPoint point0 = itr.key();
			m_Disparity[index][level]->remove(point0);

		}
	}
}
//对不满足三个约束条件的进行再次匹配
//确保了smooth and ordering constraint 下进行再次匹配
//匹配后的结果需要用 check uniqueness constraint
void PixelMatching::rematch(int level)
{
	int N = 2*(m_pPyrImagePair.level-1-level + N_WINDOWS/2)+1;
	if(N>9)
		N = 9;
	//int N = N_WINDOWS;

	printf("rematch \n");
	for(int index=0; index<2; index++)
	{
		printf("missMatch_Disparity[%d].count = %d\n", index,  missMatch_Disparity[index]->size());
		int t_index = (index+1)%2;

		
		for(Disparity::iterator itr = missMatch_Disparity[index]->begin();itr != missMatch_Disparity[index]->end();++itr )
		{
			CvPoint point0 = itr.key();
			CvPoint point1;
			point1.y = point0.y;

			//mask 范围
			int Min_x,Max_x;
			getMask(m_pPyrImagePair.m_pMask[index][level], point0.y, Min_x, Max_x);
			

			int x_L, x_R;

			bool bFind_L = false;
			bool bFind_R = false;

			//获取mask 范围
			//getMask(m_pPyrImagePair.m_pMask[t_index][level], point0.y, x_L, x_R);

			//找到符合约束的第一个左边像素			
			for(int x=point0.x-1; x>=Min_x;x--)
			{
				Disparity::iterator itr1 = m_Disparity[index][level]->find(cvPoint(x, point0.y));
				if( itr1!=m_Disparity[index][level]->end() )
				{
					float d = itr1.value();

					x_L = x + d;

					bFind_L = true;
					break;
				}
			}
		

			if(!bFind_L)
			{
				//没有找到，不进行搜索
				//printf("not find bFind_L limit\n");
				continue;
			}


			//找到符合约束的第一个右边像素
			for(int x=point0.x+1; x<=Max_x; x++)
			{
				Disparity::iterator itr1 = m_Disparity[index][level]->find(cvPoint(x, point0.y));
				if( itr1!=m_Disparity[index][level]->end() )
				{
					float d = itr1.value();

					x_R = x + d;

					bFind_R = true;
					break;
				}
			}

			if(!bFind_R)
			{
				//没有找到，不进行搜索
				//printf("not find bFind_R limit\n");
				continue;
			}



			double MAX_NCC = NCC_Threshold;//NCC  //小于该门限的拒绝 加入匹配结果
			int Match_x=-1;
			double ncc_value;


			 std::vector<float> tex0, tex1;

			 grabTex( m_pPyrImagePair.m_pImage[index][level], point0, tex0, N);
			 Normalize(tex0);

			//从 符合约束的左右像素 来确定 搜索范围
			for(int x=x_L; x<=x_R; x++)
			{
				point1.x = x;

				//ncc_value = NCC(m_pPyrImagePair.m_pImage[index][level], m_pPyrImagePair.m_pImage[t_index][level], point0, point1, N);
				grabTex( m_pPyrImagePair.m_pImage[t_index][level], point1, tex1, N);
				Normalize(tex1);

				ncc_value = RGB_NCC(tex0, tex1);

				if(MAX_NCC<ncc_value)
				{
					MAX_NCC = ncc_value;
					Match_x = x;
				}
			}

			if(Match_x!=-1)
			{
				//没有找到，不进行搜索
				//printf("find match\n");

				float d = Match_x-point0.x;
				m_Disparity[index][level]->insert(point0, d); //更新匹配对
		
			}
			//else
			//{
				//printf("ncc_value = %f \n", ncc_value);
			//}
			

		}


		missMatch_Disparity[index]->clear();
	}
}
//加载数据
bool PixelMatching::LoadData(int index)
{
	//获取相机参数
	for(int i=0; i<2; i++)
	{
		char path[1024];
		sprintf(path, "%s//Rectify//%.4d_%d.calrt", m_DirName.data(), index+1, i);
		if(!m_CameraParam[i].LoadCameraParam(path))
		{
			printf("load camera param error: %s !\n", path);
			return false;
		}
	}

	
	//获取图片信息
	ImagePair t_ImagePair;
	for(int i=0; i<2; i++)
	{
		char path[1024];
		//sprintf(path, "%s//Rectify//%.4d_%d.bmp", m_DirName.data(), index+1, i);
		sprintf(path, "%s//Rectify//%.4d_%d.jpg", m_DirName.data(), index + 1, i);

		IplImage* img = cvLoadImage(path);
		if(img==NULL)
		{
			printf("load pic error: %s !\n", path);
			return false;
		}
		
		t_ImagePair.m_pImage[i] = img;//图像不进行rgb->gray的转换
		

		sprintf(path, "%s//Rectify//%.4d_%d_mask.bmp", m_DirName.data(), index+1, i);

		img = cvLoadImage(path);
		
		if(img==NULL)
		{
			printf("load mask pic error: %s !\n", path);
			return false;
		}
		t_ImagePair.m_pMask[i] = img;//mask图像不进行rgb->gray的转换
		

	}
	//构建金字塔
	{
		int w = max(t_ImagePair.m_pImage[0]->width, t_ImagePair.m_pImage[0]->height);

		int level = 1;
		while(w>200)
		{
			w = w/2;
			level++;
		}

		m_pPyrImagePair.level = level;
		m_pPyrImagePair.m_pImage[0] = build_gauss_pyr( t_ImagePair.m_pImage[0], level );//rgb
		m_pPyrImagePair.m_pImage[1] = build_gauss_pyr( t_ImagePair.m_pImage[1], level );//rgb

		m_pPyrImagePair.m_pMask[0] = build_gauss_pyr( t_ImagePair.m_pMask[0], level );//rgb
		m_pPyrImagePair.m_pMask[1] = build_gauss_pyr( t_ImagePair.m_pMask[1], level );//rgb


		

	}
	//释放数据
	if(!t_ImagePair.m_pImage[0])
		cvReleaseImage(&t_ImagePair.m_pImage[0]);
	if(!t_ImagePair.m_pImage[1])
		cvReleaseImage(&t_ImagePair.m_pImage[1]);
	if(!t_ImagePair.m_pMask[0])
		cvReleaseImage(&t_ImagePair.m_pMask[0]);
	if(!t_ImagePair.m_pMask[1])
		cvReleaseImage(&t_ImagePair.m_pMask[1]);


	return true;
}

bool PixelMatching::LoadData(int a, int b) {
	//获取相机参数
	char path[1024];
	sprintf(path, "%s//newCalrt//%08d.txt", m_DirName.data(), a);
	if (!m_CameraParam[0].LoadCameraParam(path))	{
		printf("load camera param error: %s !\n", path);
		return false;
	}
	sprintf(path, "%s//newCalrt//%08d.txt", m_DirName.data(), b);
	if (!m_CameraParam[1].LoadCameraParam(path))	{
		printf("load camera param error: %s !\n", path);
		return false;
	}

	sprintf(path, "%s/K.txt", m_DirName.data());
	std::ifstream ifs(path);
	for (int i = 0; i < 9; i++) {
		ifs >> m_CameraParam[0].m_K[i];
		m_CameraParam[1].m_K[i] = m_CameraParam[0].m_K[i];
	}
	ifs.close();

	//获取图片信息
	ImagePair t_ImagePair;
	sprintf(path, "%s//pic//%08d.jpg", m_DirName.data(), a);
	IplImage* img = cvLoadImage(path);
	if (img == NULL)	{
		printf("load pic error: %s !\n", path);
		return false;
	}
	t_ImagePair.m_pImage[0] = img;//图像不进行rgb->gray的转换
	t_ImagePair.m_pMask[0] = cvCreateImage(cvSize(img->width, img->height), img->depth, 1); //mask图像不进行rgb->gray的转换

	sprintf(path, "%s//pic//%08d.jpg", m_DirName.data(), b);
	img = cvLoadImage(path);
	if (img == NULL)	{
		printf("load pic error: %s !\n", path);
		return false;
	}
	t_ImagePair.m_pImage[1] = img;//图像不进行rgb->gray的转换
	t_ImagePair.m_pMask[1] = cvCreateImage(cvSize(img->width, img->height), img->depth, 1); //mask图像不进行rgb->gray的转换

	for (int i = 0; i < t_ImagePair.m_pMask[0]->width; i++) {
		for (int j = 0; j < t_ImagePair.m_pMask[0]->height; j++) {
			t_ImagePair.m_pMask[0]->imageData[i*t_ImagePair.m_pMask[0]->height + j] = 255;
			t_ImagePair.m_pMask[1]->imageData[i*t_ImagePair.m_pMask[0]->height + j] = 255;
		}
	}
	
	//构建金字塔
	{
		int w = max(t_ImagePair.m_pImage[0]->width, t_ImagePair.m_pImage[0]->height);

		int level = 1;
		while (w > 200)
		{
			w = w / 2;
			level++;
		}

		m_pPyrImagePair.level = level;
		m_pPyrImagePair.m_pImage[0] = build_gauss_pyr(t_ImagePair.m_pImage[0], level);//rgb
		m_pPyrImagePair.m_pImage[1] = build_gauss_pyr(t_ImagePair.m_pImage[1], level);//rgb

		m_pPyrImagePair.m_pMask[0] = build_gauss_pyr(t_ImagePair.m_pMask[0], level);//rgb
		m_pPyrImagePair.m_pMask[1] = build_gauss_pyr(t_ImagePair.m_pMask[1], level);//rgb




	}
	//释放数据
	if (!t_ImagePair.m_pImage[0])
		cvReleaseImage(&t_ImagePair.m_pImage[0]);
	if (!t_ImagePair.m_pImage[1])
		cvReleaseImage(&t_ImagePair.m_pImage[1]);
	if (!t_ImagePair.m_pMask[0])
		cvReleaseImage(&t_ImagePair.m_pMask[0]);
	if (!t_ImagePair.m_pMask[1])
		cvReleaseImage(&t_ImagePair.m_pMask[1]);


	return true;
}

////计算NCC
////
////pImage0, pImage1 都是 32-bit grayscale image 
//double PixelMatching::NCC(IplImage* pImage0, IplImage* pImage1, CvPoint point0, CvPoint point1, int N)
//{
//	double NCC_Value = 0;
//
//	int x0 = point0.x;
//	int y0 = point0.y;
//
//	int x1 = point1.x;
//	int y1 = point1.y;
//
//	int WindowsSize = N*N;
//	double *v0 = new double[WindowsSize];
//	double *v1 = new double[WindowsSize];
//
//	double SUM_v0 = 0,  SUM_v1=0;
//	
//	for(int i=0; i<N; i++)
//	{
//		int _y0 = y0 +i - N/2;
//		int _y1 = _y0;
//		for(int j=0; j<N; j++)
//		{
//			int _x0 = x0 +j - N/2;
//			
//			int _x1 = x1 +j - N/2;
//			
//
//			if(_x0<0 || _x0>=pImage0->width || _y0<0 || _y0>=pImage0->height)
//				v0[i*N + j] = 0;
//			else 
//			{
//				int index =  _y0 * pImage0->widthStep + _x0 * pImage0->depth/8;
//				float* gray = (float*)(pImage0->imageData+index) ; //gray
//				
//				v0[i*N + j] = *gray;
//			}
//
//			if(_x1<0 || _x1>=pImage1->width || _y1<0 || _y1>=pImage1->height)
//				v1[i*N + j] = 0;
//			else 
//			{
//				int index =  _y1 * pImage1->widthStep + _x1 * pImage1->depth/8;
//				float* gray = (float*)(pImage1->imageData+index) ; //gray
//
//				v1[i*N + j] = *gray;
//			}
//
//			SUM_v0 += v0[i*N + j];
//			SUM_v1 += v1[i*N + j];
//
//
//		}
//	}
//
//	
//	double Mean_v0 = SUM_v0/WindowsSize; //平均值
//	double Mean_v1 = SUM_v1/WindowsSize;
//
//	double s = 0;
//	double t0 = 0; 
//	double t1 = 0;
//
//	for(int i=0; i<WindowsSize; i++)
//	{
//		s += (v0[i] - Mean_v0)*(v1[i] - Mean_v1);
//
//		t0 += (v0[i] - Mean_v0)*(v0[i] - Mean_v0);
//
//		t1 += (v1[i] - Mean_v1)*(v1[i] - Mean_v1);
//	}
//
//	if(t0==0||t1==0)
//	{
//		NCC_Value = -3;
//	}
//	else
//	{ 
//		NCC_Value = s / sqrt(t0*t1);
//	}
//
//
//	delete v0;
//	delete v1;
//
//	return NCC_Value;
//}
//

void PixelMatching::grabTex( IplImage* pImage,  CvPoint point, std::vector<float>& tex, int N)
{
	tex.clear();
	int x0 = point.x;
	int y0 = point.y;

	//int WindowsSize = N*N;

	int w_step = pImage->nChannels*pImage->depth/8;
	
	for(int i=0; i<N; i++)
	{
		int _y0 = y0 +i - N/2;
		
		for(int j=0; j<N; j++)
		{
			int _x0 = x0 +j - N/2;

			float r=0,g=0,b=0;

			if(_x0<0 || _x0>=pImage->width || _y0<0 || _y0>=pImage->height)
			{
				
			}	
			else 
			{
				int index = pImage->widthStep*_y0;
				uchar* imgPtr=(uchar*)(pImage->imageData + index +  _x0* w_step) ;
				r = (int)*imgPtr; 
				g = (int)*(imgPtr+1);
				b = (int)*(imgPtr+2);
			}

			tex.push_back(r);
			tex.push_back(g);
			tex.push_back(b);

		}
	}
}
//通过查找mask图像，来限制搜索范围
void PixelMatching::getMask(IplImage* pMask, int y, int &Min_x, int &Max_x)
{
	Min_x = Max_x = 0;

	int index = y*pMask->widthStep;
	int step = pMask->nChannels*pMask->depth/8;

	for(int x=0; x<pMask->width; x++)
	{
		if(pMask->imageData[index + x*step]!=0)
		{
			Min_x = x;
			break;
		}
	}

	for(int x=pMask->width-1; x>=0; x--)
	{
		if(pMask->imageData[index + x*step]!=0)
		{
			Max_x = x;
			break;
		}
	}

	//test 相等于没有mask

	/*Min_x = 20;
	Max_x = pMask->width-20;*/
}


//获取上一级匹配结果,来确定搜索范围 index=0代表左->右   1：代表右->左
void PixelMatching::getSearchZone(CvPoint point, int level, int index,int &Min_x, int&Max_x)
{
	try
	{
		int x = point.x;
		int y = point.y;

		int half_x = x/2;
		int half_y = y/2;

		int ini_dx = 0;

		if(x%2==0)
			ini_dx =1;

		int t_Min_x = Min_x, t_Max_x = Max_x;

		Disparity *pDisparity =  m_Disparity[index][level+1];

		//左边一个像素
		for(int dx=ini_dx; dx<=half_x; dx++)
		{
			Disparity::iterator itr =pDisparity->find(cvPoint(half_x-dx, half_y));
			if(itr!=pDisparity->end())
			{
				int d = itr.value();
				int t = (half_x+d)*2;

				if(Min_x<=t)
				{
					//printf("dx=%d Min_x=%d, half_x+d=%d\n", dx, Min_x, half_x+d);
					Min_x = t;
					
				}
				/*else
				{
				  printf("\n(x=%d, y=%d)  Min_x=%d, half_x*2+d=%d\n\n", x, y, Min_x, t);
				}*/
				break;
			}
		}

		//右边一个像素
		/*if(half_x<m_pPyrImagePair.m_pImage[index][level]->width-1)
			half_x +=1;*/

		int w = m_pPyrImagePair.m_pImage[index][level]->width/2;
		for(int dx=1; dx<w-half_x; dx++)
		{
			Disparity::iterator itr1 = pDisparity->find(cvPoint(half_x+dx, half_y));
			if(itr1!=pDisparity->end())
			{
				int d = itr1.value();
				int t = (half_x+d)*2;

				if(Max_x>=t)
				{
					//printf("dx=%d Max_x=%d, half_x+d=%d\n", dx, Max_x, half_x+d);
					Max_x = t;
					
				}
				/*else
				{
				  printf("\n(x=%d, y=%d)  Max_x=%d, half_x*2+d=%d\n\n", x, y, Max_x, t);
				}*/
				break;
			}
		}

		if(Min_x>Max_x)
		{//不改变搜索区域
			Min_x = t_Min_x;
			Max_x = t_Max_x;

			//printf("Min_x>Max_x\n");
		}
	}
	catch(...)
	{
	}
}
//Disparity Map Refinement
void PixelMatching::DisparityMap_Refine(int level)
{
	float d=0, d_left=0, d_right=0,d_up=0,d_down=0;
	double e_left, e_zero, e_right;

	double ds, dp;
	double WP;


	int N = 2*(m_pPyrImagePair.level-1-level + N_WINDOWS/2)+1;

	int iterations = 40;//迭代次数

	if(level==0)
		iterations = 180;

	printf("\nDisparity Map Refinement level:%d\n", level);
	for(int iter = 0; iter<iterations; iter++)
	{

		//printf("\nDisparity Map Refinement iter:%d\n", iter);
		for(int index=0; index<2; index++)
		{
			int t_index = (index+1)%2;


			Disparity* pDisparity = m_Disparity[index][level];

			Disparity::iterator itr  =  pDisparity->begin();
			for(; itr !=  pDisparity->end();++itr )
			{
				CvPoint p = itr.key();
				d = itr.value();

				/******************** compute  ds ****************************/
			 
				//left
				Disparity::iterator itr0 = pDisparity->find(cvPoint(p.x - 1, p.y));
				if(itr0==pDisparity->end()) 
					continue;
				d_left = itr0.value();




				//right

				Disparity::iterator itr1 = pDisparity->find(cvPoint(p.x + 1, p.y));
				if(itr1==pDisparity->end()) 
					continue;
				d_right = itr1.value();


				//up

				Disparity::iterator itr2 = pDisparity->find(cvPoint(p.x  , p.y-1));
				if(itr2==pDisparity->end()) 
					continue;
				d_up = itr2.value();


				//down

				Disparity::iterator itr3 = pDisparity->find(cvPoint(p.x  , p.y+1));
				if(itr3==pDisparity->end()) 
					continue;
				d_down = itr3.value();


				double tx = abs(d_left - d) - abs(d_right - d);
				double wx = exp(-tx*tx);

				double ty = abs(d_up - d) - abs(d_down - d);
				double wy = exp(-ty*ty);



				ds = (wx*(d_left+d_right) + wy*(d_up+d_down))/(2*(wx+wy));

				 

				/********************** compute dp*********************************/

				CvPoint q;
				q.y = p.y;
				q.x = p.x + d;

				std::vector<float> tex0, tex1;

				grabTex( m_pPyrImagePair.m_pImage[index][level], p, tex0, N);
				Normalize(tex0);

				{//left
					q.x -=1;
					grabTex( m_pPyrImagePair.m_pImage[t_index][level], q, tex1, N);
					Normalize(tex1);

					double ncc_value = RGB_NCC(tex0, tex1);

					e_left = (1-ncc_value)/2;
				}
				{//zero 
					q.x +=1;
					grabTex( m_pPyrImagePair.m_pImage[t_index][level], q, tex1, N);
					Normalize(tex1);

					double ncc_value = RGB_NCC(tex0, tex1);

					e_zero = (1-ncc_value)/2;
				}
				{//right 
					q.x +=1;
					grabTex( m_pPyrImagePair.m_pImage[t_index][level], q, tex1, N);
					Normalize(tex1);

					double ncc_value = RGB_NCC(tex0, tex1);

					e_right = (1-ncc_value)/2;
				}

				if(e_left<e_zero && e_left<e_right)
				{
					dp = d - 0.5;

					WP = e_left - e_zero;
				}
				else if(e_zero<e_left && e_zero<e_right)
				{
					dp = d + 0.5*(e_left - e_right)/(e_left+e_right-2*e_zero);

					WP = 0.5*(e_left+e_right-2*e_zero);
				}
				else
				{
					dp = d + 0.5;

					WP = e_right - e_zero;
				}



				 

				/********************** compute d1*********************************/
				 
				float d1 = (WP*dp+WS*ds)/(WP+WS);

				pDisparity->insert(p, d1);//更新差分图	
				 
			}
		}

	}


	//迭代结束后，要再次检查唯一性
	try
	{
		missMatch_Disparity[0] = new Disparity();//不满足条件的匹配
		missMatch_Disparity[1] = new Disparity();

		check_Uniqueness(level);
		//释放资源
		missMatch_Disparity[0]->clear();
		missMatch_Disparity[1]->clear();
		delete missMatch_Disparity[0];
		delete missMatch_Disparity[1];
	}
	catch(...)
	{

		printf("check_Uniqueness error... \n");
	}
}
//Triangulate all points
void PixelMatching::TriangulateAll(int level)
{
	if(m_Disparity[0][level]==NULL)
		return;

	/****************************************/
	v3_t pos0, pos1;
	v3_t frontDirection0, frontDirection1;

	m_CameraParam[0].GetPosition(pos0.p);//获取相机location
	m_CameraParam[1].GetPosition(pos1.p);

	m_CameraParam[0].ComputeCameraDirection();//计算相机的朝向
	m_CameraParam[1].ComputeCameraDirection();

	Vx(frontDirection0) = m_CameraParam[0].frontDirection[0];
	Vy(frontDirection0) = m_CameraParam[0].frontDirection[1];
	Vz(frontDirection0) = m_CameraParam[0].frontDirection[2];
	frontDirection0 = v3_unit(frontDirection0);
	frontDirection0 = v3_scale(-1, frontDirection0);

	Vx(frontDirection1) = m_CameraParam[1].frontDirection[0];
	Vy(frontDirection1) = m_CameraParam[1].frontDirection[1];
	Vz(frontDirection1) = m_CameraParam[1].frontDirection[2];
	frontDirection1 = v3_unit(frontDirection1);
	frontDirection1 = v3_scale(-1, frontDirection1);
	/*********************************************/

	printf("Triangulate ...\n");

	//int dd=0;
	Disparity::iterator itr  =  m_Disparity[0][level]->begin();
	for(;itr !=  m_Disparity[0][level]->end();++itr )
	{

		/**************显示进程**************************/
		/*if(dd==0)
		{
		  printf("\b");
		  printf("*");
		}
		else
		{
		  printf("\b");
		  printf("|");

		  dd=-1;
		}
		dd++;*/
		
		/*************************************/
		CvPoint _p = itr.key();
		float d = itr.value();

		CvPoint2D32f p;
		CvPoint2D32f q;

		p.y = _p.y;
		p.x = _p.x;
		q.y = p.y;
		q.x = p.x+d;

		CvPoint3D32f point3d;
		Triangulate(p, q, point3d, level);
		m_points[level].push_back(point3d);

		
		CvPoint3D32f color;
		getPoint3dColor(p, q, color, level);
		m_colors[level].push_back(color);


		/*************compute point normal *************/
		v3_t n0;
		v3_t n1;

		v3_t point;
		Vx(point) = point3d.x;
		Vy(point) = point3d.y;
		Vz(point) = point3d.z;

		n0 = v3_sub( pos0, point);
		n0 = v3_unit(n0);

		n1 = v3_sub( pos1, point);
		n1 = v3_unit(n1);

		CvPoint3D32f normal;

		double d0 = v3_dotp(n0, frontDirection0);
		double d1 = v3_dotp(n1, frontDirection1);

		if(d0>=d1)
		{
			normal.x = Vx(n0);
			normal.y = Vy(n0);
			normal.z = Vz(n0);

		}
		else
		{
			normal.x = Vx(n1);
			normal.y = Vy(n1);
			normal.z = Vz(n1);
		}

		
		m_normals[level].push_back(normal);

	}

	printf("\n");

}
/* Triangulate two points */
//refer  Triangulation (computer vision)

void PixelMatching::Triangulate(CvPoint2D32f p, CvPoint2D32f q,  CvPoint3D32f &point3d, int level)
{
 	vnl_vector<double> x1(2), x2(2);
 
 	vnl_matrix<double> A(4, 3);
 	vnl_vector<double> b(4);
 	vnl_vector<double> x(3);

	float scale = pow(2.0f, level);

	p.x = p.x *scale;
	p.y = p.y *scale;
	q.x = q.x *scale;
	q.y = q.y *scale;
	 

 	x1[0] = p.x;
 	x1[1] = p.y;
 	x2[0] = q.x;
 	x2[1] = q.y;
 	x1[0] = (x1[0] - m_CameraParam[0].m_K[2]) / m_CameraParam[0].m_K[0];
 	x1[1] = (x1[1] - m_CameraParam[0].m_K[5]) / m_CameraParam[0].m_K[4];
 	x2[0] = (x2[0] - m_CameraParam[1].m_K[2]) / m_CameraParam[1].m_K[0];
 	x2[1] = (x2[1] - m_CameraParam[1].m_K[5]) / m_CameraParam[1].m_K[4];
 
 
 	for (int k = 0;k < 2;k++)
 	{
 		for (int j = 0;j < 3;j++)
 		{
 			A[k][j] = m_CameraParam[0].m_R[k * 3 + j];
 			A[k + 2][j] =m_CameraParam[1].m_R[k * 3 + j];
 		}
 		b[k] = -m_CameraParam[0].m_T[k];
 		b[k + 2] = -m_CameraParam[1].m_T[k];
 	}
 
 
 	for (int j = 0;j < 2;j++)
 	{
 		A[j][0] -= m_CameraParam[0].m_R[6] * x1[j];
 		A[j][1] -= m_CameraParam[0].m_R[7] * x1[j];
 		A[j][2] -= m_CameraParam[0].m_R[8] * x1[j];
 		b[j] += m_CameraParam[0].m_T[2] * x1[j];
 		A[j + 2][0] -= m_CameraParam[1].m_R[6] * x2[j];
 		A[j + 2][1] -= m_CameraParam[1].m_R[7] * x2[j];
 		A[j + 2][2] -= m_CameraParam[1].m_R[8] * x2[j];
 		b[j + 2] += m_CameraParam[1].m_T[2] * x2[j];
 	}
 
 	vnl_matrix<double> ATA(A.transpose()*A);
 	vnl_vector<double> ATb(A.transpose()*b);
 	vnl_matrix_inverse<double> M(ATA);
 	x = M.solve(ATb);

	point3d.x = x[0];
 	point3d.y = x[1];
 	point3d.z = x[2];

	//printf("\nbefore levenberg_marquardt X=(%lf, %lf, %lf)\n", x[0], x[1], x[2]);

	////vnl_levenberg_marquardt

	//globa_Pmatrix0 = m_CameraParam[0].m_Pmatrix;
	//globa_Pmatrix1 = m_CameraParam[1].m_Pmatrix;
	//global_p[0] = p.x;
	//global_p[1] = p.y;
	//global_q[0] = q.x;
	//global_q[1] = q.y;

	//triangulation_residual f;
	//vnl_vector<double> x_LM(3);
 //   x_LM = vnl_levenberg_marquardt_minimize(f, x);

	//printf("after levenberg_marquardt x_LM=(%lf, %lf, %lf)\n", x_LM[0], x_LM[1], x_LM[2]);
 //
 //	point3d.x = x_LM[0];
 //	point3d.y = x_LM[1];
 //	point3d.z = x_LM[2];
	
}




//获取3d点的颜色
void PixelMatching::getPoint3dColor(CvPoint2D32f p, CvPoint2D32f q, CvPoint3D32f &color, int level)
{
	CvPoint3D32f color0;//, color1;

	IplImage *img0 = m_pPyrImagePair.m_pImage[0][level];
	//IplImage * img1 = m_pPyrImagePair_RGB.m_pImage[1][level];

	{
		int w_step = img0->nChannels*img0->depth/8;

		int index = img0->widthStep*p.y;
		uchar* imgPtr=(uchar*)(img0->imageData + index + (int)p.x* w_step) ;
		int r = *imgPtr; 
		int g = *(imgPtr+1);
		int b = *(imgPtr+2);

		color0.x = r;
		color0.y = g;
		color0.z = b;

	}

	//{
	//	int w_step = img1->nChannels*img0->depth/8;

	//	int index = img1->widthStep*q.y;
	//	uchar* imgPtr=(uchar*)(img1->imageData + index + (int)q.x* w_step) ;
	//	int r = *imgPtr; 
	//	int g = *(imgPtr+1);
	//	int b = *(imgPtr+2);

	//	color1.x = r;
	//	color1.y = g;
	//	color1.z = b;

	//}

	/*color.x = (int)(color0.x+color1.x)/2;
	color.y = (int)(color0.y+color1.y)/2;
	color.z = (int)(color0.z+color1.z)/2;*/

	color.x = color0.x;
	color.y = color0.y;
	color.z = color0.z;
		
}
//输出 Disparity
void PixelMatching::saveDisparity(int level)
{
	char path[1024];
	sprintf(path, "Disparity_%d.bmp",  level);

	int num_y = 60*pow(2.0f, (int)m_pPyrImagePair.level-1-level);


	IplImage* pImg0 = m_pPyrImagePair.m_pImage[0][level];//downsample(m_ImagePair.m_pImage[0], level);
	IplImage* pImg1 = m_pPyrImagePair.m_pImage[1][level];//downsample(m_ImagePair.m_pImage[1], level);

	

	IplImage* DisparityImg = stack_imgs(pImg0, pImg1);

	for(int index =0; index<2; index++)
	{
		Disparity *pDisparity  = m_Disparity[index][level];
	
		for(Disparity::iterator itr  =  pDisparity->begin(); itr !=  pDisparity->end();++itr )
		{


			CvPoint _p = itr.key();
			float d = itr.value();

			if(_p.y!=num_y)
				continue;

			int t = _p.y*DisparityImg->widthStep;


			CvPoint pt1, pt2;
			pt1 = cvPoint(  _p.x , _p.y);
			pt2 = cvPoint( _p.x+(int)d, _p.y);

			/*printf("index=%d: (%d, %d)  (%d %d)\n", index, pt1.x, pt1.y, pt2.x, pt2.y);*/

			if(index==0)
				pt2.y += pImg0->height;
			else
				pt1.y += pImg0->height;

			if(index==0)
				cvLine( DisparityImg, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );	//red
			else
				cvLine( DisparityImg, pt1, pt2, CV_RGB(255,255,0), 1, 8, 0 );//yellow



		}

	}
	cvSaveImage(path, DisparityImg);
}
//
////写点到ply文件中
//void PixelMatching::WritePly(int index)
//{
//	
//
//	for(int level=0; level<m_pPyrImagePair.level; level++)
//	{
//		char filename[256];
//	    sprintf(filename, "points_%d_%d.ply", index, level);
//		/*if(index==-1)
//			sprintf(filename, "points_all_%d.ply",  level);*/
//
//		DumpPointsToPly(m_DirName.data(), filename, m_points[level], m_colors[level]);
//	}
//}

//根据点距相机平面的距离来删除点
void PixelMatching::PointRemove(int level,float percent)
{
	double cam_pos[2][3];//相机坐标

	m_CameraParam[0].GetPosition(cam_pos[0]);
	m_CameraParam[1].GetPosition(cam_pos[1]);

	v3_t v[2];

	Vx(v[0]) = cam_pos[1][0]-cam_pos[0][0];
	Vy(v[0]) = cam_pos[1][1]-cam_pos[0][1];
	Vz(v[0]) = 0;

	double v0_mag = v3_mag(v[0]);


	//计算所有点到相机平面的距离,这里假设相机所在平面法向与z轴垂直,因此转化为XY平面问题

	std::vector<double> distance;//点到面的距离
	Vz(v[1]) = 0;

	double d_sum = 0;
	for(long i=0; i<m_points[level].size(); i++)
	{ 
		Vx(v[1]) = m_points[level][i].x;
		Vy(v[1]) = m_points[level][i].y;
		
		double d = v3_mag(v3_cross(v[0], v[1]))/v0_mag;

		distance.push_back(d);

		d_sum +=d;
	}

	double d_mean = d_sum/ m_points[level].size();//平均值


	for(long i=distance.size()-1; i>=0; i--)
	{
		double d = distance[i];

		if(d_mean<d&&(d-d_mean)/d_mean>percent)
		{//删除

			m_points[level].erase(m_points[level].begin()+i);
			m_colors[level].erase(m_colors[level].begin()+i);
		}
	}
	 
}