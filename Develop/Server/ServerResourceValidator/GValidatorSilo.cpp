#include "stdafx.h"
#include "GValidatorSilo.h"
#include "GServerResourceValidators.h"
#include "GColtValidator.h"
#include "GTalentInfoValidator.h"
#include "GBuffInfoValidator.h"
#include "GNPCInfoValidator.h"
#include "GConditionsInfoValidator.h"
#include "GLootInfoValidator.h"
#include "GQuestInfoValidator.h"
#include "GDialogInfoValidator.h"
#include "GNPCShopInfoValidator.h"
#include "GSoulBindingInfoValidator.h"
#include "GItemInfoValidator.h"
#include "GFieldInfoValidator.h"
#include "GTrialFieldValidator.h"
#include "GRecipeInfoValidator.h"
#include "GCraftInfoValidator.h"
#include "GChallengerQuestInfoValidator.h"
#include "GQPEventInfoValidator.h"

GValidatorSilo::GValidatorSilo()
{
	// ¹ë¸®µ¥ÀÌÅÍ
	m_vecValidator.push_back(new GColtValidator);
	m_vecValidator.push_back(new GTalentInfoValidator);
	m_vecValidator.push_back(new GBuffInfoValidator);
	m_vecValidator.push_back(new GNPCInfoValidator);
	m_vecValidator.push_back(new GConditionsInfoValidator);
	m_vecValidator.push_back(new GLootInfoValidator);
	m_vecValidator.push_back(new GQuestInfoValidator);
	m_vecValidator.push_back(new GDialogInfoValidator);
	m_vecValidator.push_back(new GNPCShopInfoValidator);
	m_vecValidator.push_back(new GSoulBindingInfoValidator);
	m_vecValidator.push_back(new GItemInfoValidator);
	m_vecValidator.push_back(new GFieldInfoValidator);
	m_vecValidator.push_back(new GTrialFieldValidator);
	m_vecValidator.push_back(new GCraftInfoValidator);
	m_vecValidator.push_back(new GRecipeInfoValidator);
	m_vecValidator.push_back(new GChallengerQuestInfoValidator);
	m_vecValidator.push_back(new GQPEventInfoValidator);

	// ¹ë¸®µ¥ÀÌÅÍ ÄÄÆ÷Áþ
	m_pAllCheckValidator = new GServerResourceValidators;

	for each(CSBaseValidator* pValidator in m_vecValidator)
	{
		m_pAllCheckValidator->AddValidator(pValidator);
	}
}

GValidatorSilo::~GValidatorSilo()
{
	delete m_pAllCheckValidator;

	while(!m_vecValidator.empty())
	{
		delete m_vecValidator.back();
		m_vecValidator.pop_back();
	}
}

CSBaseValidator* GValidatorSilo::GetAllCheckValidator(void)
{
	return m_pAllCheckValidator;
}

bool GValidateResouces()
{
	// Server resource validator
	GValidatorSilo silo;
	CSBaseValidator* pValidator = silo.GetAllCheckValidator();

	bool ret = pValidator->Check();
	if (ret == false) 
	{
		mlog("Validate Resource : Failed\n");
	}
	else
	{
		mlog("Validate Resource : Success!\n");
	}
	return ret;
}