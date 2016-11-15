#include "StdAfx.h"
#include "XItem_DroperInfoMgr.h"
#include "XItemRelatedNPCInfo.h"
#include "XNPCInfo.h"

XItem_DroperInfoMgr::XItem_DroperInfoMgr(void)
{
}

XItem_DroperInfoMgr::~XItem_DroperInfoMgr(void)
{
}

size_t XItem_DroperInfoMgr::GetDropperCount( int nItemID)
{
	XItemRelatedNPCInfo* pRelatedNPC = Get( nItemID);
	if ( pRelatedNPC == NULL)		return 0;
	return pRelatedNPC->m_setNPCID.size();
}

const wchar_t* XItem_DroperInfoMgr::GetDropperName( int nItemID, int nIndex)
{
	XItemRelatedNPCInfo* pRelatedNPC = Get( nItemID);
	if ( pRelatedNPC == NULL)		return L"";

	set< int>::const_iterator itr = pRelatedNPC->m_setNPCID.begin();
	for ( int i = 0;  i != nIndex;  i++, itr++) ;
	
	XNPCInfo* pNPCInfo = info.npc->Get( (*itr));
	if ( pNPCInfo == NULL)			return L"";
	return pNPCInfo->GetName();
}
