#include "precompiled.h"
#include "mystring.h"

int countArgs(char* args)
{
	int argc = 1;
	if(!args || !*args)
		return 0;

	while(args = strchr(args, ' ')) {
		args++;
		argc++;
	}

	return argc;
}

void replacechars(char* src, char* replace, char replacement)
{
	char* replacepos;

	for(; *src; src++)
		for(replacepos = replace; *replacepos; replacepos++)
			if(*src == *replacepos) {
				*src = replacement;
				continue;
			}
}

void replacechar(char* src, char replace, char replacement)
{
	for(; *src; src++)
		if(*src == replace)
				*src = replacement;
}

char* replacestr(char* in, char* find, char* replace)
{
	char* pos = in;

	while(pos = strstr(pos, find)) {
		strncpy(pos, replace, strlen(replace));
		pos += strlen(replace);
		strcpy(pos, pos + (strlen(find) - strlen(replace)));
	}

	return in;
}


void stripchars(char* src, char* strip)
{
	char* srcpos; 
	char* strippos;

	for(srcpos = src; *srcpos; srcpos++)
		for(strippos = strip; *strippos; strippos++)
			if(*srcpos != *strippos)
				*src++ = *srcpos;

	*src = 0;
}

char* lstrip(char* str)
{
	if(!str || !*str)
		return str;

	char* pos;

	for(pos = str; isspace(*pos); pos++);
	for(;*pos;*str=*pos, pos++, str++);
	*str=0;
	return str;
}

char* rstrip(char* str)
{
	if(!str || !*str)
		return str;

	char* pos;

	for(pos=str + strlen(str) - 1; pos >= str && isspace(*pos); *pos = 0, pos--);
	return str;
}

char* strip(char* str)
{

	if(!str || !*str)
		return str;

	lstrip(str);
	rstrip(str);
	return str;
}

char *getToken(char **src, char *token_sep)
{
	char *tok;

	if(!(src && *src && **src))
		return NULL;
	while(**src && strchr(token_sep, **src))
		(*src)++;
	if(**src)
		tok = *src;
	else
		return NULL;
	*src = strpbrk(*src, token_sep);
	if(*src)
	{
		**src = '\0';
		(*src)++;
		while(**src && strchr(token_sep, **src))
			(*src)++;
	}
	else
		*src = "";
	return tok;
}

#define chrlower(x) { if ((x >= 'A') && (x <= 'Z')) x += ('a' - 'A'); }
#define chrupper(x) { if ((x >= 'a') && (x <= 'z')) x -= ('a' - 'A'); }

char* strlower(char* str)
{
	char* start = str;
	for(;*str;str++)
		chrlower(*str);

	return start;
}

// not mine although it looks like something i'd write
int wildcmp(const char* w, const char* s)
{
	if(toupper(*w) == toupper(*s)) return !*s || wildcmp(++w, ++s);
	if(!*s) return '*' == *w && wildcmp(++w, s);
	if('?' == *w) return wildcmp(++w, ++s);
	if('*' == *w) if(!*++w) return 1; else while(*s) if(wildcmp(w, s++)) return 1;
	return 0;
}

char* sanitizePath(char* out, const char* in)
{
	strcpy(out, in);
	if(!*out)
		return out;

	strlower(out);
	replacechar(out, '/', '\\');
	replacestr(out, "\\\\", "\\");	

	if(out[strlen(out) - 1] == '\\')
		out[strlen(out) - 1] = 0;
	if(out[0] == '\\')
		strcpy(out, out + 1);

	return out;
}

// this should be faster but isn't for some insane reason
char* sanitizepath(char* path)
{
	char* ret = path;
	char* in = path;
	char* out = path;

	while((*in == '\\') || (*in == '/'))
		++in;

	while(*in) {
		*out = *in;
		if(*out == '/')
			*out = '\\';
		if(*out == '\\') {
			while((*(in + 1) == '/') || (*(in + 1) == '\\'))
				++in;
			if(*(in + 1) == 0)
				*out = 0;
		} else
			chrlower(*out);
		in++, out++;
	}

	*out = 0;	

	return ret;
}

char* strDup(const char* src)
{
	char* dst = new char[strlen(src) + 1];
	strcpy(dst, src);
	return dst;
}