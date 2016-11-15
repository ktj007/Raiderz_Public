#pragma once

#include "XTargetActorInfo.h"

/// 인터랙션과 관련된 정보는 모두 이곳에 둡니다. - birdkr
class XInteractionInfo
{
private:
	MUID		m_InteractionSearchUID;		// Search한 ObjectUID 
public:
	XInteractionInfo();
	~XInteractionInfo();


	XTargetActorInfo	stTargetActorInfo;			///< 타겟
	MUID				InteractionTargetUID;		///< Interaction 중인 ObjectUID

	MUID				RequestPlayerUID;			///< 나에게 요청하는 상대방의 UID

	MUID				VSPlayerUID;				///< 전투중인 상대방의 UID

	bool		m_bNPCInteractionState;		// NPC와 인터렉션 상태

	void		Init_ForChangeField();

	UIID		GetSearchUIID()				{ return stTargetActorInfo.nSearchUIID; }
	void		SetSearchUIID(UIID uiid)	{ stTargetActorInfo.nSearchUIID = uiid; };

	UIID		GetTargetUIID()				{ return stTargetActorInfo.nTargetUIID; }
	void		SetTargetUIID(UIID uiid)	{ stTargetActorInfo.nTargetUIID = uiid; };

	MUID		GetInteractionSearchUID()	{ return m_InteractionSearchUID; }
	void		SetInteractionSearchUID(MUID uid)	{ m_InteractionSearchUID = uid; }

	MUID		GetRequestPlayerUID()				{ return RequestPlayerUID; }
	void		SetRequestPlayerUID(MUID uid)		{ RequestPlayerUID = uid; }

};