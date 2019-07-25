#include "stdafx.h"
#include "AnyLog.h"

using namespace AnyLog;

void ILog::Log(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	LogFormatInner(LOG_INFO, format, va);
	va_end(va);
}

void ILog::LogWarning(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	LogFormatInner(LOG_WARNING, format, va);
	va_end(va);
}

void ILog::LogError(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	LogFormatInner(LOG_ERROR, format, va);
	va_end(va);
}

void ILog::LogException(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	LogFormatInner(LOG_EXCEPTION, format, va);
	va_end(va);
}

void ILog::LogFormatInner(LOG_TYPE logType, const char* format, va_list va)
{
	if (format)
	{
		char buff[2049] = { 0 };
#ifdef _WIN32
		_vsnprintf(buff, 2048, format, va);
#else
		vsnprintf(buff, 2048, format, va);
#endif

		LogImpl(logType, buff);
	}
}

class FLog : public ILog
{
private:
	PLogFunc _log_message;
public:
	FLog() : _log_message(NULL) {}
	void SetLogCall(PLogFunc pFunc)
	{
		assert(pFunc && "AnyLog::PLogFunc must not be null.");
		_log_message = pFunc;
	}
	static AnyLog::PLOG CreateILog(void* pfunc)
	{
		AnyLog::PLOG pLog = new FLog();
		pLog->SetLogCall((PLogFunc)pfunc);
		pLog->Log("AnyLog Established!");
		return pLog;
	}
	static void DestroyILog(AnyLog::PLOG pLog)
	{
		if (NULL != pLog)
		{
			delete pLog;
			pLog = NULL;
		}
	}
protected:
	void LogImpl(LOG_TYPE logType, const char* message)
	{
		if (_log_message != NULL)
		{
			char buff[2049] = { 0 };
			snprintf(buff, 2048, "%s", message);
			_log_message((int)logType, buff);
		}
	}
};

static AnyLog::ILog* g_theLog = NULL;
AnyLog::ILog* g_GetAnyLog()
{
	return g_theLog;
}
void g_SetAnyLog(AnyLog::ILog* pLog)
{
	FLog::DestroyILog(g_theLog);
	g_theLog = pLog;
}

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _WIN32
	__declspec(dllexport) void Lua_EstablishAnyLog(void* pfunc)
	{
		g_SetAnyLog(FLog::CreateILog(pfunc));
	}

	__declspec(dllexport) void Lua_UnEstablishAnyLog()
	{
		log_info("UnEstablishAnyLog.");
		g_SetAnyLog(NULL);
	}
#else
	extern void Lua_EstablishAnyLog(void* pfunc)
	{
		g_SetAnyLog(FLog::CreateILog(pfunc));
	}

	extern void Lua_UnEstablishAnyLog()
	{
		log_info("UnEstablishAnyLog.");
		g_SetAnyLog(NULL);
	}
#endif//_WIN32


#if defined(__cplusplus)
}
#endif

/*
 *在C#中使用方法：
 public delegate void XLua_Unity_Log_Delegate(LogType logType, string message);

 [DllImport(LUADLL, CallingConvention = CallingConvention.Cdecl)]
 public extern static void Lua_EstablishAnyLog(XLua_Unity_Log_Delegate func);

 [AOT.MonoPInvokeCallback(typeof(XLua_Unity_Log_Delegate))]
 private static void On_XLuaLog_WarpUnity(LogType logType, string message)
 {
	 switch(logType)
	 {
	 case LogType.Log:
		Debug.Log(message);
		break;
	 case LogType.Warning:
		Debug.LogWarning(message);
		break;
	 case LogType.Error:
		Debug.LogError(message);
		break;
	 case LogType.Exception:
		Debug.LogException(new Exception(message));
		break;
	 case LogType.Assert:
		Debug.LogError(message);
		break;
	 }
 }

 public static void InitXLuaAnyLog()
 {
	L_EstablishAnyLog(On_XLuaLog_WarpUnity);
 }
*/