#include "StdAfx.h"
#include "GTrainingInfoValidator.h"
#include "GTrainingInfoMgr.h"
#include "GTrainingInfo.h"
#include "GValidateLogger.h"

bool GTrainingInfoValidator::Check()
{
	_VLI(FILENAME_TRAINING);

	MAP_TRAININGINFO& mapTrainingInfo = gmgr.pTrainingInfoMgr->GetContainer();
	for each (pair<int, GTrainingInfo*> pairTrainingInfo in mapTrainingInfo)
	{
		GTrainingInfo* pTrainingInfo = pairTrainingInfo.second;
		_ASSERT(NULL != pTrainingInfo);
		if (NULL == pTrainingInfo) continue;

		CheckTraining(pTrainingInfo);
	}

	return true;
}

void GTrainingInfoValidator::CheckTraining(GTrainingInfo* pTrainingInfo)
{
	_ASSERT(NULL != pTrainingInfo);
	if (NULL == pTrainingInfo) return;

	_VLE(TRAINING_XML_TAG_TRAINING);
	_VLA(TRAINING_XML_ATTR_ID, pTrainingInfo->m_nID);
	_VLP;

	_VLOGGER->ValidateMinusORZero(pTrainingInfo->m_nID, TRAINING_XML_ATTR_ID);
	if (NULL == pTrainingInfo->m_pDialogInfo)
	{
		_VLOGGER->Log(TRAINING_XML_ATTR_DIALOG IS_NOT_IN_DIALOG);
	}
}