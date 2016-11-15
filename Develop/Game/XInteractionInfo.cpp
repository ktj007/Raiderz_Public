#include "stdafx.h"
#include "XInteractionInfo.h"


XInteractionInfo::XInteractionInfo() : InteractionTargetUID(0, 0), 	m_InteractionSearchUID(0, 0), RequestPlayerUID(0, 0), VSPlayerUID(0,0), m_bNPCInteractionState(false)
{
	stTargetActorInfo.Init();
}

XInteractionInfo::~XInteractionInfo()
{
	stTargetActorInfo.Init();
}

void XInteractionInfo::Init_ForChangeField()
{
	InteractionTargetUID	= MUID::ZERO;
	m_InteractionSearchUID	= MUID::ZERO;
	RequestPlayerUID		= MUID::ZERO;
	VSPlayerUID				= MUID::ZERO;

	stTargetActorInfo.Init();
}

