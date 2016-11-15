#pragma once

#include "XGlueFunction.h"

/// lua에서 사용하는 게임 API
class XGlueGameFunction : public XGlueFunction
{
private:
	wstring					m_strStringBuffer;
public:
	virtual void DeclGlueFunc(MWLua* pLua);

	// 채팅
	void					ChatGlobal(const char* szMsg);

	// 대화
	void					DialogButtonSel(int nSel);
	const char*				XGetString(const char* nKey);
	const char*				XGetString2(const char* nKey);

	void					UnHoldMyCharacter();

	// 로그인

	// 게임 종료

	// 네트워크
	void					Connect(const char* szLoginID, const char* szPassword, const char* szIP);
	void					Disconnect();

	const char*				GetPlayerID( MUID uid);
	const char*				GetInteractivePlayerName();

	//부활
	void					RequireRebirth(int nSelect);

	// Buff 정보
	const char*				GetBuffName(int nBuffID);
	bool					IsBuff(int nBuffID);

	unsigned int			GetDynamicFieldRemainTime();

	int						GameTime();
	int						GameWeather();

	int					GetStringWidth(const char* szWidgetName, const char* szMsg);

	// Feedback
	void					SendFeedback();
	void					ReloadFeedback();
};
