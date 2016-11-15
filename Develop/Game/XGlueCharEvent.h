#ifndef _XGLUE_CHAR_EVENT_H
#define _XGLUE_CHAR_EVENT_H

#include "XUISystem.h"

///////////////////////////////////////////////////////////////////////////
/// 캐릭터 선택, 생성, 삭제 관련 이벤트
class XGlueCharEvent
{
private:
	MWLua *					m_pMWLua;
public:
	XGlueCharEvent();
	void Init(MWLua* pLua);

	void OnRefresh()									{ WCALL(m_pMWLua, "shCharEvent_OnRefresh", WNULL); }
	void OnDeletedCharacter(bool bRet, const char* szErrorMsg)	{ WCALL2(m_pMWLua, "shCharEvent_OnDeletedCharacter", bRet, szErrorMsg, WNULL); }
	void OnCreatedCharacter(bool bRet, const char* szErrorMsg)	{ WCALL2(m_pMWLua, "shCharEvent_OnCreatedCharacter", bRet, szErrorMsg, WNULL); }
};


#endif 