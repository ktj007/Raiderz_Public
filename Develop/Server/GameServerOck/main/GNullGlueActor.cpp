#include "StdAfx.h"
#include "GNullGlueActor.h"

GNullGlueActor* GNullGlueActor::m_pNullObj = NULL;

GNullGlueActor::GNullGlueActor(void)
: GGlueActor(NULL)
{
	 
}

GNullGlueActor::~GNullGlueActor(void)
{
	
}

GNullGlueActor* GNullGlueActor::GetNull()
{
	return m_pNullObj;
}

void GNullGlueActor::Init()
{
	m_pNullObj = new GNullGlueActor;
}

void GNullGlueActor::FIni()
{
	SAFE_DELETE(m_pNullObj);
}
