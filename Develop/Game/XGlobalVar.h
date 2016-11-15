#ifndef _XGLOBAL_VAR_H
#define _XGLOBAL_VAR_H

#include <string>
using namespace std;

#include "CSDef.h"
#include "XGlobalVar_MyInfo.h"
#include "XGlobalVar_Network.h"
#include "XGlobalVar_Char.h"
#include "XGlobalVar_World.h"
#include "XGlobalVar_Temp.h"
#include "XGlobalVar_Debug.h"
#include "XGlobalVar_Game.h"


/// 전역적으로 사용하는 변수
class GlobalVar : public MSingleton<GlobalVar>
{
public:
	GlobalVar_Network		Network;
	GlobalVar_MyInfo		MyInfo;
	GlobalVar_Temp			Temp;
	GlobalVar_Debug			Debug;
	GlobalVar_Game			Game;
	XGlobalVar_World		World;
	GlobalVar_Char			Char;		///< 캐릭터 선택 관련
};
#define gvar		GlobalVar::GetInstance()

inline MUID XGetMyUID()
{
	return gvar.MyInfo.MyUID;
}

inline UIID XGetMyUIID()
{
	return gvar.MyInfo.GetUIID();
}

inline XMyPlayer* XGetMyPlayer()
{
	return gvar.Game.pMyPlayer;
}

inline wchar_t* XGetMyName()
{
	return gvar.MyInfo.ChrInfo.szName;
}

inline bool AmIGod()
{
#ifdef _PUBLISH
	return gvar.MyInfo.bGod;
#endif
	return false;
}

inline XInteractionInfo& XGetInteractionInfo()
{
	return GlobalVar::GetInstance().Game.GetInteractionInfo();
}


#endif