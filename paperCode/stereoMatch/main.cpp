#include <fstream>
#include <time.h>
#include "windows.h"
//using namespace std;

#include "PixelMatching.h"
#include "ply.h"

#include "cv.h"

//#include "PointsProcess.h"

int  g_N_WINDOWS = 3;
//NCC 的门限 小于该值的全部抛弃
double  g_NCC_Threshold = -0.5;

std::string g_DirName = "data";
int g_PairwiseNum = 0; //配对数量

std::vector<CvPoint3D32f> *g_points = NULL;
std::vector<CvPoint3D32f> *g_colors = NULL;
std::vector<CvPoint3D32f> *g_normals = NULL;

bool g_IsDebug = false;

//int g_Level_stop = 0;//运算到第几级停止
int g_Level_stop = 2;


/*********************** write cmvs PATCH******************************/
std::vector<int> *g_patchs = NULL;

typedef std::pair<int, int> MatchIndex;

std::vector<MatchIndex> g_MatchIndex;

//patch 格式见 http://grail.cs.washington.edu/software/pmvs/documentation.html
void WritePatch(const char *output_directory, const char *filename, std::vector<CvPoint3D32f> points, std::vector<CvPoint3D32f> normals, std::vector<int> patchs)
{
	char Patch_out[256];
    sprintf(Patch_out, "%s/%s", output_directory, filename);

	printf("write Patch %s\n", Patch_out);
    FILE *f = fopen(Patch_out, "w");

    if (f == NULL) {
	printf("Error opening file %s for writing\n", Patch_out);
	return;
    }

	fprintf(f, "PATCHES\n");
	fprintf(f, "%d\n", points.size());

	int index_patch = 0;
	for(int i=0; i<points.size(); i++)
	{
		/* Output the vertex */
		fprintf(f, "PATCHS\n");
		fprintf(f, "%.5f %.5f %.5f 1\n", points[i].x, points[i].y, points[i].z );
		fprintf(f, "%.6f %.6f %.6f 0\n", normals[i].x, normals[i].y, normals[i].z );

		fprintf(f, "0.7 0.5 0.5\n");
		fprintf(f, "2\n");

		
		for(int j=index_patch; j<patchs.size(); j++)
		{
			if(i<patchs[j])
			{
				index_patch = j;
				break;
			}

		}
		fprintf(f, "%d %d\n", g_MatchIndex[index_patch].first, g_MatchIndex[index_patch].second);

		fprintf(f, "0\n");
		fprintf(f, "\n");
		fprintf(f, "\n");
	}


	fclose(f);
}

void showMatching(PixelMatching matchs, int level, int pairIndex) {
	
	IplImage *image1, *image2;
	image1 = matchs.m_pPyrImagePair.m_pImage[0][level];
	image2 = matchs.m_pPyrImagePair.m_pImage[1][level];

	int width = image1->width, height = image1->height;
	IplImage *img1 = cvCreateImage(cvSize(width, height), image1->depth, image1->nChannels), *img2 = cvCreateImage(cvSize(width, height), image2->depth, image2->nChannels);

	cvResize(image1, img1);
	cvResize(image2, img2);
	IplImage* img = cvCreateImage(cvSize(width*2, height), img1->depth, img1->nChannels);
	//combine two images
	CvRect rect = cvRect(0, 0, img1->width, img1->height);
	cvSetImageROI(img, rect);
	cvCopy(img1, img);
	cvResetImageROI(img);

	rect = cvRect(img1->width, 0, img2->width, img2->height);
	cvSetImageROI(img, rect);
	cvCopy(img2, img);
	cvResetImageROI(img);

	//drawing matching points
	//int numOfPt = matchs.m_Disparity[0][level]->size();
	QHash<CvPoint, float>::iterator itr(matchs.m_Disparity[0][level]);
	int idx = 0;
	srand((unsigned)GetCurrentTime());
	while (itr != matchs.m_Disparity[0][level]->end()) {
		CvPoint pt1 = itr.key();
		CvPoint pt2 = cvPoint(pt1.x + itr.value() + width, pt1.y);

		idx++;
		if (idx % (width / 2) == 0) {
			cvLine(img, pt1, pt2, CV_RGB(rand()%255, rand()%255, rand()%255), 1, 8, 0);
		}
		++itr;
	}

	//showing
	cvNamedWindow("match");
	cvShowImage("match", img);
//	cvWaitKey(0);

	char str[100];
	sprintf(str, "match_%03d.jpg", pairIndex);
	cvSaveImage(str, img);

	cvReleaseImage(&matchs.m_pPyrImagePair.m_pImage[0][level]);
	cvReleaseImage(&matchs.m_pPyrImagePair.m_pImage[1][level]);
}

/************************ end write cmvs PATCH ***************************/

//获取配对数量
void getPairwiseNum()
{

	std::string pairwisFile = g_DirName+"\\pairwise.txt";

	char filename[1024];
	sprintf(filename, "%s ", pairwisFile.data());

	std::ifstream inFile(filename);

	if( !inFile )	{
		printf("not find  pairwise.txt \n");
		exit(-1);
	}

	printf("read pairwise.txt \n");

	
	inFile>>g_PairwiseNum; //配对数量

	printf("pairs = %d : ", g_PairwiseNum);

	g_MatchIndex.clear();
	while(!inFile.eof())
	{
		MatchIndex t_MatchIndex;

		inFile>>t_MatchIndex.first>>t_MatchIndex.second; //相机编号从1开始

		printf(" [%d %d] ", t_MatchIndex.first, t_MatchIndex.second);

		t_MatchIndex.first -=1; //pmvs patch 要求相机编号从0开始
		t_MatchIndex.second -=1;
		g_MatchIndex.push_back(t_MatchIndex);

		if (g_MatchIndex.size() >= g_PairwiseNum) break;
	}
	printf("\n");

	if(g_PairwiseNum>g_MatchIndex.size())
		printf("pairwise.txt 文件有误 配对数不符\n");

	inFile.close();
}


