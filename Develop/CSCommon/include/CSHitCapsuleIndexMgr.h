#pragma once

#include "MTypes.h"
#include "CSDef.h"
#include "CSCommonLib.h"

class CSCOMMON_API CSHitCapsuleIndexMgr
{
public:
	CSHitCapsuleIndexMgr();

	// 현재 상황에 맞는 캡슐그룹 아이디 반환
	int8	GetGroupIndex();
	// NPC모드에 맞는 캡슐그룹 아이디 넣기
	void	InsertNPCModeGroupIndex(int nMode, int8 nGroupID);
	// 탤런트에 의한 변경
	void	Change_ByTalent(int8 nGroupID);
	// 탤런트 종료로 인해 사용 안하게 수정
	void	Change_ByTalentComplete();
	// 모드변환에 의한 변경
	void	ChangeNPCMode(int nMode);
	// 트리거(COLT,Lua) 의한 변경, 현재 모드의 캡슐그룹 아이디를 영구히 바꾼다.
	void	Change_ByTrigger(int8 nGroupID);
	// 모든 멤버 정보를 초기화
	void	Clear();
private:
	// 현재모드에 맞는 캡슐그룹 아이디 반환
	int8	GetModeGroupID();
private:
	typedef map<int, int8> MAP_MODE;	// pair<nModeID, nCapsuleGroupID>
	// 모드 변환시 변경 되는 캡슐그룹 맵
	MAP_MODE	m_mapMode;
	// NPC의 현재 모드
	int			m_nCurNPCMode;
	// 탤런트 사용시 변경되는 캡슐그룹
	bool		m_bUsingTalent;
	int8		m_nTalent;
};
