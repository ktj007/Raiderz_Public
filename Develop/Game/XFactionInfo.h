#pragma once

#include "CSFactionInfo.h"

enum FACTION_RELATION_TYPE;

class XFactionInfo
{
public:
	const uint8				m_nID;
	FACTION_RELATION_TYPE	m_nDefaultFRT;

public:
	XFactionInfo(uint8 nID, uint16 nDefaultQuantity, wstring strTitle, wstring strDesc);

	wstring GetTitle();
	wstring GetDesc();

private:
	wstring			m_strTitle;
	wstring			m_strDesc;
};
