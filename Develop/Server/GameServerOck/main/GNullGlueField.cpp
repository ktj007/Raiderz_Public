#include "StdAfx.h"
#include "GNullGlueField.h"

GNullGlueField* GNullGlueField::m_pNullObj = NULL;

GNullGlueField::GNullGlueField()
: GGlueField(NULL)
{
	// do nothing
}

GNullGlueField::~GNullGlueField(void)
{
	// do nothing
}

GNullGlueField* GNullGlueField::GetNull()
{
	return m_pNullObj;
}

void GNullGlueField::Init()
{
	m_pNullObj = new GNullGlueField;
}

void GNullGlueField::FIni()
{
	SAFE_DELETE(m_pNullObj);
}
