#pragma once

#include "GGateInfo.h"

class GPlayerField;
class GEntityPlayer;
class GPlayerFieldListener;
class GSharedField;

/// 플레이어의 채널 이동 처리
class GPlayerFieldShared : public MTestMemPool<GPlayerFieldShared>
{
private:
	GPlayerField*			m_pPlayerField;
	GEntityPlayer*			m_pOwner;					///< 소유자
	GPlayerFieldListener*	m_pLisener;					///< 리스너

	uint32					m_nPrepareChannelTime;		///< 채널 변경하기로 예정한 시각
	CHANNELID				m_nPrepareChannelID;		///< 변경할 채널 ID

	GSharedField*			GetEnterableSharedFIeld(int nReqFieldID, int nReqChannelID);

public:
	GPlayerFieldShared(GEntityPlayer* pOwner, GPlayerField* pPlayerField);
	~GPlayerFieldShared() {}

	void		SetListener(GPlayerFieldListener* pLisener)	{ m_pLisener = pLisener; }

	// 채널을 바꾸기전에 준비
	void		PrepareChangeChannel(CHANNELID nChannelID);
	// 채널 이동 중
	bool		IsPreparedChangeChannel();
	// 채널이동 요청 시각 초기화
	void		ClearPrepareChannelChange();

	// 채널이동 요청이 온 시각을 반환
	uint32		GetPrepareChannelTime()						{ return m_nPrepareChannelTime; }
	// 채널이동 요청이 온 채널 아이디 반환
	CHANNELID	GetPrepareChannelID()						{ return m_nPrepareChannelID; }

	bool		GateRequest(int nReqFieldID, int nReqChannelID, vec3 vecDestPos, vec3 vecDestDir);
	bool		GateExecute(GSharedField* pField, vec3 vecDestPos, vec3 vecDestDir);

};
