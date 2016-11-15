#include "StdAfx.h"
#include "XFactionInfo.h"
#include "CSFactionCalculator.h"
#include "XStrings.h"

XFactionInfo::XFactionInfo(uint8 nID, uint16 nDefaultQuantity, wstring strTitle, wstring strDesc)
: m_nID(nID)
, m_nDefaultFRT(CSFactionCalculator::CalculRelation(nDefaultQuantity))
, m_strTitle(strTitle)
, m_strDesc(strDesc)
{

}

wstring XFactionInfo::GetTitle()
{
	return XGetStr(m_strTitle);
}

wstring XFactionInfo::GetDesc()
{
	return XGetStr(m_strDesc);
}
