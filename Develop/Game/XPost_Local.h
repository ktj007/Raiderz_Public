#ifndef _XPOST_LOCAL_H
#define _XPOST_LOCAL_H

#include "XNetClient.h"
#include "XCommandTable.h"

// 테스트 관련 --------------------------------------------------------------------------------------
inline void XPostLocal_ReloadUI()
{
	XPOSTCMD0(MC_LOCAL_RELOAD_UI);
}


// 게임 관련
inline void XPostLocal_ChangeFrameFrame(int nGameFrame)
{
	XPOSTCMD1(MC_LOCAL_CHANGE_GAMEFRAME, MCommandParameterInt(nGameFrame));
}



#endif