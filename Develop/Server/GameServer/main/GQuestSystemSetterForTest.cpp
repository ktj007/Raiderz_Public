#include "StdAfx.h"
#include "GQuestSystemSetterForTest.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"

#define _SET(original, mock)	SAFE_DELETE(original); original = mock;

GQuestSystemSetterForTest::GQuestSystemSetterForTest( GQuestSystem* pOwner )
: m_pOwner(pOwner)
{

}

void GQuestSystemSetterForTest::SetQObjUpdater( GQObjUpdater* pQObjUpdater )
{
	_SET(m_pOwner->m_pQObjUpdater, pQObjUpdater);
}