/////////////////////////////////////////////////////////////////////////////
// precompiled.h
// all headers in this file are precompiled to make compilation faster
// $Id: precompiled.h,v 1.12 2004/07/09 16:04:56 tstivers Exp $
//

#define VC_EXTRALEAN

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include <assert.h>
#include <shlwapi.h>

#define inline_ __forceinline
#pragma intrinsic(memcmp)
#pragma intrinsic(memcpy)
#pragma intrinsic(memset)
#pragma intrinsic(strcat)
#pragma intrinsic(strcmp)
#pragma intrinsic(strcpy)
#pragma intrinsic(strlen)
#pragma intrinsic(abs)
#pragma intrinsic(labs)
#pragma inline_depth( 255 )

#include <string>
#include <hash_map>
#include <list>
#include <deque>
#include <vector>
#include <set>

#include "platform/types.h"
#include "common/common.h"
#include "common/smart_ptr.h"
#include "common/smart_cptr.h"
#include "common/mystring.h"
#include "jsapi.h"

#include "d3d9.h"
#include "d3dx9core.h"
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

#include "resource/resource.h"

#include "zlib/include/zlib.h"


#define ASSERT(f) (void)((f) || (DebugBreak(), 0))

#define DIK_BUTTON0 0xF0
#define DIK_BUTTON1 0xF1
#define DIK_BUTTON2 0xF2
#define DIK_BUTTON3 0xF3
#define DIK_MWHEELUP 0xF4
#define DIK_MWHEELDN 0xF5

#define BIGFLOAT 1000000000000.0f

#define EXTENSION_TEXTMESH ".tm"
#define EXTENSION_TEXTMESHSYSTEM ".tms"

extern HINSTANCE gHInstance;
extern int gActive;

#include "math/vertex.h"

// some misc crap functions
void load_map(const char* filename, alias_list& list);
char* find_alias(const char* key, alias_list& list);

// class for char* hash_map
class hash_char_ptr {
public:
	const static size_t bucket_size = 4;
    const static size_t min_buckets = 8;
    
	inline_ size_t operator()(const char* Key) const
    {
        size_t hash = 5381;
        int c;

        while (c = *Key++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }
    
    inline_ bool operator()(const char* keyval1,
        const char* keyval2) const
    {
        return strcmp(keyval1, keyval2) < 0; //<
    }
};