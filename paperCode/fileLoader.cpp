// loads all files of a given name and extension
#include "fileLoader.hpp"
    #include <boost/filesystem.hpp>
#include <fstream>
    
    using namespace cv;
    using namespace std;
    namespace fs = ::boost::filesystem;
    static fs::path root;
static vector<fs::path> txt;
static vector<fs::path> png;
static vector<fs::path> depth;


void get_all(const fs::path& root, const string& ext, vector<fs::path>& ret);
void loadAhanda(const char * rootpath,
                double range,
                int imageNumber,
                Mat& image,
                Mat& d,
                Mat& cameraMatrix,
                Mat& R,
                Mat& T){
    if(root!=string(rootpath)){

        root=string(rootpath);
        get_all(root, ".txt", txt);
        get_all(root, ".png", png);
        get_all(root, ".depth", depth);
                cout<<"Loading"<<endl;
    }
    
	convertAhandaPovRayToStandard((char *)txt[imageNumber].c_str(), imageNumber,
                                      cameraMatrix,
                                      R,
                                      T);
    cout<<"Reading: "<<png[imageNumber].filename().string()<<endl;
    imread(png[imageNumber].string(), -1).convertTo(image,CV_32FC3,1.0/range,1/255.0);
    int r=image.rows;
    int c=image.cols;
    if(depth.size()>0){
        cout<<"Depth: "<<depth[imageNumber].filename().string()<<endl;
        d=loadDepthAhanda(depth[imageNumber].string(), r,c,cameraMatrix);
    }
    
   
}


Mat loadDepthAhanda(string filename, int r,int c,Mat cameraMatrix){
    ifstream in(filename.c_str());
    int sz=r*c;
    Mat_<float> out(r,c);
    float * p=(float *)out.data;
    for(int i=0;i<sz;i++){
        in>>p[i];
        assert(p[i]!=0);
    }
    Mat_<double> K = cameraMatrix;
    double fx=K(0,0);
    double fy=K(1,1);
    double cx=K(0,2);
    double cy=K(1,2);
    for (int i=0;i<r;i++){
        for (int j=0;j<c;j++,p++){
            double x=j;
            double y=i;
            x=(x-cx)/fx;
            y=(y-cy)/fy;
            *p=*p/sqrt(x*x+y*y+1);
        }
    }
    
    
    
    return out;
}



