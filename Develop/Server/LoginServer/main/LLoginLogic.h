#pragma once

struct TD_PMANG_LOGIN_DATA;

class LLoginLogic
{
private:
	void DelPlayerObject( MUID uidPlayer );
	void AddPlayerObject( MUID uidPlayer );
public:
	LLoginLogic();

	bool Login(MUID uidPlayer, wstring strUserID, wstring strPassword);
	bool PmangLogin(MUID uidPlayer, wstring strLoginStream);
};
