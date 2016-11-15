#include "StdAfx.h"
#include "GLootInfo.h"
#include "GSTLUtil.h"

GLootInfo::GLootInfo(void)
: m_nID(0)
, m_nLimitDropQuantity(INT_MAX)
, m_bValidation(true)
, m_bBPartLoot(false)
{
}

GLootInfo::~GLootInfo(void)
{
	Clear();
}

void GLootInfo::Clear()
{
	SAFE_DELETE_VECTOR(m_vecLootItems);
	SAFE_DELETE_VECTOR(m_vecLootMoneys);
	SAFE_DELETE_VECTOR(m_vecLootSouls);
	m_vecLootBuffs.clear();
}