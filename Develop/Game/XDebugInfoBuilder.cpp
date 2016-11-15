#include "stdafx.h"
#include "XDebugInfoBuilder.h"
#include "XGlobalVar.h"
#include "XGlobal.h"
#include "XNetwork.h"
#include "XMyPlayer.h"
#include "XGameState.h"
#include "XStrings.h"
#include "XDef.h"
#include "SVNRevision.h"

void XDebugInfoBuilder::Build()
{
	m_strBuf = L"";
	m_strBuf += _Build_Header();
	m_strBuf += _Build_Server();
	m_strBuf += _Build_Field();
	m_strBuf += _Build_Character();
	m_strBuf += _Build_Tail();
}

wstring XDebugInfoBuilder::_Build_Header()
{
	wstring str;
	AddLine(str);

	str += L" Debug Info\n";

	AddLine(str);


	return str;
}

wstring XDebugInfoBuilder::_Build_Tail()
{
	wstring str;

	AddLine(str);

	return str;
}

wstring XDebugInfoBuilder::_Build_Server()
{
	wstring str;

	wchar_t text[1024];

	wstring strServer, strLoginID;

	strServer = global.net->GetDesc().strServerIP;
	strLoginID = gvar.MyInfo.strLoginID;

	swprintf_s(text, L"SVN Revision: %s , SVN Date: %s\n", MLocale::ConvAnsiToUTF16(SVN_REVISION).c_str(), MLocale::ConvAnsiToUTF16(SVN_DATE).c_str());
	str += text;

	swprintf_s(text, L"Server: %s , LoginID: %s\n", strServer.c_str(), strLoginID.c_str());
	str += text;

	return str;
}

void XDebugInfoBuilder::AddLine( wstring& str )
{
	str += L"-------------------------------------------------------------------------------------\n";
}

wstring XDebugInfoBuilder::_Build_Field()
{
	if (gg.currentgamestate == NULL ||
		gg.currentgamestate->GetWorld() == NULL ||
		gg.currentgamestate->GetWorld()->GetInfo() == NULL)
	{
		return L"";
	}

	wchar_t text[1024];
	wstring str;

	XWorld* pWorld = gg.currentgamestate->GetWorld();

	wstring strField, strTime, strWeather;
	int nFieldID = 0;
	vec3 vPos = vec3::ZERO;
	GAME_TIME_TYPE nGameTime;
	GAME_WEATHER_TYPE nWeatherType;

	strField = pWorld->GetInfo()->GetName();
	nFieldID = pWorld->GetInfo()->m_nFieldID;
	nGameTime = pWorld->GetWorldEnv()->GetCurrentTime();
	strTime = XStrings::GameTime(nGameTime);

	nWeatherType = pWorld->GetWorldEnv()->GetCurrentWeather();
	strWeather = XStrings::Weather(nWeatherType);

	if (gvar.Game.pMyPlayer)
	{
		vPos = gvar.Game.pMyPlayer->GetPosition();;
	}

	swprintf_s(text, L"Field: %s(%d) , Pos: (%.1f %.1f %.1f) , Time: %s(%d) , Weather: %s(%d)\n",
		strField.c_str(), nFieldID,
		vPos.x, vPos.y, vPos.z,
		strTime.c_str(), (int)nGameTime,
		strWeather.c_str(), (int)nWeatherType);

	str += text;


	return str;
}

wstring XDebugInfoBuilder::_Build_Character()
{
	wchar_t text[1024];
	wstring str;

	//캐릭터 이름, 캐릭터 레벨 , 성별 , 캐릭터 스탠스

	wstring strName, strSex, strStance;
	int nLevel = 0;

	GlobalVar_MyInfo& myInfo = gvar.MyInfo;
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;

	if (pMyPlayer == NULL)
	{
		return L"";
	}

	strName = myInfo.ChrInfo.szName;
	nLevel = myInfo.nLevel;

	strSex = XStrings::Sex(myInfo.ChrInfo.nSex);
	strStance = XStrings::Stance(pMyPlayer->GetStance());

	swprintf_s(text, L"Name: %s , Level: %d , Sex: %s , Stance: %s\n",
		strName.c_str(),
		nLevel,
		strSex.c_str(),
		strStance.c_str());
	str += text;

	return str;
}

wstring XDebugInfoBuilder::GetInfoForClipboard()
{
	wstring strBuf;
	strBuf = L"{{{\n" + m_strBuf + L"}}}\n";
	return strBuf;
}
