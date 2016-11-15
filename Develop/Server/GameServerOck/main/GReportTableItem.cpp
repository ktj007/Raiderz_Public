#include "stdafx.h"
#include "GReportTableItem.h"
#include <iomanip>

void GReportTableItem::SetData(wstring strData)
{
	m_strData = strData;
}

void GReportTableItem::SetSubData(int nSubCol, wstring strData)
{
	_ASSERT(0 <= nSubCol);

	if ((int)m_vecSubData.size() <= nSubCol)
	{
		m_vecSubData.resize(nSubCol + 1);
	}
	
	m_vecSubData[nSubCol] = strData;
}

wstring GReportTableItem::GetData(void) const
{
	return m_strData;
}

wstring GReportTableItem::GetSubData(int nSubCol) const
{
	_ASSERT(0 <= nSubCol);

	if ((int)m_vecSubData.size() <= nSubCol)
	{
		return wstring();
	}

	return m_vecSubData[nSubCol];
}

int GReportTableItem::GetSubDataCount(void) const
{
	return m_vecSubData.size();
}
