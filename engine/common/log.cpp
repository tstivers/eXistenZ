#include "precompiled.h"
#include "common/log.h"

namespace Log {
	struct Consumer {
		Consumer(ConsumerCallback callback, void* user, U32 mask) :
			callback(callback), user(user), mask(mask) {}
		ConsumerCallback callback;
		void* user;
		U32 mask;
	};

	typedef std::map<std::string, Consumer> consumer_map_t;
	consumer_map_t consumer_map;
}

void Log::addConsumer(const std::string& name, unsigned int mask, ConsumerCallback callback, void* userdef /* = NULL */)
{
	consumer_map.insert(consumer_map_t::value_type(name, Consumer(callback, userdef, mask)));
}

void Log::removeConsumer(const std::string& name)
{
	consumer_map.erase(name);
}

void Log::log(const char* file, unsigned int line, const char* function, unsigned int flags, const char* format, ...)
{
	va_list args;
	char buffer[512];

	va_start(args, format);	
	vsprintf(buffer, format, args);
	va_end(args);

#ifdef _DEBUG
	char buffer2[512];
	sprintf(buffer2, "%s:%i(%s) %s\n", file, line, function, buffer);
	OutputDebugString(buffer2);
#endif

	for(consumer_map_t::const_iterator it = consumer_map.begin(); it != consumer_map.end(); it++) {
		if(flags & it->second.mask)
			it->second.callback(file, line, function, flags, buffer, it->second.user);
	}
}