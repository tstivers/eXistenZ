#define VC_EXTRALEAN
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include <assert.h>
#include <shlwapi.h>
#include <float.h>

#include <string>
#include <hash_map>
#include <list>
#include <deque>
#include <queue>
#include <vector>
#include <set>
#include <functional>
#include <map>
#include <algorithm>

#include "d3d9.h"
#include "d3dx9core.h"
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

#include "js32.h"

#include "common/types.h"
#include "common/log.h"
#include "common/smart_ptr.h"
#include "common/smart_cptr.h"
#include "common/mystring.h"
#include "common/FastDelegate.h"
#include "common/registeredfunctions.h"

#define DIK_BUTTON0 0xF0
#define DIK_BUTTON1 0xF1
#define DIK_BUTTON2 0xF2
#define DIK_BUTTON3 0xF3
#define DIK_MWHEELUP 0xF4
#define DIK_MWHEELDN 0xF5

#define ASSERT assert
