#include "stdafx.h"
#include "XPvPAreaLogic.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XPvPAreaInfo.h"
#include "XPlayerPVP.h"
#include "XUISystem.h"
#include "XGame.h"
#include "XStrings.h"
#include "XWorld.h"
#include "CSFactionCalculator.h"
#include "XPost_QPVP.h"
#include "XUIMiniMap.h"


bool XPvPAreaLogic::IsInPvPArea()
{
	return m_PvPAreaInfo.IsInPvPArea();
}

void XPvPAreaLogic::SetupEnemy( MUID uidPlayer )
{
	if (IsInPvPArea())
	{
		XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
		if (pMyPlayer == NULL) return;

		XNetPlayer* pPlayer = gg.omgr->FindNetPlayer(uidPlayer);
		if (pPlayer)
		{
			if (pPlayer->IsAlly(pMyPlayer) == false)
			{
				wstring strEnemyName = MakeEnemyName(pPlayer);
				pPlayer->ChangeNameForPvP(strEnemyName);
				pPlayer->SetCharacterPane_VSNetPlayer();
			}
		}
	}
}

wstring XPvPAreaLogic::MakeEnemyName( XNetPlayer* pNetPlayer )
{
	wstring strName;

	if (pNetPlayer->GetPlayerPVP()->GetPvPAreaTeam() == XPlayerPVP::ePvPAreaTeam_1)
	{
		strName =  CSFormatString::Format( XGetStr( L"DUEL_MERCERNARY"), FSParam( m_PvPAreaInfo.GetTeam1FactionName()));
	}
	else if (pNetPlayer->GetPlayerPVP()->GetPvPAreaTeam() == XPlayerPVP::ePvPAreaTeam_2)
	{
		strName =  CSFormatString::Format( XGetStr( L"DUEL_MERCERNARY"), FSParam( m_PvPAreaInfo.GetTeam2FactionName()));
	}
	else
	{
		strName = pNetPlayer->GetName();
	}

	return strName;
}

void XPvPAreaLogic::Update( float fDelta, XPlayerFactions& myPlayerFactions )
{
	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo == NULL) return;

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if (pMyPlayer == NULL) return;

	vec3 vPos = pMyPlayer->GetPosition();

	if (m_PvPAreaChecker.CheckPvPStateChange(fDelta, pFieldInfo, vPos))
	{
		bool bPvPArea = m_PvPAreaChecker.IsCurrentPvPArea();

		if (bPvPArea)
		{
			SetMyPlayerInPvPArea(pMyPlayer, myPlayerFactions);

			XPostQPVP_EnterPvp();
		}
		else
		{
			m_PvPAreaInfo.SetOutPvPArea();
			pMyPlayer->GetPlayerPVP()->ReleasePvPAreaFaction();

			if ( global.script)
				global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "END");


			XUIMiniMap::SetMissionTarget( 0);
		}

		RefreshNetPlayers( bPvPArea);

		OutputNarration(bPvPArea);
	}
}

void XPvPAreaLogic::SetMyPlayerInPvPArea( XMyPlayer* pMyPlayer, XPlayerFactions& PlayerFactions )
{
	PVP_AREA_INFO& pvpAreaInfo = m_PvPAreaChecker.GetCurrentPvPArea();

	uint16 nFaction1Quantity = 0;
	uint16 nFaction2Quantity = 0;
	bool bFaction1 = false;
	bool bFaction2 = false;

	if (PlayerFactions.IsExist(pvpAreaInfo.nFaction1))
	{
		nFaction1Quantity = PlayerFactions.GetQuantity(pvpAreaInfo.nFaction1);

		if (CSFactionCalculator::IsNormalThanRelation(nFaction1Quantity))
		{
			bFaction1 = true;
		}
	}
	if (PlayerFactions.IsExist(pvpAreaInfo.nFaction2))
	{
		nFaction2Quantity = PlayerFactions.GetQuantity(pvpAreaInfo.nFaction2);
		if (CSFactionCalculator::IsNormalThanRelation(nFaction2Quantity))
		{
			bFaction2 = true;
		}
	}
	if (bFaction1 && bFaction2)
	{
		if (nFaction1Quantity >= nFaction2Quantity) bFaction2 = false;
		else bFaction1 = false;
	}

	m_PvPAreaInfo.SetInPvPArea(pvpAreaInfo.nFaction1, pvpAreaInfo.strFactionName1, 
		pvpAreaInfo.nFaction2, pvpAreaInfo.strFactionName2);
	pMyPlayer->GetPlayerPVP()->SetPvPAreaFaction(bFaction1, bFaction2);
}

void XPvPAreaLogic::RefreshNetPlayers( bool bInnerZone)
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if ( pMyPlayer == NULL)		return;

	for ( XObjectMap::iterator itr = gg.omgr->BeginItor();  itr != gg.omgr->EndItor();  itr++)
	{
		XObject* pObj = (*itr).second;
		if (pObj->IsPlayer() == false  ||  pObj == pMyPlayer) continue;

		XNetPlayer* pNetPlayer = AsNetPlayer( pObj);
		if (IsEnemy( pNetPlayer) == true)
		{
//			SetupEnemy(pObj->GetUID());

			wstring strEnemyName = MakeEnemyName( pNetPlayer);
			pNetPlayer->ChangeNameForPvP( strEnemyName);
			pNetPlayer->SetCharacterPane_VSNetPlayer();
		}
		else
		{
			pNetPlayer->RestoreNameForPvP();
			pNetPlayer->SetCharacterPane_Player();
		}
	}
}

bool XPvPAreaLogic::IsEnemy( XPlayer* pPlayer )
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if (pMyPlayer == NULL) return false;

	return (!pMyPlayer->IsAlly(pPlayer));
}

wstring XPvPAreaLogic::MakeEnemyChatMsg( const wchar_t* szOriginalChatMsg )
{
	char _buff[ 1024];
	memset( _buff, 0, sizeof( _buff));

	size_t _len = wcslen( szOriginalChatMsg);
	for ( size_t i = 0;  i < _len;  i++)
	{
		if ( szOriginalChatMsg[ i] == L' ')		_buff[ i] = ' ';
		else
		{
			int _seed = rand() % 10;
			switch ( _seed)
			{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :	_buff[ i] = XMath::RandomNumber( 'a', 'z');		break;
			
			case 6 :
			case 7 :
			case 8 :	_buff[ i] = XMath::RandomNumber( 'A', 'Z');		break;
			
			case 9 :	_buff[ i] = XMath::RandomNumber( '0', '9');		break;
			}
		}
	}

	return MLocale::ConvAnsiToUTF16( _buff);
}

void XPvPAreaLogic::OutputNarration( bool bPvPArea )
{
	if (global.ui)
	{
		wstring strNarration;
		if (bPvPArea)	strNarration = XGetStr(L"IN_PVP_AREA_NARRATION");
		else			strNarration = XGetStr(L"OUT_PVP_AREA_NARRATION");

		//			wstring strPvP = "PVP Áö¿ª"

		global.ui->OnPresentationUpper(strNarration);
	}
}


wstring XPvPAreaLogic::GetTeamName( int _team)
{
	switch ( (XPlayerPVP::XPvPAreaTeam)_team)
	{
	case XPlayerPVP::ePvPAreaTeam_NA :
		break;

	case XPlayerPVP::ePvPAreaTeam_1 :
		return m_PvPAreaInfo.GetTeam1FactionName();

	case XPlayerPVP::ePvPAreaTeam_2 :
		return m_PvPAreaInfo.GetTeam2FactionName();
	}

	return L"";
}
