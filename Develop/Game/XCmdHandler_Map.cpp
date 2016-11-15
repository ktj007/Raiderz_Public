#include "stdafx.h"
#include "XCmdHandler_Map.h"

XCmdHandler_Map::XCmdHandler_Map(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_MINIMAP_MARK_POS,	OnMiniMapMarkPos);	
}

MCommandResult XCmdHandler_Map::OnMiniMapMarkPos(MCommand* pCommand, MCommandHandler* pHandler)
{
	vec3 vPos;
	if (pCommand->GetParameter(&vPos,		0, MPT_VEC)==false) return CR_ERROR;

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnMiniMapMarkPosition(vPos.x, vPos.y);
	}

	return CR_TRUE;
}