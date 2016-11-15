#include "StdAfx.h"
#include "GNullGlueQuestPVP.h"

GNullGlueQuestPVP* GNullGlueQuestPVP::m_pNullObj = NULL;

GNullGlueQuestPVP::GNullGlueQuestPVP(void)
: GGlueQuestPVP(NULL)
{
}

GNullGlueQuestPVP::~GNullGlueQuestPVP(void)
{
}

GNullGlueQuestPVP* GNullGlueQuestPVP::GetNull()
{
	return m_pNullObj;
}

void GNullGlueQuestPVP::Init()
{
	m_pNullObj = new GNullGlueQuestPVP;
}

void GNullGlueQuestPVP::FIni()
{
	SAFE_DELETE(m_pNullObj);
}

bool GNullGlueQuestPVP::IsNull() const
{
	return true;
}
