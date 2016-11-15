#pragma once

#include "CSTriggerInfo.h"


class GEntityPlayer;
class GPlayerField;
class GDynamicFieldMaster;
class GPlayerFieldListener;
enum DYNAMIC_FIELD_TYPE;
struct GATE_INFO_DYNAMIC;

/// 플레이어의 동적 필드 이동 처리
class GPlayerFieldDynamic
{
protected:
	GPlayerField*			m_pPlayerField;
	GEntityPlayer*			m_pOwner;					///< 소유자
	GPlayerFieldListener*	m_pLisener;					///< 리스너

	CHANNELID				m_EnterChannelID;	///< 동적필드에 진입하기 전 공유필드의 채널ID
	MARKER_INFO				m_EnterPos;			///< 동적필드 진입지점
	MARKER_INFO				m_Checkpoint;		///< 동적필드에서 죽었을 때 부활되는 체크포인트

	MUID					GetEnterableDynamicFieldGroup(DYNAMIC_FIELD_TYPE eType, int nFieldGroupID, bool bIsSingleEnter);

	MUID					GetFieldGroupWhenReconnect();
	MUID					FindEnterableTrialField(int nFieldGroupID, bool bIsSingleEnter);
	MUID					FindEnterableBattleArena(int nFieldGroupID);

	bool					IsExistLocalServer(MUID uidFieldGroup);
	bool					IsValidFieldGroupWith(MUID uidDestFieldGroup, DYNAMIC_FIELD_TYPE eType, int nFieldGroupID);

	virtual void			_EndGateExecute(GDynamicFieldMaster* pFieldGroup) {}

public:
	GPlayerFieldDynamic(GEntityPlayer* pOwner, GPlayerField* pPlayerField);
	~GPlayerFieldDynamic() {}

	void SetListener(GPlayerFieldListener* pLisener)			{ m_pLisener = pLisener; }

	// 동적필드 진입지점 설정
	void SetEnterPos(const MARKER_INFO& EnterPos, CHANNELID nEnterChannelID=INVALID_CHANNELID);
	void ClearEnterPos();
	// 동적필드 부활지점 설정
	void SetCheckpoint(const MARKER_INFO& markerCheckpoint)		{ m_Checkpoint = markerCheckpoint; }
	void ClearCheckpoint();

	// 동적필드 진입지점 반환
	const MARKER_INFO& GetEnterPos() const						{ return m_EnterPos; }
	int GetEnterPosChannelID()									{ return m_EnterChannelID; }
	// 동적필드 부활지점 반환
	const MARKER_INFO& GetCheckpoint()							{ return m_Checkpoint; }

	// 동적필드 진입지점으로 이동
	bool GateToEnterPos(int nDynamicFieldID);
	// 동적필드 부활지점으로 이동
	bool GateToCheckPoint();

	// 동적 필드 이동 요청
	bool GateRequest(const GATE_INFO_DYNAMIC* pGateInfo);
	virtual bool GateRequest(DYNAMIC_FIELD_TYPE eType, int nFieldGroupID, int nFieldID, vec3 vecDestPos, vec3 vecDestDir, bool bSingleEnter=false);

	/// 실제 이동 처리
	bool GateExecute(MUID uidFieldGroup, int nFieldID, vec3 vecDestPos, vec3 vecDestDir);
};
