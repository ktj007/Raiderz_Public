#include "stdafx.h"
#include "XGlueGameEvent.h"
#include "XBattleArenaInfo.h"
#include "XBaseApplication.h"
#include "XLoginFrame.h"
#include "XHelp.h"

XGlueGameEvent::XGlueGameEvent() : m_pMWLua(NULL)
{

}

XGlueGameEvent::~XGlueGameEvent()
{
	m_pMWLua = NULL;
}

void XGlueGameEvent::Init( MWLua* pLua )
{
	m_pMWLua = pLua;
}

bool XGlueGameEvent::OnLoginEvent( string strMsg, string sParam /*=0*/, int nParam1 /*=0*/, int nParam2 /*=0*/)
{
	if ( global.app  &&  global.app->GetCurrGameFrame() != LOGIN_FRAME_ID)
		return false;

	bool bRetVal = false;
	WCALL4( m_pMWLua, "OnLoginEvent", strMsg.c_str(), sParam.c_str(), nParam1, nParam2, &bRetVal);
	return bRetVal;
}

bool XGlueGameEvent::OnCharacterEvent( string strMsg, string sParam /*=0*/, int nParam1 /*=0*/, int nParam2 /*=0*/)
{
	if ( global.app  &&  global.app->GetCurrGameFrame() != CHAR_FRAME_ID)
		return false;

	bool bRetVal = false;
	WCALL4( m_pMWLua, "OnCharacterEvent", strMsg.c_str(), sParam.c_str(), nParam1, nParam2, &bRetVal);
	return bRetVal;
}

bool XGlueGameEvent::OnGameEvent( string strMsg, string sParam /*=0*/, int nParam1 /*=0*/, int nParam2 /*=0*/)
{
	if ( global.app  &&  global.app->GetCurrGameFrame() != PLAY_FRAME_ID)
		return false;

	bool bRetVal = false;
	WCALL4( m_pMWLua, "OnGameEvent", strMsg.c_str(), sParam.c_str(), nParam1, nParam2, &bRetVal);
	return bRetVal;
}

void XGlueGameEvent::OnInteractonWindowsClose()
{
	global.script->GetGlueGameEvent().OnGameEvent( "SHOP", "END");
	global.script->GetGlueGameEvent().OnGameEvent( "ACCEPT_QUEST", "END");
	global.script->GetGlueGameEvent().OnGameEvent( "INTERACTION_NPC", "END");
	global.script->GetGlueGameEvent().OnGameEvent( "DIALOG_NPC", "END");
}

void XGlueGameEvent::OnShowChannelWindow( int count, int* nChannel )
{
	MWLua::table t(m_pMWLua->GetState());

	char buff[128]="";
	for (int i = 0; i < count; i++)
	{
		_itoa_s(i+1, buff, 10);
		t.set(buff, nChannel[i] );
	}

	WCALL1(m_pMWLua, "shGameEvent_ShowChannelWindow", t, WNULL);
}

void XGlueGameEvent::OnTrainerToAllTalent( const char* strText )
{
	string szText = "shGameEvent_OnTrainerToAllTalent(";
	szText += strText;
	szText += ")";
	WRUNSTRING(m_pMWLua, szText.c_str());
}

void XGlueGameEvent::OnRefreshTalentTrainer( const char* strText )
{
	string szText = "shGameEvent_OnRefreshTalentTrainer(";
	szText += strText;
	szText += ")";
	WRUNSTRING(m_pMWLua, szText.c_str());
}

void XGlueGameEvent::OnPartyWindowRefresh( const char* strText )
{
	string szText = "shGameEvent_PartyWindowRefresh(";
	szText += strText;
	szText += ")";
	WRUNSTRING(m_pMWLua, szText.c_str());
}

// float 이시간동안 스코어 보드를 보여준다.
// bool  이겼다 졋다
// int   이긴팀 스코어
// int   진팀 스코어
// flost 투기장 게임 시간
void XGlueGameEvent::OnShowBattleScore(float fWaitSecond, MUID uidWinner, int nWinnerScore, int nLoserScore, float fGameElaspedTime)
{
	bool bWin = false;

	if (gvar.MyInfo.Party.GetUID() == uidWinner)
	{
		bWin = true;
	}

	MWLua::table t(m_pMWLua->GetState());

	char buff[128]="";
	_itoa_s(1, buff, 10);
	t.set(buff, fWaitSecond );
	_itoa_s(2, buff, 10);
	t.set(buff, bWin);
	_itoa_s(3, buff, 10);
	t.set(buff, nWinnerScore);
	_itoa_s(4, buff, 10);
	t.set(buff, nLoserScore);
	_itoa_s(5, buff, 10);
	t.set(buff, fGameElaspedTime);

	WCALL1(m_pMWLua, "shGameEvent_ShowBattleScore", t, WNULL);
}

void XGlueGameEvent::OnFetchFeedback( vector<string> vecTextList )
{
	MWLua::table t(m_pMWLua->GetState());

	char buff[128]="";
	for (size_t i = 0; i < vecTextList.size(); i++)
	{
		_itoa_s(i+1, buff, 10);

		static char text[1024];	// 루아로 넘길 때 지역변수이면 안된다.
		sprintf_s(text, "%s", vecTextList[i].c_str());
		t.set(buff, text );
		
	}

	WCALL1(m_pMWLua, "shGameEvent_FetchFeedback", t, WNULL);
}

int XGlueGameEvent::GetVisibledWindow()
{
	if ( global.app  &&  global.app->GetCurrGameFrame() != PLAY_FRAME_ID)
		return false;

	int nCount = 0;
	WCALL( m_pMWLua, "GetVisibledWindow", &nCount);
	return nCount;
}

bool XGlueGameEvent::OnTutorialTrigger( const string& strTutorialType, int nHelpID )
{
	return OnGameEvent( "HELP", strTutorialType, nHelpID ); 
}

bool XGlueGameEvent::OnTutorialTriggerClose( const string& strTutorialType )
{
	return OnGameEvent( "CLOSEHELP", strTutorialType ); 
}

bool XGlueGameEvent::IsShowUI( const string& strUIName )
{
	return OnGameEvent( "ISSHOWUI", strUIName ); 
}

bool XGlueGameEvent::IsUpdateTutorial()
{
	return OnGameEvent( "ISUPDATETUTORIAL" ); 
}