#pragma once

class LPlayerObject;
struct TD_PMANG_LOGIN_DATA;

class LLoginLogic
{
private:
	void DelPlayerObject( MUID uidPlayer );
	LPlayerObject*	AddPlayerObject( MUID uidPlayer );
public:
	LLoginLogic();

	bool Login(MUID uidPlayer, wstring strUserID, wstring strPassword);
	bool PmangLogin(MUID uidPlayer, wstring strLoginStream);
	bool PWELogin(MUID uidPlayer, wstring strUserID, wstring strPassword);
};
