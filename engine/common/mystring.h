#pragma once

char* strlower(char* str);
char* strip(char* str);
int countArgs(char* args);
char *getToken(char **src, char *token_sep);
int wildcmp(const char* w, const char* s);
char* replacestr(char* src, char* find, char* replace);
char* sanitizePath(char* out, const char* in);
char* sanitizepath(char* path);
char* strDup(const char* src);

// class for char* hash_map
class hash_char_ptr_traits {
public:
	const static size_t bucket_size = 4;
	const static size_t min_buckets = 8;

	size_t operator()(const char* Key) const
	{
		size_t hash = 5381;
		int c;

		while (c = *Key++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}

	bool operator()(const char* keyval1,
		const char* keyval2) const
	{
		return strcmp(keyval1, keyval2) < 0; //<
	}
};

class char_ptr_less {
public:
	bool operator()(const char* val1,
		const char* val2) const
	{
		return strcmp(val1, val2) < 0; //<
	}
};