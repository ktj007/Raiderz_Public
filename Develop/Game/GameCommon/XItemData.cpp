#include "stdafx.h"
#include "XItemData.h"
#include "XStrings.h"

XItemData::XItemData()
{
	// do nothing
}

XItemData::~XItemData()
{
	// do nothing
}

const TCHAR* XItemData::GetName(void)
{
	return XStringResMgr::GetInstance()->GetString(m_strName);
}

const TCHAR* XItemData::GetDesc(void)
{
	return XStringResMgr::GetInstance()->GetString(m_strDesc);
}
