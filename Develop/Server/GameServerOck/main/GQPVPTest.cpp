#include "StdAfx.h"
#include "GQPVPTest.h"
#include "GConfig.h"
#include "GSharedFieldMaster.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GQuestPVP.h"
#include "GGlueQuestPVP.h"

#define AVERAGE_QPVPEVENT_TIME 600

GQPVPTest::GQPVPTest(void)
: m_pField(NULL)
{
}

GQPVPTest::~GQPVPTest(void)
{

}

void GQPVPTest::OnCreate()
{
	InitInfo();
	PickUPField();
	
	m_pField->GetQuestPVP()->GetDelegator()->DisableBeginEvent();
}


void GQPVPTest::OnUpdate( float fDelta )
{
	if (false == m_pField->GetQuestPVP()->IsDoingEvent())
	{
		m_pField->GetQuestPVP()->BeginEvent(m_Info.nQPVPEventID);
	}	
}

void GQPVPTest::InitInfo()
{
	m_Info.nFieldID = GConfig::m_nFieldID;
	m_Info.nQPVPEventID = GConfig::m_nQPVPEventID;
}

void GQPVPTest::PickUPField()
{
	GSharedFieldMaster* pSharedField = gmgr.pFieldMgr->GetSharedFieldMaster(m_Info.nFieldID);
	if (pSharedField)
	{
		for (int i = 0; i < 4; i++)
		{
			m_pField = pSharedField->GetChannel(i);
			if (m_pField != NULL) break;
		}
	}
}
