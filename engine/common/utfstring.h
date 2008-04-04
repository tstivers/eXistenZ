#pragma once

bool FromUtf8(const char* in, DWORD in_len, wchar_t* out, DWORD* out_len);
bool ToUtf8(const wchar_t* in, DWORD in_len, char* out, DWORD* out_len);

