/////////////////////////////////////////////////////////////////////////////
// precompiled.cpp
// emtpy cpp file used to build precompiled headers
// $Id: precompiled.cpp,v 1.1 2003/10/07 20:17:44 tstivers Exp $
//

#include "precompiled.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
			if(*src == *replacepos)
				*src++ = replacement;
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

char* strlower(char* str)
{
	char* retval = str;

	for(;*str;str++)
		*str=tolower(*str);

	return retval;
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
