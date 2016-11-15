#include "StdAfx.h"
#include "GSoulBindingInfoValidator.h"
#include "GSoulBindingInfoMgr.h"
#include "GSoulBindingInfo.h"
#include "GValidateLogger.h"
#include "GServerSystems.h"
#include "GDef.h"

bool GSoulBindingInfoValidator::Check()
{
	if (gmgr.pSoulBindingInfoMgr == NULL)	return true;

	_VLI(FILENAME_SOULBINDING);

	MAP_SOULBINDINGINFO mapSoulBindingInfo = gmgr.pSoulBindingInfoMgr->GetContainer();
	for each (pair<int, GSoulBindingInfo*> pairSoulBindingInfo in mapSoulBindingInfo)
	{
		GSoulBindingInfo* pSoulBindingInfo = pairSoulBindingInfo.second;
		_ASSERT(NULL != pSoulBindingInfo);
		if (NULL == pSoulBindingInfo) continue;

		CheckSoulBinding(pSoulBindingInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GSoulBindingInfoValidator::CheckSoulBinding(GSoulBindingInfo* pSoulBindingInfo)
{
	_ASSERT(NULL !=  pSoulBindingInfo);
	if (NULL == pSoulBindingInfo) return;

	_VLE(_T(SOULBINDING_XML_TAG_SOULBINDING));
	_VLA(_T(SOULBINDING_XML_ATTR_ID), pSoulBindingInfo->m_nID);
	_VLP;

	_VLOGGER->ValidateID(pSoulBindingInfo->m_nID);
	_VLOGGER->ValidateDialogID(pSoulBindingInfo->m_nDialogID, _T(SOULBINDING_XML_ATTR_DIALOG));
}