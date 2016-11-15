#include "stdafx.h"
#include "VBuffDataValidator.h"


VBuffDataValidator::VBuffDataValidator()
:VDataValidator()
, m_nBuffCount(0)
{

}

VBuffDataValidator::~VBuffDataValidator()
{

}

void VBuffDataValidator::Init( VValidatorInfoMgr* vInfoMgr )
{
	//m_nBuffCount = vInfoMgr->m_pBuffInfoMgr->size();
}

void VBuffDataValidator::CheckStart( VValidatorInfoMgr* vInfoMgr )
{

}