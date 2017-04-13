#ifdef _MSC_VER
#pragma once
#endif

#ifndef CHANNEL_H__
#define CHANNEL_H__

#include <string>
#include <vector>

#include "frames.h"

#define BAM_API __declspec(dllexport)

namespace capg
{
	namespace sensor
	{
		//! 传感器通道
		/*!
		Details
		*/
		class BAM_API Channel
		{
		public:
			Channel() {}
			Channel(const std::string& name) : m_name(name) {}

			std::string name() const {
				return m_name;
			}
			void set_name(std::string val) {
				m_name = val;
			}

			int frame_count() const {
				return m_frames.size();
			}

			void add_frame(int index, double x, double y, double z, double time) {
				m_frames.push_back(Frame(index, x, y, z, time));
			}

			void add_frame(int index, double x, double y, double z, double w, double time) {
				m_frames.push_back(Frame(index, x, y, z, w, time));
			}

			Frame& frame(int i) {
				return m_frames[i];
			}

			const Frame& frame(int i) const {
				return m_frames[i];
			}

			template <typename T>
			void resort(){
				std::sort(m_frames.begin(), m_frames.end(), [](const T& a, const T&b){return a.time < b.time || (a.time == b.time && a.index < b.index); });
			}

		public:
			std::string m_name;
			std::vector<Frame> m_frames;
		};
	}
}

#endif // CHANNEL_H__