#pragma once

namespace misc {
	struct AliasList {
		typedef vector<pair<string, string> > list_t;
		list_t list;
		void load(const string& filename);
		const char* findAlias(const char* key);
	};
}