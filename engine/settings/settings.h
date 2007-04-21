#pragma once

namespace settings {

	class Setting;

	struct eqstr {
		bool operator() (char* s1, char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};

	typedef stdext::hash_map<char*, Setting*, hash_char_ptr> settings_hash_map;

	typedef bool (* setFunction)(Setting* setting, void* value);
	typedef bool (* getFunction)(Setting* setting, void** value);

	void init(void);
	void release(void);

	void addsetting(char* name, U8 type, U32 flags, setFunction setter, getFunction getter, void* data);
	void delsetting(char* name);
	Setting* findsetting(char* name);

	void dump(char* pattern = NULL, bool sort = true);


	enum {		
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_VECT3F,
		TYPE_USERDEF
	};

	static const int FLAG_VIRTUAL		= 1;
	static const int FLAG_READONLY		= 2;
	static const int FLAG_MANAGE_MEM	= 4;

	bool standard_setter(char* name, void* value);
	bool standard_getter(char* name, void* value);

	bool string_setter(Setting* setting, void* value);
	bool string_getter(Setting* setting, void** value);
	bool int_setter(Setting* setting, void* value);
	bool int_getter(Setting* setting, void** value);
	bool float_setter(Setting* setting, void* value);
	bool float_getter(Setting* setting, void** value);

	bool setstring(char*name, char* value);
	char* getstring(char*name);
	bool setint(char*name, int value);
	int getint(char*name);
	bool setfloat(char*name, float value);
	float getfloat(char*name);

	class Setting {
		public: 
			Setting() {};
			~Setting() {free(name);};
				
			char* name;
			setFunction set;
			getFunction get;
			U8 type;
			U32 flags;
			void* data;
	};	
};