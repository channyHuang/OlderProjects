#ifndef CAPTURE_SESSION_H
#define CAPTURE_SESSION_H

#include <string>
#include <map>
#include "node.h"

#define BAM_API __declspec(dllexport)

namespace capg {
	namespace sensor {
		/**
		* @brief ?????
		*/
		class BAM_API sensor_session_t
		{
		public:
			bool load(const std::string& file_name);
			bool save(const std::string& file_name);

			/**
			* @brief get_node ??????
			*
			* @param name ???
			*
			* @return ??Node
			*/
			Node& get_node(const std::string& name) {
				return nodes[name];
			}

		private:
			std::map<std::string, Node> nodes;
			double begin_time;
			double end_time;
		};
	}
}

#endif