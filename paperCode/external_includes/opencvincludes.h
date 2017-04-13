#ifndef OPENCV_INCLUDES_H
#define OPENCV_INCLUDES_H

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"

// #include "opencv2/contrib/contrib.hpp"
//#include "opencv2/cudev/common.hpp"
//#include "opencv2/cudev.hpp"
#ifdef ENABLE_CUDA_OPENCV
#include "opencv2/core/cuda.hpp"
#include "opencv2/cuda.hpp"
// #include "opencv2/cudastereo.hpp"
// #include "opencv2/cudalegacy.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/cudafilters.hpp"
// #include "opencv2/optim/optim.hpp"
#include "opencv2/cudafeatures2d.hpp"
// #include "opencv2/nonfree/cuda.hpp"
#include "opencv2/cudaoptflow.hpp"
// #include "
#endif

#include "opencv2/gpu/gpu.hpp"
// #include "opencv2/nonfree/features2d.hpp"
#include "opencv2/features2d/features2d.hpp"
// #include "opencv2/nonfree/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/features2d/features2d.hpp"


#endif