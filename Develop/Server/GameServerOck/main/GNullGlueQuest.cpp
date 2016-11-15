#include "StdAfx.h"
#include "GNullGlueQuest.h"

GNullGlueQuest* GNullGlueQuest::m_pNullObj = NULL;

GNullGlueQuest::GNullGlueQuest(void)
: GGlueQuest(NULL)
{
	// do nothing
}

GNullGlueQuest::~GNullGlueQuest(void)
{
	// do nothing
}

GNullGlueQuest* GNullGlueQuest::GetNull()
{
	return m_pNullObj;
}

void GNullGlueQuest::Init()
{
	m_pNullObj = new GNullGlueQuest;
}

void GNullGlueQuest::FIni()
{
	SAFE_DELETE(m_pNullObj);
}
