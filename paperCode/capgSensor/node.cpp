#include "node.h"

using capg::sensor::Channel;

capg::sensor::Channel& capg::sensor::Node::add_channel(std::string channel_name)
{
	for (int i = 0; i < m_channels.size(); ++i) {
		if (m_channels[i].name() == channel_name) {
			printf("channel %s already exist in the node", channel_name.c_str());
			return m_channels[i];
		}
	}

	m_channels.push_back(Channel(channel_name));
	return m_channels.back();
}

capg::sensor::Channel& capg::sensor::Node::add_channel(const Channel& channel)
{
	m_channels.push_back(channel);
	return m_channels.back();
}

boost::optional<capg::sensor::Channel&> capg::sensor::Node::channel(std::string channel_name)
{
	for (int i = 0; i < m_channels.size(); ++i) {
		if (m_channels[i].name() == channel_name) {
			return boost::optional<Channel&>(m_channels[i]);
		}
	}

	return boost::none;
}

boost::optional<const Channel&> capg::sensor::Node::channel(std::string channel_name) const
{
	for (int i = 0; i < m_channels.size(); ++i) {
		if (m_channels[i].name() == channel_name) {
			return boost::optional<const Channel&>(m_channels[i]);
		}
	}

	return boost::none;
}

const Channel& capg::sensor::Node::channel(int index) const
{
	BOOST_ASSERT(index >= 0 && index < m_channels.size());
	return m_channels[index];
}

Channel& capg::sensor::Node::channel(int index)
{
	BOOST_ASSERT(index >= 0 && index < m_channels.size());
	return m_channels[index];
}