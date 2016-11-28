#include "stdafx.h"
#include "GJobT_BreakPart.h"
#include "GEntityNPC.h"
#include "GNPCBParts.h"

GJobT_BreakPart::GJobT_BreakPart(GModuleAI* pmAI, uint8 nPartID, MUID uidOpponent)
: GInstantJob(pmAI)
, m_nPartID(nPartID)
, m_uidOpponent(uidOpponent)
{
	// do nothing
}

GJobT_BreakPart::~GJobT_BreakPart()
{
	// do nothing
}

bool GJobT_BreakPart::OnStart()
{	
	if (GetNPC()->GetNPCBParts())
		GetNPC()->GetNPCBParts()->Break(m_nPartID, m_uidOpponent);
	return true;
}

string GJobT_BreakPart::GetParamString() const
{
	GEntityNPC* pTarget = GetOwnerNPC()->FindNPC(m_uidOpponent);
	if (NULL == pTarget)	
		return "Opponent: invalid_target";

	char buff[512];
	sprintf(buff, "Opponent: %s(%I64d), PartID: %d", 
		MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str(), pTarget->GetUID().Value, m_nPartID);
	return buff;
}
