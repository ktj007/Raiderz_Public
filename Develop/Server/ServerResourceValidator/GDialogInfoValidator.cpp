#include "StdAfx.h"
#include "GDialogInfoValidator.h"
#include "GServerSystems.h"
#include "GDef.h"
#include "GDialogInfoMgr.h"
#include "GDialogInfo.h"
#include "GValidateLogger.h"
#include "GItemAddRemoveInfo.h"
#include "GItemAddRemoveInfoValidator.h"
#include "CSDef_Dialog.h"

bool GDialogInfoValidator::Check()
{
	if (gmgr.pDialogInfoMgr == NULL)	return true;

	_VLI(FILENAME_DIALOG);

	MAP_DIALOGINFO& mapDialogInfo = gmgr.pDialogInfoMgr->GetContainer();
	for each (pair<int, GDialogInfo*> pairDialogtInfo in mapDialogInfo)
	{
		GDialogInfo* pDialogInfo = pairDialogtInfo.second;
		_ASSERT(NULL != pDialogInfo);
		if (NULL == pDialogInfo) continue;

		CheckDialog(pDialogInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GDialogInfoValidator::CheckDialog(GDialogInfo* pDialogInfo)
{
	_ASSERT(NULL != pDialogInfo);
	if (NULL == pDialogInfo) return;

	_VLE(_T(DIALOG_XML_TAG_DIALOG));
	_VLA(_T(DIALOG_XML_ATTR_ID), pDialogInfo->m_nID);
	_VLP;
	
	_VLOGGER->ValidateID(pDialogInfo->m_nID);
	if (NULL == pDialogInfo->m_pDialogSayInfo)
	{
		_VLOGGER->ValidateMinusORZero(pDialogInfo->m_nExit, _T(DIALOG_XML_ATTR_EXIT));
	}
	else
	{
		CheckSay(pDialogInfo->m_pDialogSayInfo);		
	}	
}

void GDialogInfoValidator::CheckSay(GDialogSayInfo* pDialogSayInfo)
{
	_ASSERT(NULL != pDialogSayInfo);
	if (NULL == pDialogSayInfo) return;

	_VLE(_T(DIALOG_XML_TAG_SAY));
	_VLP;

	for each (GDialogSelectInfo* pDialogSelectInfo in pDialogSayInfo->m_vecDialogSelectInfo)
	{
		_ASSERT(NULL != pDialogSelectInfo);
		if (NULL == pDialogSelectInfo) continue;

		CheckSelect(pDialogSelectInfo);
	}	
}

void GDialogInfoValidator::CheckSelect(GDialogSelectInfo* pDialogSelectInfo)
{
	_ASSERT(NULL != pDialogSelectInfo);
	if (NULL == pDialogSelectInfo) return;

	_VLE(_T(DIALOG_XML_TAG_SELECT));
	_VLP;	

	if (0 == pDialogSelectInfo->m_nExit)
	{
		if (NULL !=  pDialogSelectInfo->m_pItemAddRemoveInfo)
		{
			_VLOGGER->Log(_T(DIALOG_XML_ATTR_EXIT) L"을 가지고 있지 않은" _T(DIALOG_XML_TAG_SELECT) L" 안에 " _T(DIALOG_XML_TAG_ITEM) L"을 기술 할수 없습니다.\n");
		}
	}
	else
	{
		if (NULL !=  pDialogSelectInfo->m_pDialogSayInfo)
		{
			_VLOGGER->Log(_T(DIALOG_XML_ATTR_EXIT) L"을 가지고 있는" _T(DIALOG_XML_TAG_SELECT) L" 안에는 아무것도 올 수 없습니다.\n");
		}
	}

	if (NULL != pDialogSelectInfo->m_pDialogSayInfo)
	{
		CheckSay(pDialogSelectInfo->m_pDialogSayInfo);
	}

	if (NULL != pDialogSelectInfo->m_pItemAddRemoveInfo)
	{
		CheckItem(pDialogSelectInfo->m_pItemAddRemoveInfo);
	}
}

void GDialogInfoValidator::CheckItem(GItemAddRemoveInfo* pItemInfo)
{
	VALID(pItemInfo);

	_VLE(_T(DIALOG_XML_TAG_ITEM));
	_VLP;	

	GItemAddRemoveInfoValidator itemValidator;
	itemValidator.Check(pItemInfo);	
}