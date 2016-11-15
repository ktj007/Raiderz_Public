#pragma once

#include "XPvPAreaInfo.h"
#include "XPvPAreaChecker.h"
#include "XPlayerFaction.h"

class XNetPlayer;

/// 공용필드의 PVP 지역에 관한 로직 처리 클래스
class XPvPAreaLogic
{
protected:
	XPvPAreaInfo			m_PvPAreaInfo;		///< PvP 지역 정보
	XPvPAreaChecker			m_PvPAreaChecker;

	void SetMyPlayerInPvPArea( XMyPlayer* pMyPlayer, XPlayerFactions& PlayerFactions );
	void OutputNarration( bool bPvPArea );
public:
	XPvPAreaLogic() {}
	~XPvPAreaLogic() {}

	wstring MakeEnemyName(XNetPlayer* pNetPlayer);
	void SetupEnemy( MUID uidPlayer );
	void Update( float fDelta, XPlayerFactions& myPlayerFactions );

	void RefreshNetPlayers( bool bInnerZone);

	bool IsInPvPArea();
	bool IsEnemy(XPlayer* pPlayer);
	wstring MakeEnemyChatMsg(const wchar_t* szOriginalChatMsg);

	wstring GetTeamName( int _team);
};


