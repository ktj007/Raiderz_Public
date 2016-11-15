#include "StdAfx.h"
#include "XFieldInfoHelper.h"
#include "XFieldInfo.h"
#include "XGameState.h"

void XFieldInfoHelper::CheckPointFieldIDReset( int nFieldID )
{
	CSFieldFileInfo* pFieldInfo = info.field->GetFieldList().Find( nFieldID);
	if ( pFieldInfo && pFieldInfo->bDynamic == false)
	{
		gvar.MyInfo.nCheckPointFieldID = 0;
	}
}

string XFieldInfoHelper::GetNPCIconStateString(XFieldInfo* pFieldInfo, int type, int nNPCID)
{
	if ( pFieldInfo == NULL) return "";

	NPC_UIMAP_INFO_VECTOR* pVecNpcUiInfo = &pFieldInfo->m_NpcUiInfo[type];
	if (NULL == pVecNpcUiInfo) return "";

	for (NPC_UIMAP_INFO_VECTOR:: iterator itr = pVecNpcUiInfo->begin(); itr != pVecNpcUiInfo->end(); ++itr)
	{
		NPC_UIMAP_INFO& npcUIMapInfo = (*itr);
		if(npcUIMapInfo.nNPCID == nNPCID)
			return npcUIMapInfo.strLookState;
	}		

	return "";
}