#include "stdafx.h"
#include "GJobT_Warp.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GFieldSystem.h"

GJobT_Warp::GJobT_Warp(GModuleAI* pmAI, vec3 vecPos, vec3 vecDir, bool bRouteToMe) 
: GInstantJob(pmAI)
, m_vecPos(vecPos)
, m_vecDir(vecDir)
, m_bRouteToMe(bRouteToMe)
{
	// do nothing
}

GJobT_Warp::~GJobT_Warp()
{
	// do nothing
}

bool GJobT_Warp::OnStart()
{
	gsys.pFieldSystem->Warp(GetNPC(), m_vecPos, m_vecDir, m_bRouteToMe);

	return true;
}

string GJobT_Warp::GetParamString() const
{
	char buff[512];
	sprintf(buff, "TarPos: %.1f %.1f %.1f", 
		m_vecPos.x, m_vecPos.y, m_vecPos.z);
	return buff;
}
