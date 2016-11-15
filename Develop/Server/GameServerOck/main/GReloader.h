#pragma once

#include "GPlayerTalent.h"

class GReloader
{
public:
	bool Reload_All();
	bool Reload_Field( int nFieldID );
	bool Reload_NPC(int nNPCID);

private:
	void RouteMessage(const wchar_t* pFormat, ...);
	
	bool DestoryField();
	bool BuildField();

private:
	set<int>				m_setRefreshFieldIDs;
};
