/********************************************************************
FileName :  FType.h
Version  :  0.10
Date     :	2010-2-1 19:41:30
Author   :  Feng(libyyu@qq.com)
Comment  :

*********************************************************************/
#ifndef __ILOG_H__
#define __ILOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#if defined(WIN32) || defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
	#include <windows.h>
	#include <float.h>
	#include <io.h>
#define STD_CALL  __stdcall
#define CALLBACK  __stdcall
#define WINAPI    __stdcall
#else
#define STD_CALL  
#define CALLBACK  
#define WINAPI    
#endif



enum LOG_TYPE
{
	LOG_ERROR = 0,
	LOG_ASSERT = 1,
	LOG_WARNING = 2,
	LOG_INFO = 3,
	LOG_EXCEPTION = 4,
};
typedef void (STD_CALL *PLogFunc) (int logType, const char* message);

namespace AnyLog
{
	class ILog
	{
	public:
		void Log(const char* format,...);
		void LogWarning(const char* format,...);
		void LogError(const char* format,...);
		void LogException(const char* format,...);
		virtual void SetLogCall(PLogFunc pFunc) = 0;
	protected:
		virtual void LogImpl(LOG_TYPE logType,const char* message) = 0;
	private:
		void LogFormatInner(LOG_TYPE logType,const char* format, va_list va);
	};

	typedef ILog *PLOG;
}

AnyLog::ILog* g_GetAnyLog();
void g_SetAnyLog(AnyLog::ILog*);

#define  log_info(fmt,...)    \
if(g_GetAnyLog() != NULL) \
{	\
	g_GetAnyLog()->Log(fmt,##__VA_ARGS__);  \
}
#define  log_warning(fmt,...)  \
if(g_GetAnyLog() != NULL) \
{	\
	g_GetAnyLog()->LogWarning(fmt,##__VA_ARGS__);  \
}
#define  log_error(fmt,...)   \
if(g_GetAnyLog() != NULL) \
{	\
	g_GetAnyLog()->LogError(fmt,##__VA_ARGS__);  \
}
#define  log_exception(fmt,...)   \
if(g_GetAnyLog() != NULL) \
{	\
	g_GetAnyLog()->LogException(fmt,##__VA_ARGS__);  \
}

#endif//__ILOG_H__