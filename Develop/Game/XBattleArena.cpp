#include "stdafx.h"
#include "XBattleArena.h"
#include "XBattleArenaInfo.h"
#include "XContainer.h"
#include "XNetPlayer.h"
#include "XGlobalVar_MyInfo.h"
#include "XGame.h"
#include "XWidgetNameDef.h"
#include "XStrings.h"

void XBattleArena::Start(MUID* pEntryUIDs, int nUIDCount)
{
	gvar.MyInfo.BattleArenaInfo.SetInBattle();

	// TODO : 0 : ·¹µåÆÀ, 1 : ºí·çÆÀ
	if (global.ui)
	{
		wstring strTeam;
		
		if (gvar.MyInfo.Party.GetUID() == pEntryUIDs[0])		strTeam = XGetStr( L"BATTLEARENA_REDTEAM");
		else													strTeam = XGetStr( L"BATTLEARENA_BLUETEAM");
		
		wstring strMsg = CSFormatString::Format( XGetStr( L"BATTLEARENA_BEGIN"), FSParam(strTeam));
		global.ui->OnPresentationUpper(strMsg);
	}

	vector<MUID> vecPlayer;
	gg.omgr->GetEntityTypeList(vecPlayer, ETID_PLAYER);

	for each (MUID uid in vecPlayer)
	{
		InNetPlayer(uid);
	}

	global.script->GetGlueGameEvent().OnShowBattleArenaRealTimeScore();
	global.script->GetGlueGameEvent().OnShowBattleScore();	
}

void XBattleArena::Finish()
{
	if (global.script)
	{
		global.ui->OnPresentationUpper( XGetStr( L"BATTLEARENA_END"));
	}

	gvar.MyInfo.BattleArenaInfo.SetOutBattle();
	global.script->GetGlueGameEvent().OnHideBattleArenaRealTimeScore();
}

void XBattleArena::InNetPlayer( MUID uidPlayer )
{
	if (IsInBattleArena())
	{
		bool bMyTeam = false;
		if(gvar.MyInfo.Party.Find(uidPlayer)) //ÆÄÆ¼ ¸â¹öÀÌ¸é ¿ì¸®Æí
		{
			bMyTeam = true;
		}


		if (bMyTeam == false)
		{
			XNetPlayer* pPlayer = gg.omgr->FindNetPlayer(uidPlayer);
			if (pPlayer)
			{
				pPlayer->SetCharacterPane_VSNetPlayer();
				pPlayer->SetBattleArenaEnemy();
			}
		}
	}
}

bool XBattleArena::IsInBattleArena()
{
	return gvar.MyInfo.BattleArenaInfo.IsInBattleArena();
}

void XBattleArena::SetMaxLife( int nLife )
{
	gvar.MyInfo.BattleArenaInfo.SetMaxLife(nLife);

	global.script->GetGlueGameEvent().OnSetBattleArenaRealTimeScore(nLife, nLife);
	global.script->GetGlueGameEvent().OnRefreshBattleScore();
}

void XBattleArena::SetCurrentScore( int* pScores)
{
	SetCurrentScore( pScores[0], pScores[1] );
}

void XBattleArena::SetCurrentScore( int nRed, int nBlue )
{
	gvar.MyInfo.BattleArenaInfo.SetScore(nRed, nBlue);

	global.script->GetGlueGameEvent().OnShowBattleArenaRealTimeScore();
	global.script->GetGlueGameEvent().OnSetBattleArenaRealTimeScore(nRed, nBlue);

	global.script->GetGlueGameEvent().OnShowBattleScore();
	global.script->GetGlueGameEvent().OnRefreshBattleScore();
}

MWLua::ret_int2	XBattleArena::GetCurrentScore() const
{
	int* score = gvar.MyInfo.BattleArenaInfo.GetScore();
	return MWLua::ret_int2( score[0], score[1] );
}

void XBattleArena::ReJoin()
{
	MUID* pEntryUIDs = gvar.MyInfo.BattleArenaInfo.GetUidTeam();

	// TODO : 0 : ·¹µåÆÀ, 1 : ºí·çÆÀ
	if (global.ui)
	{
		wstring strTeam;

		if (gvar.MyInfo.Party.GetUID() == pEntryUIDs[0])		strTeam = XGetStr( L"BATTLEARENA_REDTEAM");
		else													strTeam = XGetStr( L"BATTLEARENA_BLUETEAM");

		wstring strMsg = CSFormatString::Format( XGetStr( L"BATTLEARENA_PROGRESS"), FSParam(strTeam));
		global.ui->OnPresentationUpper(strMsg);
	}

	vector<MUID> vecPlayer;
	gg.omgr->GetEntityTypeList(vecPlayer, ETID_PLAYER);

	for each (MUID uid in vecPlayer)
	{
		InNetPlayer(uid);
	}

	global.script->GetGlueGameEvent().OnShowBattleArenaRealTimeScore();

	SetMaxLife(gvar.MyInfo.BattleArenaInfo.GetMaxLife());
	SetCurrentScore(gvar.MyInfo.BattleArenaInfo.GetScore());
}

bool XBattleArena::IsVisibleScoreBoard()
{
	MFrame* pScore = (MFrame*)global.mint->FindWidget( WIDGET_NAME_BATTLE_ARENA_SCORE);
	if (pScore == NULL) return false;
	return pScore->GetShow();
}

void XBattleArena::SetResultScore( float fWaitSecond, MUID uidWinner, int nWinnerScore, int nLoserScore, float fGameElaspedTime )
{

}