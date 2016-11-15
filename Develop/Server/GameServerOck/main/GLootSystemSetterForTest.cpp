#include "StdAfx.h"
#include "GLootSystemSetterForTest.h"
#include "GLootSystem.h"
#include "GLootBeginner.h"
#include "GLootShower.h"
#include "GLootLooter.h"
#include "GLootMasterLooter.h"
#include "GLootEnder.h"
#include "GLootModifyHandler.h"
#include "GLootItemGetter.h"

#define _SET(original, mock)	SAFE_DELETE(original); original = mock;

GLootSystemSetterForTest::GLootSystemSetterForTest(GLootSystem* pOwner)
: m_pOwner(pOwner)
{
}

void GLootSystemSetterForTest::SetStarter(GLootBeginner* pStarter)
{
	_SET(m_pOwner->m_pBeginner, pStarter);
}

void GLootSystemSetterForTest::SetShower(GLootShower* pShower)
{
	_SET(m_pOwner->m_pShower, pShower);
}

void GLootSystemSetterForTest::SetLooter(GLootLooter* pLooter)
{
	_SET(m_pOwner->m_pLooter, pLooter);
}

void GLootSystemSetterForTest::SetMasterLooter(GLootMasterLooter* pMasterLooter)
{
	_SET(m_pOwner->m_pMasterLooter, pMasterLooter);
}

void GLootSystemSetterForTest::SetEnder(GLootEnder* pEnder)
{
	_SET(m_pOwner->m_pEnder, pEnder);
}

void GLootSystemSetterForTest::SetModifyHandler(GLootModifyHandler* pModifyHandler)
{
	_SET(m_pOwner->m_pModifyHandler, pModifyHandler);
}

void GLootSystemSetterForTest::SetItemGetter(GLootItemGetter* pItemGetter)
{
	_SET(m_pOwner->m_pItemGetter, pItemGetter);
}