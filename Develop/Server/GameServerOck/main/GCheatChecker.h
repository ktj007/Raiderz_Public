#pragma once

#include "GPositionChecker.h"
#include "GDef.h"

class GEntityPlayer;


//
//		GCheatChecker
//
//	치팅 방지 클래스들을 관리하는 클래스
//
class GCheatChecker
{
public:
	GCheatChecker(void);
	virtual ~GCheatChecker(void);

	// 플레이어 인스턴스가 초기화될때 같이 초기화되는 함수
	void Init(GEntityPlayer* pPlayer);

	// 이동관련을 한후 꼭 불려야함
	void CheckPosition(POSITION_CHECK_TYPE nType, GEntityPlayer* pPlayer);

	void Update(float fDelta);

protected:
	// 치팅
	virtual void OnLog_Disconnect( CHEAT_TYPE nCheatType, int nCheatSubType, GEntityPlayer* pPlayer );
	virtual void OnLog_Warning( CHEAT_TYPE nCheatType, int nCheatSubType, GEntityPlayer* pPlayer );

	// 처리하기
	void OnTreat( CHEAT_TYPE nCheatType, int nCheatSubType, CHEAT_TREAT_TYPE nTreat, GEntityPlayer* pPlayer );
	
private:
	// 이동관련 치팅체크를 위한 인스턴스
	GPositionChecker	m_PositionChecker; 
	MRegulator			m_rgrTick;
	bool				m_bCheckable;
};
