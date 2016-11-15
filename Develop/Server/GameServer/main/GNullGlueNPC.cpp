#include "StdAfx.h"
#include "GNullGlueNPC.h"
#include "GGlueNPC.h"

GNullGlueNPC* GNullGlueNPC::m_pNullObj = NULL;

GNullGlueNPC::GNullGlueNPC(void)
: GGlueNPC(NULL)
{
	 
}

GNullGlueNPC::~GNullGlueNPC(void)
{
	
}

GNullGlueNPC* GNullGlueNPC::GetNull()
{
	return m_pNullObj;
}

void GNullGlueNPC::Init()
{
	m_pNullObj = new GNullGlueNPC;
}

void GNullGlueNPC::FIni()
{
	SAFE_DELETE(m_pNullObj);
}