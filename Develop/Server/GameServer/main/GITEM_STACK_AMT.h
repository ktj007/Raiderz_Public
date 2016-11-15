#ifndef _GDBITEM_STACK_AMT_H
#define _GDBITEM_STACK_AMT_H

class GItem;


struct GITEM_STACK_AMT
{
	GITEM_STACK_AMT(GItem* pItem);
	GITEM_STACK_AMT();
	
	int64	nIUID;
	int		nItemID;
	int16	nToStackAmt;
	uint8	nSlotType;
	uint16	nSlotID;
	int16	nModStackAmt;
};

typedef vector<GITEM_STACK_AMT> GITEM_STACK_AMT_VEC;


#endif