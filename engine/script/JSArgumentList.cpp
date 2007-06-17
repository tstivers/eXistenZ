#include "precompiled.h"
#include "script/JSArgumentList.h"

namespace script {
}

using namespace script;

JSArgumentList::JSArgumentList(JSContext* cx)
	: cx(cx)
{
}

JSArgumentList::~JSArgumentList()
{
	clear();
}

int JSArgumentList::getCount()
{
	return argv.size();
}

jsval* JSArgumentList::getArgv()
{
	if(!argv.empty())
		return &argv[0];
	else
		return NULL;
}

void JSArgumentList::clear()
{
	for(vec_jsval::iterator it = roots.begin(); it != roots.end(); it++)
		JS_RemoveRoot(cx, &(*it));
	roots.clear();
	argv.clear();
}

JSArgumentList& JSArgumentList::addParam(jsval val, bool rooted /* = false */)
{
	if(!rooted && JSVAL_IS_GCTHING(val)) {
		JS_AddRoot(cx, &val);
		roots.push_back(val);
	}
	
	argv.push_back(val);
	return *this;
}

JSArgumentList& JSArgumentList::addParam(int i)
{
	ASSERT(INT_FITS_IN_JSVAL(i));
	return addParam(INT_TO_JSVAL(i));		
}

JSArgumentList& JSArgumentList::addParam(float f)
{
	double d = f;
	return addParam(d);
}

JSArgumentList& JSArgumentList::addParam(double d)
{
	jsval number;
	JSBool ret = JS_NewNumberValue(cx, d, &number);
	ASSERT(ret == JS_TRUE);
	return addParam(number);
}

JSArgumentList& JSArgumentList::addParam(bool b)
{
	return addParam(BOOLEAN_TO_JSVAL(b));
}

JSArgumentList& JSArgumentList::addParam(std::string& str)
{
	return addParam(str.c_str());
}

JSArgumentList& JSArgumentList::addParam(const char* str)
{
	JSString* jsstr = JS_NewStringCopyZ(cx, str);
	ASSERT(jsstr);
	return addParam(STRING_TO_JSVAL(jsstr));
}