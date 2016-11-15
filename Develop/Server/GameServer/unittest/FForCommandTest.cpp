#include "stdafx.h"
#include "FForCommandTest.h"
#include "GDynamicFieldMaster.h"
#include "GTestForward.h"
#include "GUTHelper_Field.h"


FForCommandTest::FForCommandTest()
{
	InitField();
	InitPlayerAndLink();

	m_pNetAgent->ResetSendCommands();
}

FForCommandTest::~FForCommandTest()
{
	m_pField->Destroy();
}

void FForCommandTest::InitField()
{
	// -- field
	m_nSharedFieldID = 1;
	m_nSharedFieldID2 = 2;

	m_nDynamicFieldID = 100;

	GFieldInfo* pSharedFieldInfo = AddFieldInfo(m_nSharedFieldID);
	pSharedFieldInfo->m_bDynamic = false;

	GFieldInfo* pSharedFieldInfo2 = AddFieldInfo(m_nSharedFieldID2);
	pSharedFieldInfo2->m_bDynamic = false;

	GFieldInfo* pDynamicFieldInfo = AddFieldInfo(m_nDynamicFieldID);
	pDynamicFieldInfo->m_bDynamic = true;

	FIELD_GROUP_INFO* pFieldGroupInfo = GUTHelper_Field::NewFieldGroupInfo(pDynamicFieldInfo);
	GDynamicFieldMaster* pDynamicFieldMaster = GUTHelper_Field::NewTrialFieldMaster(pFieldGroupInfo);

	m_uidDynamicFieldGroup = pDynamicFieldMaster->GetUID();

	GUTHelper_Field::NewSharedField(pSharedFieldInfo, 1);

	m_pField = GUTHelper_Field::DefaultMockField();
}

void FForCommandTest::InitPlayerAndLink()
{
	m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));

	// -- link
	MUID uidLink = m_pPlayer->GetUID();
	m_pLink = NewLink(m_pPlayer);
	m_pLink->AddIgnoreID(MC_FATIGUE_UPDATE);
	m_pLink->AddIgnoreID(MC_TIME_SYNCH);	
}
