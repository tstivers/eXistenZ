#include "precompiled.h"
#include "common/log.h"

namespace Log {

}

void Log::log(const char* file, unsigned int line, const char* function, unsigned int flags, const char* format, ...)
{
	va_list args;
	char buffer[512];
	char buffer2[512];

	va_start(args, format);	
	vsprintf(buffer, format, args);
	va_end(args);

	sprintf(buffer2, "%s:%i(%s) %s\n", file, line, function, buffer);
	OutputDebugString(buffer2);
}