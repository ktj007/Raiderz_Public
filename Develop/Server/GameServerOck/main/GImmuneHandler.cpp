#include "StdAfx.h"
#include "GImmuneHandler.h"
#include "MUtil.h"
#include "GDef.h"

GImmuneHandler::GImmuneHandler(void)
: m_bInit(false)
{
}

GImmuneHandler::~GImmuneHandler(void)
{
}

void GImmuneHandler::Init( immune::INFO info )
{
	m_Info = info;
	m_bInit = true;
}

bool GImmuneHandler::IsDamageTypeImmune( DAMAGE_ATTRIB nDamageType ) const
{
//	VALID_RET(m_bInit, false);
	return CheckBitSet(m_Info.nImmuneDamageType, nDamageType) != 0;
}

bool GImmuneHandler::IsSkillTypeImmune( TALENT_SKILL_TYPE nSkillType ) const
{
	//	VALID_RET(m_bInit, false);
	return CheckBitSet(m_Info.nImmuneSkillType, nSkillType) != 0;
}

bool GImmuneHandler::IsBuffImmune( BUFF_PASSIVE_EXTRA_ATTRIB_ nBuffExtraType ) const
{
//	VALID_RET(m_bInit, false);
	return CheckBitSet(m_Info.nImmuneBuff, nBuffExtraType) != 0;
}

bool GImmuneHandler::IsImmune( immune::CUSTOM_TYPE nCustomType ) const
{
//	VALID_RET(m_bInit, false);
	return CheckBitSet(m_Info.nImmuneCustomType, nCustomType) != 0;
}