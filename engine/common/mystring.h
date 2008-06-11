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

string StripPathFromFileName(const string& filename);
string StripFileNameFromPath(const string& filename);

// class for char* hash_map
class hash_char_ptr_traits
{
public:
	enum
	{
		bucket_size = 4,
		min_buckets = 8
	};

	size_t operator()(const char* key) const
	{
		size_t hash = 5381;
		int c;

		while (c = *key++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}

	bool operator()(const char* val1, const char* val2) const
	{
		return strcmp(val1, val2) < 0; //<
	}
};

class char_ptr_less
{
public:
	bool operator()(const char* val1, const char* val2) const
	{
		return strcmp(val1, val2) < 0; //<
	}
};