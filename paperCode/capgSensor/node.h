
#include "channel.h"
#ifdef _MSC_VER
#pragma once
#endif

#ifndef NODE__H__
#define NODE__H__

#include "channel.h"
#include <vector>
#include <string>
#include <cstdio>
#include <boost/optional.hpp>

#define BAM_API __declspec(dllexport)

/**
* @brief
*/
namespace capg
{
	/**
	* @brief 传感器相关
	*/
	namespace sensor
	{
		//! 传感器节点
		/*!
		Details
		*/
		class BAM_API Node
		{
		public:
			Node(std::string node_name) : m_name(node_name) {}
			Node() {}

			std::string name() const { return m_name; }
			void set_name(std::string name) { m_name = name; }

			Channel& add_channel(std::string channel_name);
			Channel& add_channel(const Channel& channel);

			boost::optional<Channel&> channel(std::string channel_name);
			boost::optional<const Channel&> channel(std::string channel_name) const;

			Channel& channel(int index);
			const Channel& channel(int index) const;

			int channel_count() const { return m_channels.size(); }

		private:
			std::vector<Channel> m_channels;
			std::string m_name;
		};
	}
}

#endif // NODE__H__