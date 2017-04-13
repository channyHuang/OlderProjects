#ifdef _MSC_VER
#pragma once
#endif

#ifndef FRAMES_H__
#define FRAMES_H__

#include "release_assert.hpp"
#include <stdexcept>

#include <string>

namespace capg
{
	namespace sensor
	{
		//! ????
		/*!
		TODO: with channel count as tempalte param.
		Details
		*/
		class Frame
		{
		public:
			Frame(int _index, double _x, double _y, double _z, double _time)
				: index(_index), x(_x), y(_y), z(_z), time(_time) {
			}

			Frame(int _index, double _x, double _y, double _z, double _w, double _time)
				: index(_index), x(_x), y(_y), z(_z), w(_w), time(_time) {
			}

			Frame() {}

			double& operator[](int index) {
				BOOST_ASSERT(index < 4);
				if (index == 0)
					return x;
				if (index == 1)
					return y;
				if (index == 2)
					return z;
				if (index == 3)
					return w;
				throw std::out_of_range(std::string("index out of frame channel (at most 4): ") + std::to_string(index));
			}

			const double& operator[](int index) const {
				BOOST_ASSERT(index < 4);
				if (index == 0)
					return x;
				if (index == 1)
					return y;
				if (index == 2)
					return z;
				if (index == 3)
					return w;
				throw std::out_of_range(std::string("index out of frame channel (at most 4): ") + std::to_string(index));
			}

		public:
			double x;
			double y;
			double z;
			double w;

			int index;
			double time;
		};
	}
}

#endif // FRAMES_H__