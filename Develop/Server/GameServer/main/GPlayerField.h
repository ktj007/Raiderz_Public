#pragma once

#include "CSTriggerInfo.h"
#include "GGateInfo.h"
#include "GPlayerFieldEntry.h"

class GEntityActor;
class GEntityPlayer;
class GPlayerFieldListener;
class GPlayerFieldMasterServer;
class GPlayerFieldShared;
class GPlayerFieldDynamic;
class GPlayerFieldLazyGateway;
class GPlayerFieldGateRetryer;

/// 플레이어의 필드 이동 로직
class GPlayerField : public MTestMemPool<GPlayerField>
{
public:
	GPlayerField(GEntityPlayer* pOwner);
	~GPlayerField(void);

	void SetLisener(GPlayerFieldListener* pLisener);
	void Update(float fDelta=0.0f);

	bool Gate(int nDestFieldID, vec3 vecDestPos, vec3 vecDestDir, bool bRoute=true, CHANNELID nChannelID = INVALID_CHANNELID);
	bool GateToMarker(int nFieldID, int nMarkerID);
	bool GateToMarker(const MARKER_INFO& refMarker, bool bRouteToMe = true);
	bool GateToActor(GEntityActor* pActor);
	bool GateToSoulBidning();
	bool GateToTrial(int nDynamicFieldGroupID, bool bSingleEnter = false);
	bool GateToInnRoom();
	bool GateToPlayer(wstring strTarPlayerName);
	bool Summon(wstring strTarPlayerName);

	void Warp(vec3 vecDestPos, vec3 vecDestDir, bool bRoute=true);

	MUID GetDynamicFieldGroupUID() { return m_GateInfo.uidDynamicFieldGroup; }
	void SetDynamicFieldGroupUID(const MUID& uidDynamicFieldGroup);
	
	void SetGateInfo(const GGateInfo& GateInfo)		{ m_GateInfo = GateInfo; }
	GGateInfo GetGateInfo(void)						{ return m_GateInfo; }

	GPlayerFieldEntry&			GetFieldEntry()			{ return m_FieldEntry; }
	GPlayerFieldMasterServer&	GetFieldMasterServer()	{ return *m_pFieldMasterServer; }
	GPlayerFieldShared&			GetFieldShared()		{ return *m_pFieldShared; }
	GPlayerFieldDynamic&		GetFieldDynamic()		{ return *m_pFieldDynamic; }
	GPlayerFieldLazyGateway&	GetLazyGateway()		{ return *m_pFieldLazyGateway; }
	GPlayerFieldGateRetryer&	GetFieldGateRetryer()	{ return *m_pFieldGateRetryer; }

	// 개발용 함수: 서비스 버전에선 항상 실패합니다. --------------------------------------
	bool GateToRequestField(int nFieldId, vec3 vecDestPos, vec3 vecDestDir = vec3(0, 1, 0));
	bool GateToFirstField(void);
	bool GateAnywhere(void);
	// ------------------------------------------------------------------------------------

private:
	GEntityPlayer*				m_pOwner;				///< 소유자
	GPlayerFieldListener*		m_pLisener;				///< 리스너

	GPlayerFieldEntry			m_FieldEntry;			///< 필드 진입 처리
	GPlayerFieldMasterServer*	m_pFieldMasterServer;	///< 마스터서버 경유하여 필드 이동 처리
	GPlayerFieldShared*			m_pFieldShared;			///< 공유 필드, 채널 이동 처리
	GPlayerFieldDynamic*		m_pFieldDynamic;		///< 동적 필드 이동 처리
	GPlayerFieldLazyGateway*	m_pFieldLazyGateway;	///< 예약 이동 관리자
	GPlayerFieldGateRetryer*	m_pFieldGateRetryer;	///< 필드 이동 재시도 요청자

	GGateInfo					m_GateInfo;				///< 게이트 정보

	bool	IsSameField(int nDestFieldID, CHANNELID nChannelID);
	bool	IsDynamicField(int nDestFieldID);

	bool	GateToSharedFIeld(int nFieldID, vec3 vecDestPos, vec3 vecDestDir, CHANNELID nChannelID=INVALID_CHANNELID);

	DYNAMIC_FIELD_TYPE GetDynamicFieldGroupType(MUID uidFieldGroup);
	int GetDynamicFieldGroupID(MUID uidFieldGroup);
};
