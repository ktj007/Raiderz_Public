#include "StdAfx.h"
#include "GDialogInfo.h"
#include "GSTLUtil.h"
#include "GItemAddRemoveInfo.h"

GDialogInfo::GDialogInfo()
: m_nID(0)
, m_nExit(0)
, m_nTextID(0)
, m_pDialogSayInfo(NULL)
{
}

GDialogInfo::~GDialogInfo()
{
	SAFE_DELETE(m_pDialogSayInfo);
}

GDialogSayInfo::GDialogSayInfo()
: m_nTextID(0)
{

}

GDialogSayInfo::~GDialogSayInfo()
{
	for_each(m_vecDialogSelectInfo.begin(), m_vecDialogSelectInfo.end(), DO());
}

GDialogSelectInfo::GDialogSelectInfo()
: m_nExit(0)
, m_nTextID(0)
, m_pDialogSayInfo(NULL)
, m_pItemAddRemoveInfo(NULL)
{

}

GDialogSelectInfo::~GDialogSelectInfo()
{
	SAFE_DELETE(m_pDialogSayInfo);
	SAFE_DELETE(m_pItemAddRemoveInfo);
}