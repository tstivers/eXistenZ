/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: jssettings.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace jssettings {

	struct eqstr {
		bool operator() (char* s1, char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};

	void init(void);

	JSBool dump(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
	
	typedef std::hash_map<char*, settings::Setting*, std::hash<char*>, eqstr> propmap_hash;
	typedef propmap_hash::iterator propmap_iterator;

	bool addsetting(settings::Setting* setting);

	JSBool jsgetsetting(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool jssetsetting(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
};
