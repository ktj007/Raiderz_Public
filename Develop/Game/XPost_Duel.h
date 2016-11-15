#pragma once

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"
#include "MCommandParameter.h"

// 결투 요청
inline void XPostDuel_Request(const MUID& uidTarget)
{
	XPOSTCMD1(MC_DUEL_REQUEST, MCommandParameterUID(uidTarget));
}

// 결투 요청 응답
inline void XPostDuel_QuestionRespond(const bool& bAnswer)
{
	XPOSTCMD1(MC_DUEL_QUESTION_RESPOND, MCommandParameterBool(bAnswer));
}

// 파티 결투 요청
inline void XPostPartyDuel_Request(const MUID& uidTarget)
{
	XPOSTCMD1(MC_DUEL_PARTY_REQUEST, MCommandParameterUID(uidTarget));
}