#pragma once

inline void XPostCutscene_BeginReq(int nCutsceneID)
{
	XPOSTCMD1(MC_CUTSCENE_BEGIN_REQ, MCommandParameterInt(nCutsceneID));
}


inline void XPostCutscene_EndReq(int nCutsceneID)
{
	XPOSTCMD1(MC_CUTSCENE_END_REQ, MCommandParameterInt(nCutsceneID));
}
