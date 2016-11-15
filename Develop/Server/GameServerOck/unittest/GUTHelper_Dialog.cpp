#include "StdAfx.h"
#include "GUTHelper_Dialog.h"
#include "SUnitTestUtil.h"
#include "GDialogInfo.h"
#include "GDialogInfoMgr.h"

GDialogInfo* GUTHelper_Dialog::NewDialogInfo(int nID)
{
	if (0 == nID) nID = SUnitTestUtil::NewID();

	GDialogInfo* pDialogInfo = new GDialogInfo();
	pDialogInfo->m_nID = nID;
	pDialogInfo->m_nExit = 1;

	gmgr.pDialogInfoMgr->Insert(pDialogInfo);

	return pDialogInfo;
}

GDialogSayInfo* GUTHelper_Dialog::NewDialogSayInfo()
{
	GDialogSayInfo* pNewDialogSayInfo = new GDialogSayInfo();
	pNewDialogSayInfo->m_vecDialogSelectInfo.push_back(NewDialogSelectInfo());

	return pNewDialogSayInfo;
}

GDialogSelectInfo* GUTHelper_Dialog::NewDialogSelectInfo(int nExit)
{
	GDialogSelectInfo* pNewDialogSelectInfo = new GDialogSelectInfo();
	pNewDialogSelectInfo->m_nExit = nExit;

	return pNewDialogSelectInfo;
}
