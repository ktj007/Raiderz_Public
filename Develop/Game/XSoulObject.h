#ifndef _XSOUL_OBJECT_H
#define _XSOUL_OBJECT_H

#include "XObject.h"

using namespace rs3;


//	#define TEST_SOUL									// 테스트 모드 사용할 때 사용

//	#define SHOW_SPLASH_EFFECT							// 소울이 뽑혀 나오는 이펙트 사용
	#define SHOW_ABSORB_EFFECT							// 소울이 흡수 될 때 나오는 이펙트 사용




// SoulUpdateInfo
struct SoulUpdateInfo
{
	XObject*	pTarget;
	vec3		dest_pos;
	vec3		effect_pos;
	DWORD		curr_time;
	DWORD		elipsed;
	float		radian;
	vec3		toler;
};



// XSoulObject
class XSoulObject : public XObject, public MMemPool<XSoulObject>
{
	MDeclareRTTI;

private:
	XObject*		m_pNPC;								// 소울을 방출할 NPC 포인터
	MUID			m_uidTarget;						// 타겟
	MUID			m_uidEffectSoul;					// 소울 이펙트

#ifdef SHOW_SPLASH_EFFECT
	MUID			m_uidEffectSplash;					// 소울 방출 이펙트
#endif

#ifdef SHOW_ABSORB_EFFECT
	MUID			m_uidEffectAbsorb;					// 소울 흡수 이펙트
#endif

	int				m_nSoulType;						// 소울 타입
	vec3			m_vPos;								// 소울 기준 위치 좌표

	bool			m_bWaiting;							// 처음 생성 후 일정 시간 대기하고 있는지 여부
	bool			m_bArrival;							// 소울이 목적지에 도착했는지 여부
	bool			m_bBackRush;						// 소울이 목적지와 가까운 거리에서 생성될 경우 뒤쪽으로 날아가는지 여부

	vec3			m_vFlyingFactor;					// 소울의 비생 관련 랜덤 씨드

	DWORD			m_dwTime;							// 타이머

	float			m_fMoveSpeed;						// 비행 속도

	vec3			m_vStartPos;						// 처음 시작 위치
	vec3			m_vStartDir;						// 처음 시작 방향

	unsigned int	m_nWaitTime;						// 소울 생성 전에 대기 할 시간(ms단위)


public:
	XSoulObject(MUID uid);
	virtual ~XSoulObject();
	virtual XObjectID				GetType() { return XOBJ_SWORD_TRAIL; }


	bool Create( XObject* pOwnerNpc, const MUID& uidTarget, int nType, unsigned int nWaitTime =0);


protected:
	void GetRandomFlyingFactor();
	void GetBackRushState();

	bool MakeSoulUpdateInfo( SoulUpdateInfo& info);

	virtual void OnDestroy();
	virtual void OnUpdate( float fDelta);

	bool OnWaiting( float fDelta, SoulUpdateInfo& info);
	bool OnUpdateMoveSoul( float fDelta, SoulUpdateInfo& info);
	bool OnUpdateEffect( float fDelta, SoulUpdateInfo& info);


#ifdef TEST_SOUL
	void RebirthTest();
#endif

};



#endif