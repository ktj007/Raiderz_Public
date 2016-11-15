#include "stdafx.h"
#include "XPlayerFaction.h"
#include "XFactionInfo.h"
#include "XFactionInfoMgr.h"
#include "CSFactionCalculator.h"
#include "AStlUtil.h"
#include "MLocale.h"

XPlayerFaction::XPlayerFaction(uint8 nID, uint16 nQuantity)
: m_nID(nID), m_nQuantity(nQuantity)
{

}

FACTION_RELATION_TYPE XPlayerFaction::GetRelation()
{
	return CSFactionCalculator::CalculRelation(m_nQuantity);
}

uint16 XPlayerFaction::GetQuantity()
{
	return m_nQuantity;
}

//////////////////////////////////////////////////////////////////////////

XPlayerFactions::XPlayerFactions()
{
	Clear();
}

XPlayerFactions::~XPlayerFactions()
{
	Clear();
}

void XPlayerFactions::Clear()
{
	SAFE_DELETE_MAP(m_mapPlayerFaction);
}

bool XPlayerFactions::GetIndex( size_t nIndex, XPlayerFaction** ppFaction)
{
	MAP_PLAYERFACTION::iterator itr = m_mapPlayerFaction.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++)
	{
		if ( itr == m_mapPlayerFaction.end())	return false;
	}
	*ppFaction = (*itr).second;
	return true;
}

XPlayerFaction* XPlayerFactions::Insert(uint8 nID, uint16 nQuantity)
{
	if (true == IsExist(nID)) return NULL;

	XFactionInfo* pFactionInfo =  info.faction->Get(nID);
	if (NULL == pFactionInfo) return NULL;

	XPlayerFaction* pNewPlayerFaction = new XPlayerFaction(nID, nQuantity);

	m_mapPlayerFaction.insert(MAP_PLAYERFACTION::value_type(pFactionInfo->m_nID, pNewPlayerFaction));

	return pNewPlayerFaction;
}

XPlayerFaction* XPlayerFactions::Get( uint8 nID )
{
	MAP_PLAYERFACTION::iterator itor = m_mapPlayerFaction.find(nID);
	if (m_mapPlayerFaction.end() == itor) return NULL;

	return (*itor).second;
}

void XPlayerFactions::SetQuantity(uint8 nID, uint16 nQuantity)
{
	XPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL == pPlayerFaction)
	{
		pPlayerFaction = Insert(nID, nQuantity);
	}

	if (NULL == pPlayerFaction) return;

	pPlayerFaction->m_nQuantity = nQuantity;
}

bool XPlayerFactions::IsExist( uint8 nID )
{
	XPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL == pPlayerFaction) return false;

	return true;
}

FACTION_RELATION_TYPE XPlayerFactions::GetRelation(uint8 nID)
{
	XPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL != pPlayerFaction)	return pPlayerFaction->GetRelation();

	XFactionInfo* pFactionInfo = info.faction->Get(nID);
	if (NULL != pFactionInfo) return pFactionInfo->m_nDefaultFRT;

	return FRT_NONE;
}

uint16 XPlayerFactions::GetQuantity(uint8 nID)
{
	XPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL != pPlayerFaction)	return pPlayerFaction->GetQuantity();	

	return 0;
}

MWLua::table XPlayerFactions::GetFactionInfoTable()
{
	MWLua::table t(global.script->GetLua()->GetState());

	char buffer[32];
	int i = 1;
	for each (pair<uint8, XPlayerFaction*> pairFactionInfo in m_mapPlayerFaction)
	{
		int nID = pairFactionInfo.first;
		XPlayerFaction* pPlayerFaction = pairFactionInfo.second;

		XFactionInfo* pFactionInfo =  info.faction->Get(nID);
		if (NULL == pFactionInfo) continue;

		uint16 nValue = ((float)pPlayerFaction->GetQuantity()/CSFactionCalculator::GetMaxQuantity())*100.f;

		MWLua::table faction(global.script->GetLua()->GetState());
		faction.set("id"	, nID);
		faction.set("value"	, nValue);
		faction.set("title"	, MLocale::ConvUTF16ToAnsi(pFactionInfo->GetTitle()).c_str());
		faction.set("desc"	, MLocale::ConvUTF16ToAnsi(pFactionInfo->GetDesc()).c_str());
		faction.set("type"	, pPlayerFaction->GetRelation());

		sprintf(buffer,"%d", i);

		t.set(buffer, faction);
		i++;
	}

	return t;
}

