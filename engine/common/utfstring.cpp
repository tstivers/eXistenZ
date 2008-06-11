#include "precompiled.h"
#include "utfstring.h"

bool FromUtf8(const char* in, DWORD in_len, wchar_t* out, DWORD* out_len)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, in, in_len, NULL, 0);
	ASSERT(widesize != ERROR_NO_UNICODE_TRANSLATION);
	ASSERT(widesize != 0);

	if (widesize > *out_len)
		return false;

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, in, -1, out, *out_len);

	ASSERT(convresult == widesize);
	*out_len = convresult;

	return true;
}

bool ToUtf8(const wchar_t* in, DWORD in_len, char* out, DWORD* out_len)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, in, in_len >> 1, NULL, 0, NULL, NULL);
	ASSERT(utf8size != 0);

	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, in, in_len >> 1, out, *out_len, NULL, NULL);

	ASSERT(convresult == utf8size);
	*out_len = convresult;
	out[convresult] = 0;

	return true;
}