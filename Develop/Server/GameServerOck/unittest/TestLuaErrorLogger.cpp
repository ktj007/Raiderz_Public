#include "stdafx.h"
#include "SUnitTest.h"
#include "MLua.h"
#include "CSLogListener.h"
#include "CSLuaCallbackLogger.h"

SUITE(LuaErrorLogger)
{
	struct TestMLua: public MLua
	{
		void TestLog(const char* message)
		{
			if(m_pErrorHandler)	m_pErrorHandler(message);
		}
	};

	struct TestMWLua: public MWLua
	{
		TestMWLua()
		{
			L = lua_open();
			mapLua.insert(LUA_MAP::value_type(L, this));
		}

		~TestMWLua()
		{
			mapLua.erase(L);
			lua_close(L);
			L = NULL;
		}

		void TestLog(const char* message)
		{
			if (pfnException)
			{
				lua_pushfstring(L, message);
				Error();
			}
		}
	};

	struct LuaStringLogger: public CSLogListener
	{
		string message;

		virtual void OnLog(const char* message)
		{
			this->message = message;
		}
	};


	TEST(LuaLoggerLog)
	{
		LuaStringLogger logger;
		logger.OnLog("test error wstring");

		CHECK_EQUAL(logger.message, "test error wstring");
	}

	TEST(LuaLogAdaptorLog)
	{
		LuaStringLogger logger;
		
		CSLuaCallbackLogger::SetListener(&logger);
		CSLuaCallbackLogger::Log("test error wstring");

		CHECK_EQUAL(logger.message, "test error wstring");

		CSLuaCallbackLogger::SetListener(NULL);
	}

	TEST(OutputMLuaLog)
	{
		TestMLua lua;
		LuaStringLogger logger;

		CSLuaCallbackLogger::SetListener(&logger);
		lua.SetErrorHandler(CSLuaCallbackLogger::Log);
		lua.TestLog("test error wstring");

		CHECK_EQUAL(logger.message, "test error wstring");

		CSLuaCallbackLogger::SetListener(NULL);
	}

	TEST(OutputMWLuaLog)
	{
		TestMWLua lua;
		LuaStringLogger logger;

		CSLuaCallbackLogger::SetListener(&logger);
		lua.SetExceptionFunc(CSLuaCallbackLogger::Exception);
		lua.TestLog("test error wstring");

		CHECK_EQUAL(logger.message, "test error wstring");
		
		CSLuaCallbackLogger::SetListener(NULL);
	}
}
