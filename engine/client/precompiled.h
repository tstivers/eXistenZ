/////////////////////////////////////////////////////////////////////////////
// precompiled.h
// all headers in this file are precompiled to make compilation faster
// $Id: precompiled.h,v 1.4 2003/11/24 00:16:13 tstivers Exp $
//

#define VC_EXTRALEAN

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include <string>
#include <hash_map>
#include <list>
#include <deque>
#include <vector>

#include "platform/types.h"
#include "common/common.h"
#include "jsapi.h"

#include "d3d9.h"
#include "d3dx9core.h"
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

#define ASSERT(f) (void)((f) || (DebugBreak(), 0))

#define DIK_BUTTON0 0xF0
#define DIK_BUTTON1 0xF1
#define DIK_BUTTON2 0xF2
#define DIK_BUTTON3 0xF3

#define BIGFLOAT 1000000000000.0f

extern HINSTANCE gHInstance;
extern int gActive;

// some misc crap functions
char* strlower(char* str);
char* strip(char* str);
int countArgs(char* args);
char *getToken(char **src, char *token_sep);
int wildcmp(const char* w, const char* s);
void load_map(const char* filename, alias_list& list);
char* find_alias(const char* key, alias_list& list);

// class for char* hash_map
class hash_char_ptr {
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