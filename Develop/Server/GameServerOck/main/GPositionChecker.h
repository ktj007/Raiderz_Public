#pragma once

#include "GDef.h"

enum POSITION_CHECK_TYPE
{
	PCT_INVALID = -1,
	PCT_WALK	= 0,
	PCT_MOTIONFACTOR,
	PCT_WARP,
};

//
//		GPositionChecker
//
//	이동 치팅(스피드핵등) 방지를 위한 클래스.
//	플레이어마다 하나의 인스턴스를 갖는다.
//
class GPositionChecker
{
public:
	GPositionChecker();

	// 플레이어 인스턴스가 초기화될때 같이 초기화되는 함수
	void Init(vec3 vCurPos);

	// 이동이 유효한지 확인.  확인하고 이전정보로써 저장.  
	void Check(POSITION_CHECK_TYPE nType, vec3 vCurPos, float fMoveSpeed, POSITION_CHEAT_TYPE& outnPosCheatType, CHEAT_TREAT_TYPE& outnTreatType );
private:
	// 플레이어가 직접 걸어서 이동했을때 호출.  반환값은 치팅을 했는지 여부
	POSITION_CHEAT_TYPE Check_Walk( vec3 vCurPos, float fMoveSpeed, CHEAT_TREAT_TYPE& outnTreatType );
	// 워프를 통해서 이동 됐을때 호출.  반환값은 치팅을 했는지 여부.
	// 서버에 의한 워프이므로 항상 올바르다.
	POSITION_CHEAT_TYPE Check_Warp(vec3 vCurPos, float fMoveSpeed);

	// 모션팩터를 통해서 이동 됐을때 호출.  반환값은 치팅을 했는지 여부.
	// 서버에 의한 워프이므로 항상 올바르다.
	POSITION_CHEAT_TYPE Check_MotionfactorMove(vec3 vCurPos, float fMoveSpeed);

	// 마지막으로 사용했던 치팅정보를 저장
	void SaveLastCheckInfo(vec3 vCurPos);

	// 심각한 치팅 시도인지 확인
	bool CheckDeepSpeedhack();
	// 얕은 치팅 시도인지 확인
	bool CheckShallowSpeedhack();
	// 접속해제 시키기
	void Disconnect();
	
private:
	// 마지막으로 치팅 체크를 확인한 시각
	DWORD	m_dwLastCheckTime;

	// 마지막으로 치팅 체크를 했을때의 플레이어 위치
	vec3	m_vLastPos;

	// 마지막으로 심각한 치팅 검출된 시각
	DWORD	m_dwLastShallowSpeedhackTime;
	// 최근 심각한 치팅 검출된 횟수
	int		m_nShallowSpeedhackCount;

	// 마지막으로 심각한 치팅 검출된 시각
	DWORD	m_dwLastDeepSpeedhackTime;
	// 최근 심각한 치팅 검출된 횟수
	int		m_nDeepSpeedhackCount;
};
