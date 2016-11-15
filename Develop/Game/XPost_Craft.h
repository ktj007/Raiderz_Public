#ifndef _XPOST_CRAFT_H
#define _XPOST_CRAFT_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"

inline void XPostCraft_MakeReq(int nRecipeID)
{
	XPOSTCMD1(MC_CRAFT_MAKE_REQ, MCommandParameterInt(nRecipeID));
}

inline void XPostCraft_RecipeInsert(int nRecipeID)
{
	XPOSTCMD1(MC_CRAFT_RECIPE_INSERT_REQ, MCommandParameterInt(nRecipeID));
}

inline void XPostCraft_RecipeDelete(int nRecipeID)
{
	XPOSTCMD1(MC_CRAFT_RECIPE_DELETE_REQ, MCommandParameterInt(nRecipeID));
}

#endif//_XPOST_CRAFT_H
