#pragma once

namespace misc {
	struct AliasList {
		typedef std::vector<std::pair<std::string, std::string> > list_t;
		list_t list;
		void load(const std::string& filename);
		const char* findAlias(const std::string& key);
	};
}