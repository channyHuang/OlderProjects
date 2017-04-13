
#include "sensor_session.h"

#include <vector>
#include <numeric>
#include <string>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/all.hpp>

using namespace std;

bool inline exist_file(const std::string &filename)
{
	return boost::filesystem::exists(filename);
}

namespace capg {
	namespace sensor {
		bool sensor_session_t::save(const std::string& filename)
		{
			using std::string;
			using namespace boost;
			using boost::property_tree::ptree;

			// const string tag
			const string session_tag = "session";
			const char sep = '.';
			const string begin_time_tag = session_tag + sep + "begin-time";
			const string end_time_tag = session_tag + sep + "end-time";
			const string thread_count_tag = session_tag + sep + "thread-count";
			const string nodes_tag = session_tag + sep + "threads";

			// const node string name
			const std::string node_str = "thread";
			const std::string channel_str = "channel";
			const std::string frame_str = "frame";

			try {
				ptree root_pt;
				root_pt.put(begin_time_tag, begin_time);
				root_pt.put(end_time_tag, end_time);

				// put each node

				BOOST_FOREACH(auto &p, nodes) {
					const auto& node = p.second;

					ptree node_pt;
					node_pt.put("name", node.name());
					node_pt.put("channel-count", node.channel_count());

					for (int i = 0; i < node.channel_count(); ++i) {
						auto& cur_c = node.channel(i);

						ptree ch_pt;
						ch_pt.put("name", cur_c.name());
						ch_pt.put("frame-count", cur_c.frame_count());

						for (int j = 0; j < cur_c.frame_count(); ++j) {
							auto& cur_f = cur_c.frame(j);

							ptree f_pt;
							f_pt.put("index", cur_f.index);
							f_pt.put("time", cur_f.time);

							if (cur_c.name() == "r") {
								f_pt.put("w", cur_f.w);
							}
							f_pt.put("x", cur_f.x);
							f_pt.put("y", cur_f.y);
							f_pt.put("z", cur_f.z);

							ch_pt.add_child("frames", f_pt);
						}

						node_pt.add_child("threads", ch_pt);
					}

					root_pt.add_child(nodes_tag + sep + node_str, node_pt);
				}

				write_xml(filename, root_pt);
			}
			catch (std::exception& exce) {
				std::cerr << exce.what() << std::endl;
				return false;
			}

			return true;
		}

		bool sensor_session_t::load(const std::string& file_name)
		{
			if (!exist_file(file_name))   return false;

			map<std::string, Node> res;

			using std::string;
			using namespace boost;
			using boost::property_tree::ptree;

			// const string tag
			const string session_tag = "session";
			const char sep = '.';
			const string begin_time_tag = session_tag + sep + "begin-time";
			const string end_time_tag = session_tag + sep + "end-time";
			const string thread_count_tag = session_tag + sep + "thread-count";
			const string nodes_tag = session_tag + sep + "threads";

			// const node string name
			const std::string node_str = "thread";
			const std::string channel_str = "channel";
			const std::string frame_str = "frame";

			try {
				ptree root_pt;
				read_xml(file_name, root_pt);

				begin_time = root_pt.get<double>(begin_time_tag);
				end_time = root_pt.get<double>(end_time_tag);
				int node_count = root_pt.get<int>(thread_count_tag);

				// loop for each node
				BOOST_FOREACH(auto node_pair, root_pt.get_child(nodes_tag)) {
					if (node_pair.first != node_str)
						continue;

					auto node = node_pair.second;
					Node cur_node(node.get<string>("name"));
					int channel_count = node.get<int>("channel-count");

					// loop for each channel
					BOOST_FOREACH(auto channel_pair, node.get_child("channels")) {
						if (channel_pair.first != channel_str)
							continue;

						auto channel = channel_pair.second;
						auto channel_name = channel.get<string>("name");

						Channel cur_c(channel.get<string>("name"));
						int frame_count = channel.get<int>("frame-count");

						BOOST_FOREACH(auto frame_pair, channel.get_child("frames")) {
							if (frame_pair.first != frame_str)
								continue;

							auto frame = frame_pair.second;
							int index = frame.get<int>("index");
							double time = frame.get<double>("time");
							double x = frame.get<double>("value.x");
							double y = frame.get<double>("value.y");
							double z = frame.get<double>("value.z");

							if (channel_name == "r") {
								double w = frame.get<double>("value.w");
								cur_c.add_frame(index, x, y, z, w, time);
							}
							else {
								cur_c.add_frame(index, x, y, z, time);
							}
						}

						// check frame count correctness
						if (cur_c.frame_count() != frame_count) {
							std::cerr << "load session error : frame count is not correct" << std::endl;
							return false;
						}

						// add channel into current node
						cur_node.add_channel(cur_c);
					}

					// check node count correctness
					if (cur_node.channel_count() != channel_count) {
						std::cerr << "load session error : channel count is not correct" << std::endl;
						return false;
					}

					// add node into general_nodes
					res.insert(std::make_pair(cur_node.name(), cur_node));
				}

				// check node count correctness
				if (res.size() != node_count) {
					std::cerr << "load session error : node count is not correct" << std::endl;
					return false;
				}
			}
			catch (std::exception& exec) {
				std::cerr << exec.what() << std::endl;
				return false;
			}

			nodes = res;
			return true;
		}
	}
}