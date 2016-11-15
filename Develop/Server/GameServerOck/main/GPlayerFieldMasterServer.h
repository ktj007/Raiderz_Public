#pragma once

#include "GGateInfo.h"
#include "STransData_M2G.h"
#include "CTransData.h"

class GEntityPlayer;
class GPlayerField;


/// 플레이어의 마스터 서버를 경유한 필드 이동 처리
class GPlayerFieldMasterServer : public MTestMemPool<GPlayerFieldMasterServer>
{
private:
	GEntityPlayer*				m_pOwner;					///< 소유자
	GPlayerField*				m_pPlayerField;

	GGateInfo					m_DestGateInfo;				///< 이동할 위치 정보
	TD_LOGIN_GAME_SERVER_INFO	m_LoginGameServerInfo;		///< 이동 목표서버 정보

	bool MakeDestGateInfo(GGateInfo& refGateInfo);
	bool MakeDestGateInfo(int nFieldID, vec3 vPos, vec3 vDir, int nChannelID);
	bool MakeDestGateInfo_FieldGroup(int nFieldID, vec3 vPos, vec3 vDir, MUID uidFieldGroup);

public:
	GPlayerFieldMasterServer(GEntityPlayer* pOwner, GPlayerField* pPlayerField);
	~GPlayerFieldMasterServer() {}

	// AskGate
	bool AskGateSharedField(int nFieldID, vec3 vPos, vec3 vDir, int nChannelID = INVALID_CHANNELID);
	bool AskGateDynamicField(int nFieldID, vec3 vPos, vec3 vDir, int nFieldGroupID, MUID uidFieldGroup, DYNAMIC_FIELD_TYPE eType, bool isSingleType);
	bool AskGateChangeChannel(int nFieldID, vec3 vPos, vec3 vDir, int nChannelID);

	// MoveServer
	MUID ReadyMoveServer(int nTargetGameServerID, GGateInfo& refGateInfo);
	void SetMoveServerDestInfo(TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo);
	void GoMoveServer(void);

	// Gate
	bool ExecuteGate(const GGateInfo* pGateInfo);
	bool StartRetryGate(const GGateInfo* pGateInfo);
	bool RemainField();
	void FailGate(const GGateInfo* pGateInfo);

	// geter
	const GGateInfo						GetDestGateInfo(void)				{ return m_DestGateInfo;	}
	const TD_LOGIN_GAME_SERVER_INFO*	GetLoginGameServerInfo(void) const	{ return &m_LoginGameServerInfo;	}
};
