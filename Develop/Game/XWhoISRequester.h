#pragma once

#include <set>
#include "CSDef.h"
#include "XPost_Comm.h"

class XWhoIsRequester
{
protected:
	std::set<UIID> m_setRequestedWhoisUID;
	bool IsExist(const UIID nUIID)
	{
		return (m_setRequestedWhoisUID.find(nUIID) != m_setRequestedWhoisUID.end());
	}
public:
	XWhoIsRequester()
	{

	}
	~XWhoIsRequester()
	{

	}

	void Erase(const UIID nUIID)
	{
		if (IsExist(nUIID))
		{
			m_setRequestedWhoisUID.erase(nUIID);
		}
	}

	void Request(const UIID nUIID)
	{
		if (IsExist(nUIID)) return;

		m_setRequestedWhoisUID.insert(nUIID);
		XPostRequestWhoIsUIID(nUIID);
	}

	size_t GetSize() { return m_setRequestedWhoisUID.size(); }
};