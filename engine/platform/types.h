/////////////////////////////////////////////////////////////////////////////
// types.h
// contains all basic typedefs
// $Id: types.h,v 1.2 2003/10/09 02:47:03 tstivers Exp $
//

#pragma once

typedef signed char     	S8;
typedef unsigned char   	U8;

typedef signed short    	S16;
typedef unsigned short  	U16;

typedef signed int      	S32;
typedef unsigned int    	U32;

typedef signed _int64		S64;
typedef unsigned _int64		U64;

typedef float				F32;
typedef double          	F64;

typedef struct _pair {
	char* key;
	char* value;
	_pair(char* k, char* v) {key = k; value = v;}
} pair;

typedef std::list<pair*> alias_list;