//匹配
void match()
{
	PixelMatching *tPixelMatching = new PixelMatching[g_PairwiseNum];
	
	for(int i=0; i<g_PairwiseNum; i++)
	{
		tPixelMatching[i].m_DirName = g_DirName;
		tPixelMatching[i].m_PairwiseNum = g_PairwiseNum;
		tPixelMatching[i].N_WINDOWS = g_N_WINDOWS;
		tPixelMatching[i].NCC_Threshold = g_NCC_Threshold;

//		tPixelMatching[i].init(i);//初始化数据
		tPixelMatching[i].init(g_MatchIndex.at(i).first, g_MatchIndex.at(i).second);
	}

	int levels = tPixelMatching[0].m_pPyrImagePair.level;//默认为3
	if(!g_points)
	{
		g_points = new std::vector<CvPoint3D32f>[levels];
	}
	if(!g_colors)
	{
		g_colors = new std::vector<CvPoint3D32f>[levels];
	}
	if(!g_normals)
	{
		g_normals = new std::vector<CvPoint3D32f>[levels];
	}
	if(!g_patchs)
	{
		g_patchs = new std::vector<int>[levels];
	}

	for(int level=tPixelMatching[0].m_pPyrImagePair.level-1; level>=0; level--)
	{
		if(level <g_Level_stop)
			break;

		printf("\n\n------------------matching level=%d --------------\n\n", level);
		/***************** cal time *****************************/
		clock_t start = clock();
		/***************** cal time *****************************/

		for(int i=0; i<g_PairwiseNum; i++)
		{
			printf("------start FindBestMatch %d/%d------\n", i+1, g_PairwiseNum);

			tPixelMatching[i].FindBestMatch(level);

			if(i==0&&level<2)
			{
				tPixelMatching[i].PointRemove(level, 0.5);

			}

			 g_points[level].insert(g_points[level].end(), tPixelMatching[i].m_points[level].begin(), tPixelMatching[i].m_points[level].end());
			 g_colors[level].insert(g_colors[level].end(), tPixelMatching[i].m_colors[level].begin(), tPixelMatching[i].m_colors[level].end());
			 g_normals[level].insert(g_normals[level].end(), tPixelMatching[i].m_normals[level].begin(), tPixelMatching[i].m_normals[level].end());

			 printf("points: %d\n", tPixelMatching[i].m_points[level].size());

			 printf("------end FindBestMatch %d/%d------\n\n", i+1, g_PairwiseNum);

			 if(g_IsDebug)
			 {
				 //写ply
				 char filename[256];
				 sprintf(filename, "points_%d_%d.ply",  level, i+1);
				 DumpPointsToPly(g_DirName.data(), filename, tPixelMatching[i].m_points[level], tPixelMatching[i].m_colors[level]);	
			 }

			 int num_points = g_points[level].size();

			 g_patchs[level].push_back(num_points);
			

			 //show matching images
			 if (level <= 3) {
				 showMatching(tPixelMatching[i], level, i);
			 }
		}

			/***************** cal time *****************************/
		clock_t end = clock();    
		printf("\n------------------matching level %d : took %0.3fs ------------------\n\n",  level, (end - start) / ((double) CLOCKS_PER_SEC));
		/***************** cal time *****************************/

		//写ply
		char filename[256];
	    sprintf(filename, "points_all_%d.ply",  level);
	    DumpPointsToPly(g_DirName.data(), filename, g_points[level], g_colors[level]);	

		//写 patch	
	    sprintf(filename, "points_all_%d.patch",  level);
		WritePatch(g_DirName.data(), filename, g_points[level], g_normals[level], g_patchs[level]);

		g_points[level].clear();
		g_colors[level].clear();
		g_normals[level].clear();

	}

	for(int i=0; i<g_PairwiseNum; i++)
	{
		tPixelMatching[i].clear();//释放数据
	}
	  
	delete tPixelMatching;
}
//主函数
int main(int argc, char *argv[])
{
	argc = 2;
	argv[1] = "E:/RealFootData/data_3";
	//write pairwise file
	
	std::locale::global(std::locale(""));	//解决打开包含中文的文件目录的问题


	if(argc>=2)
		g_DirName = argv[1];

	if(argc>=3)
		sscanf(argv[2], "%d", &g_N_WINDOWS);
		//N_WINDOWS =  atoi(argv[2]);

	if(argc>=4)
		sscanf(argv[3], "%lf", &g_NCC_Threshold);

#if 1
	std::ofstream ofs(g_DirName + "/pairwise.txt");
	int pairNum = 99;
	ofs << pairNum << std::endl;
	for (int i = 1; i <= pairNum; i++) {
		ofs << i << " " << i + 1 << std::endl;
	}
	ofs.close();
#endif // 1


	{ //更多参数命令
		char buf[1024];

		int iarg = 4;

		while(argc>iarg)
		{

			sscanf(argv[iarg], "%s", buf);

			if(strcmp(buf, "-debug")==0)
			{
				g_IsDebug = true;
				iarg++;

			}

			if(strcmp(buf, "-level")==0)
			{
				iarg++;

				if(argc<=iarg)
				{
					printf("input paramater error : -level 0\n");
					exit(-1);
				}
				 
				sscanf(argv[iarg], "%d", &g_Level_stop);//运算到第几层
				iarg++;

				
			}
		}
	}
		 

	//获取配对数量
	getPairwiseNum();

	match();

	return 0;

}