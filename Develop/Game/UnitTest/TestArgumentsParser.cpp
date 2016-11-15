#include "stdafx.h"
#include "XArgumentsParser.h"
#include "MCommandLine.h"

SUITE(ArgumentsParser)
{
	class TestArgumentsParser : public XArgumentsParser
	{
	public:
		using XArgumentsParser::ParseReplayFile;
	};
	class FArgumentsParser
	{
	public:
		FArgumentsParser()
		{

		}
		~FArgumentsParser()
		{

		}
		XArgumentsParserResult	m_Result;
		TestArgumentsParser		m_Parser;
		cml2::MCommandLine		m_CommandLine;

		XArgumentsParserResult Parse(const wchar_t* szArgs)
		{
			m_Result = m_Parser.Parse(szArgs, m_CommandLine);
			return m_Result;
		}

		XArgumentsParserResult PmangParse(const wchar_t* szArgs)
		{
			m_Parser.ParseLoginPmang(szArgs, m_Result);
			return m_Result;
		}

		void CHECK_Result(_EXECUTE_MODE nExpectExecuteMode, XGameFrameID nExpectGameFrameID, const wchar_t* szExpectGameFrameArgs=NULL)
		{
			CHECK_EQUAL(nExpectExecuteMode, m_Result.nExecuteMode);
			CHECK_EQUAL(nExpectGameFrameID, m_Result.nStartGameFrameID);
			if (szExpectGameFrameArgs)
			{
				CHECK(szExpectGameFrameArgs == m_Result.strGameFrameArgs);
			}
		}
	};
	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseReplayFile)
	{
		Parse(L"bird01.smv");
		CHECK_Result(EM_REPLAY, REPLAY_FRAME_ID, L"bird01.smv");

		Parse(L"\"Dev/Program Files/aaa.smv\"");
		CHECK_Result(EM_REPLAY, REPLAY_FRAME_ID, L"Dev/Program Files/aaa.smv");

		Parse(L"aBc.sMv");
		CHECK_Result(EM_REPLAY, REPLAY_FRAME_ID, L"aBc.sMv");
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseReplayFile_Negative)
	{
		Parse(L"111.sm");
		CHECK(EM_REPLAY != m_Result.nExecuteMode);

		Parse(L"111smv");
		CHECK(EM_REPLAY != m_Result.nExecuteMode);

		Parse(L"smv");
		CHECK(EM_REPLAY != m_Result.nExecuteMode);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseZoneTest)
	{
		Parse(L"zonetest");
		CHECK_Result(EM_LOCAL_TEST, PLAY_FRAME_ID);

		Parse(L"ZoneTest ");
		CHECK_Result(EM_LOCAL_TEST, PLAY_FRAME_ID);

		Parse(L"local");
		CHECK_Result(EM_LOCAL_TEST, PLAY_FRAME_ID);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseZoneTest_Arguments)
	{
		// local zonename posx posy posz dirx diry dirz time weather
		Parse(L"zonetest birdtown 100 200 300 1 0 0 2 1");
		CHECK_Result(EM_LOCAL_TEST, PLAY_FRAME_ID);

		CHECK_EQUAL(true, gvar.Network.bDirectLogin);
		CHECK_EQUAL(true, gvar.Network.bDirectSelChar);

		CHECK_CLOSE2(100.0f, gvar.Network.vStartPos.x);
		CHECK_CLOSE2(200.0f, gvar.Network.vStartPos.y);
		CHECK_CLOSE2(300.0f, gvar.Network.vStartPos.z);
		CHECK_CLOSE2(1.0f, gvar.Network.vStartDir.x);
		CHECK_CLOSE2(0.0f, gvar.Network.vStartDir.y);
		CHECK_CLOSE2(0.0f, gvar.Network.vStartDir.z);

		CHECK_EQUAL((GAME_TIME_TYPE)2, gvar.World.nGameTime);
		CHECK_EQUAL((GAME_WEATHER_TYPE)1, gvar.World.nWeather);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLogin_Case1)
	{
		Parse(L"login birdkr localhost 9001 1 1 1");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);

		CHECK_EQUAL(true, gvar.Network.bDirectLogin);
		CHECK_EQUAL(true, gvar.Network.bDirectSelChar);

		CHECK(wstring(L"birdkr") == gvar.Network.szLoginID);
		CHECK(wstring(L"localhost") == gvar.Network.szIP);
		CHECK_EQUAL(9001, gvar.Network.nDirectLoginFieldID);
		CHECK_CLOSE2(1.0f, gvar.Network.vStartPos.x);
		CHECK_CLOSE2(1.0f, gvar.Network.vStartPos.y);
		CHECK_CLOSE2(1.0f, gvar.Network.vStartPos.z);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLogin_Case2)
	{
		Parse(L"login birdkr icecream 월드이름");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);

		CHECK_EQUAL(true, gvar.Network.bDirectLogin);		

		CHECK(wstring(L"birdkr") == gvar.Network.szLoginID);
		CHECK(wstring(L"icecream") == gvar.Network.szIP);
		CHECK(wstring(L"월드이름") == gvar.Network.szWorldName);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLogin_Case3)
	{
		Parse(L"login birdkr icecream");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);

		CHECK_EQUAL(false, gvar.Network.bDirectLogin);
		CHECK_EQUAL(false, gvar.Network.bDirectSelChar);

		CHECK(wstring(L"birdkr") == gvar.Network.szLoginID);
		CHECK(wstring(L"icecream") == gvar.Network.szIP);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLogin_Launch)
	{
		Parse(L"launch birdkr icecream");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);

		CHECK_EQUAL(false, gvar.Network.bLoginToGameServer);

		CHECK_EQUAL(false, gvar.Network.bDirectLogin);
		CHECK_EQUAL(false, gvar.Network.bDirectSelChar);

		CHECK(wstring(L"birdkr") == gvar.Network.szLoginID);
		CHECK(wstring(L"icecream") == gvar.Network.szIP);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLogin_Standalone)
	{
		Parse(L"standalone birdkr icecream");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);

		CHECK_EQUAL(true, gvar.Network.bLoginToGameServer);
	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLogin_Standalone2)
	{
		Parse(L"login birdkr localhost");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);
		CHECK_EQUAL(true, gvar.Network.bLoginToGameServer);

		Parse(L"login birdkr 127.0.0.1");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);
		CHECK_EQUAL(true, gvar.Network.bLoginToGameServer);

	}

	TEST_FIXTURE(FArgumentsParser, ArgumentsParser_ParseLoginPmang)
	{
		PmangParse(L"-key\"1234|birdkr|1000|_|_|1100|68203b08fb60425cdf40aefa786e17b1 01401974\"");

		CHECK_Result(EM_GAME, LOGIN_FRAME_ID);

		// statindex 지역정보값 01 -> 00 으로 하나 작야져야한다.
		CHECK(wstring(L"1234|birdkr|1000|_|_|1100|68203b08fb60425cdf40aefa786e17b1 00401974") == gvar.Network.PmangInfo.strLauncherParameter);
		CHECK_EQUAL(true, gvar.Network.PmangInfo.isLoginOnPmang);
	}
}
