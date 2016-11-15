#include "StdAfx.h"
#include "GItemSystemSetterForTest.h"
#include "GItemSystem.h"
#include "GItemAdder.h"
#include "GItemRemover.h"

#define _SET(original, mock)	SAFE_DELETE(original); original = mock;

GItemSystemSetterForTest::GItemSystemSetterForTest(GItemSystem* pOwner)
: m_pOwner(pOwner)
{
}

void GItemSystemSetterForTest::SetAdder( GItemAdder* pAdder )
{
	_SET(m_pOwner->m_pAdder, pAdder);
}

void GItemSystemSetterForTest::SetRemover( GItemRemover* pRemover )
{
	_SET(m_pOwner->m_pRemover, pRemover);
}