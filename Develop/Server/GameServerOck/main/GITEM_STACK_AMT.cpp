#include "stdafx.h"
#include "GITEM_STACK_AMT.h"
#include "GItem.h"

GITEM_STACK_AMT::GITEM_STACK_AMT(GItem* pItem)
{
	VALID(pItem);

	nIUID			= pItem->m_nIUID;
	nToStackAmt		= 0;
	nModStackAmt	= pItem->GetAmount();
	nSlotType		= pItem->m_nSlotType;
	nSlotID			= pItem->m_nSlotID;
	nItemID			= pItem->m_pItemData->m_nID;
}

GITEM_STACK_AMT::GITEM_STACK_AMT()
: nIUID(0)
, nItemID(0)
, nToStackAmt(0)
, nSlotType(0)
, nSlotID(0)
, nModStackAmt(0)
{

}