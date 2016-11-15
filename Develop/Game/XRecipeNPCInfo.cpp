#include "StdAfx.h"
#include "XRecipeNPCInfo.h"

XRecipeNPCInfo::XRecipeNPCInfo(void)
: m_nRecipeID(0)
{
	m_vecFieldNPC.reserve(MAX_RECIPENPC);
}

XRecipeNPCInfo::~XRecipeNPCInfo(void)
{
}
