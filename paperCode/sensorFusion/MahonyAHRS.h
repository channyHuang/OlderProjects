
#ifndef MahonyAHRS_h
#define MahonyAHRS_h

#include <cstdio>
//-------------------------------------------------------------------

namespace TrackSensor {
	class SensorFilter {
	public:
		void init();

		float invSqrt(float x);
		void MahonyAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
		void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);

		float twoKp, twoKi;
		float q0, q1, q2, q3;
		float integralFBx, integralFBy, integralFBz;
		float sampleFreq;
	};
}

#endif

