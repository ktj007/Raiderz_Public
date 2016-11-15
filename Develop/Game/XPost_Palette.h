#ifndef _XPOST_PALETTE_H
#define _XPOST_PALETTE_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostPalette_Select(PALETTE_NUM nNum)
{
	XPOSTCMD1(MC_PALETTE_SELECT_REQ, MCommandParameterUChar(nNum));
}

inline void XPostPalette_SetPrimary(PALETTE_NUM nNum)
{
	XPOSTCMD1(MC_PALETTE_SET_PRIMARY_REQ, MCommandParameterUChar(nNum));
}

inline void XPostPalette_SetSecondary(PALETTE_NUM nNum)
{
	XPOSTCMD1(MC_PALETTE_SET_SECONDARY_REQ, MCommandParameterUChar(nNum));
}

inline void XPostPalette_PutUp(PALETTE_NUM nNum, PALETTE_SLOT nSlot, PALETTE_ITEM_TYPE nType, int nItemIDorTalentID)
{
	XPOSTCMD4(MC_PALETTE_PUTUP_REQ, MCommandParameterUChar(nNum), MCommandParameterUChar(nSlot), MCommandParameterUChar(nType), MCommandParameterInt(nItemIDorTalentID));
}

inline void XPostPalette_PutDown(PALETTE_NUM nNum, PALETTE_SLOT nSlot)
{
	XPOSTCMD2(MC_PALETTE_PUTDOWN_REQ, MCommandParameterUChar(nNum), MCommandParameterUChar(nSlot));
}

inline void XPostPalette_Change(PALETTE_NUM nNum1, PALETTE_SLOT nSlot1, PALETTE_NUM nNum2, PALETTE_SLOT nSlot2)
{
	XPOSTCMD4(MC_PALETTE_CHANGE_REQ, MCommandParameterUChar(nNum1), MCommandParameterUChar(nSlot1), MCommandParameterUChar(nNum2),	MCommandParameterUChar(nSlot2));
}

#endif