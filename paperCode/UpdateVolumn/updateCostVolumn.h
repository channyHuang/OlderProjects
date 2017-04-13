#include <iostream>

namespace UpdateFromDepth {
	class Volumetric {
	public:
		int rows, cols, layers;
		float near, far, depthStep;
		
		cv::gpu::GpuMat dataContainer;
		cv::gpu::GpuMat S, C, W;
		
		Volumetric() {}
		~Volumetric() {}
		Volumetric(int _rows, int _cols, int _layers, float _near, float _far);
		
		cv::Mat downloadOldStyle(int layer) {
			cv::Mat cost;
			cv::gpu::GpuMat tmp=dataContainer.rowRange(layer,layer+1);
			tmp.download(cost);
			cost=cost.reshape(0,rows);
			return cost;
		}
		
		cv::Mat image;
		cv::Mat K;
		cv::Mat R, T;
		void refine(cv::Mat depth, cv::Mat tmpimage, cv::Mat tmpR, cv::Mat tmpT);
	}
}