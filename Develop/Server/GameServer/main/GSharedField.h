#pragma once

#include "GField.h"
#include "MMemPool.h"

class GAutoPartyMgr;

//////////////////////////////////////////////////////////////////////////
// 
// GSharedField
//
//////////////////////////////////////////////////////////////////////////

class GSharedField : public GField
{
public:
	GSharedField(const MUID& uid, GWeatherMgr* pWeatherMgr);
	virtual ~GSharedField();

	// 공유필드인 여부
	virtual bool		IsSharedField() override { return true; }
	// 무효한 공유필드인지 여부
	bool				IsInvalid() const;
	// 프라임 공유필드인지 여부 (삭제 안됨)
	bool				IsDefaultChannel() const;

	// 채널아이디 지정
	void				SetChannelID(int id)	{ m_nChannelID = id; }
	// 채널아이디 반환
	int					GetChannelID()	const	{ return m_nChannelID; }

	// 참가 가능한 플레이어 최대인원
	int					GetCapacity();

	// 필드에 진입할 수 있는지 여부
	virtual bool		IsEnterable();

	// 채널이동 예약 - StandAlone Mode 전용
	void				ReserveChangeChannel_ForStandAlone(MUID uidPlayer);
	// 채널이동 예약취소 - StandAlone Mode 전용
	void				CancelChangeChannel_ForStandAlone(MUID uidPlayer);
	// 채널이동 예약이 되어있는지 여부 - StandAlone Mode 전용
	bool				HasReservation_ForStandAlone( MUID uidPlayer );

	// 자동파티 관리자 반환
	GAutoPartyMgr*	 GetAutoPartyMgr() const { return m_pAutoPartyMgr; }
	
	virtual void		Update(float fDelta) override;
private:
	// 스스로 삭제 됨 (적당한 조건이 되었을 경우)
	void DestroySelf();

	// 엔티티가 제거될때 호출
	virtual void RemoveEntity(GEntity* pEntity) override;

	// 엔티티가 떠날때
	virtual void OnEntityLeaved(GEntity* pEntity);
	
private:
	// 채널 아이디
	CHANNELID m_nChannelID;
	GAutoPartyMgr*	m_pAutoPartyMgr;
};

class GSharedFieldMLeaf : public GSharedField, public MMemPool<GSharedFieldMLeaf>
{
public:
	GSharedFieldMLeaf(const MUID& uid, GWeatherMgr* pWeatherMgr)
		: GSharedField(uid, pWeatherMgr) {}
	virtual ~GSharedFieldMLeaf() {}
};

// Field를 SharedField로 바꿔줌
GSharedField*	ToSharedField(GField* pField);		
// Field의 채널아이디를 반환, 채널을 알 수 없는 경우라면 INVALID_CHANNELID 반환
CHANNELID		GetChannelID(GField* pField);