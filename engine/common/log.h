#pragma once

#ifdef ERROR
	#undef ERROR
#endif

#define	LF_NONE		(1<<0)
#define	LF_INFO1	(1<<1)
#define	LF_INFO2	(1<<2)
#define	LF_INFO3	(1<<3)
#define LF_INFO		(LF_INFO1|LF_INFO2|LF_INFO3)
#define	LF_ERROR	(1<<4)
#define	LF_WARNING	(1<<5)
#define	LF_DEBUG1	(1<<6)
#define	LF_DEBUG2	(1<<7)
#define	LF_DEBUG3	(1<<8)
#define LF_DEBUG	(LF_DEBUG1|LF_DEBUG2|LF_DEBUG3)
#define	LF_FRAME	(1<<9)
#define LF_SCRIPT	(1<<10)
#define LF_CONSOLE  (1<<11)
#define	LF_ALL		0xffff

#define LOG(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_INFO,__VA_ARGS__)
#define INFO(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_INFO,__VA_ARGS__)
#define WARNING(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_WARNING,__VA_ARGS__)
#define ERROR(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_ERROR,__VA_ARGS__)
#define DEBUG(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_DEBUG,__VA_ARGS__)

#define JSLOG(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_SCRIPT|LF_INFO,__VA_ARGS__)
#define JSINFO(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_SCRIPT|LF_INFO,__VA_ARGS__)
#define JSWARNING(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_SCRIPT|LF_WARNING,__VA_ARGS__)
#define JSERROR(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_SCRIPT|LF_ERROR,__VA_ARGS__)
#define JSDEBUG(...) Log::log(__FILE__,__LINE__,__FUNCTION__,LF_SCRIPT|LF_DEBUG,__VA_ARGS__)

namespace Log {
	typedef void (* ConsumerCallback)(const char* file, unsigned int line, const char* function, unsigned int flags, const char* message, void* user);

	void addConsumer(const std::string& name, unsigned int mask, ConsumerCallback callback, void* userdef = NULL);
	void removeConsumer(const std::string& name);
	void setConsumerMask(const std::string& name, unsigned int mask);
	unsigned int getConsumerMask(const std::string& name);

	void log(const char* file, unsigned int line, const char* function, unsigned int flags, const char* format, ...);
};
