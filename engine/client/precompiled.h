#define VC_EXTRALEAN
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include <assert.h>
#include <shlwapi.h>
#include <float.h>

// disable the buggy visual studio tr1
#define _HAS_TR1 0

#include <utility>
#include <string>
#include <hash_map>
#include <list>
#include <deque>
#include <queue>
#include <vector>
#include <set>
#include <map>
#include <memory>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bimap.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/member_function_pointer.hpp>
#include <boost/function_types/function_pointer.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/is_member_function_pointer.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/type_traits.hpp>
#include <boost/typeof/typeof.hpp>

#include <d3d9.h>
#include <d3dx9core.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#ifdef _DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif

#include "js32.h"

#define ASSERT assert
#define DIK_BUTTON0 0xF0
#define DIK_BUTTON1 0xF1
#define DIK_BUTTON2 0xF2
#define DIK_BUTTON3 0xF3
#define DIK_MWHEELUP 0xF4
#define DIK_MWHEELDN 0xF5

using namespace std;
using namespace boost;

#include "common/types.h"
#include "common/log.h"
#include "common/mystring.h"
#include "common/registeredfunctions.h"
#include "math/math.h"
#include "script/wrappedhelpers.hpp"
#include "script/jsnativecall.h"

