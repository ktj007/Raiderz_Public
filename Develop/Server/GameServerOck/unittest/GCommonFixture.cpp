#include "stdafx.h"
#include "GCommonFixture.h"
#include "GNPCInfo.h"
#include "GUTHelper_Field.h"

CommonFixture_FN::CommonFixture_FN()
{
	m_pNPCInfo = new GNPCInfo;
	m_pNPCInfo->nID = 1;
	gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

	m_pField = GUTHelper_Field::DefaultMockField();
}

CommonFixture_FN::~CommonFixture_FN()
{
	m_pField->Update(1.0f);
	m_pField->Destroy();
}