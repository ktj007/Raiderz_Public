#pragma once

class XGlueGamePartyFunction
{
public:
	//ÆÄÆ¼
	void					PartyInviteReq();
	void					PartyRequiredAnswer(bool bAccept);
	void					PartyConfirmRequire();

	void					PartyLeave();
	void					PartyFired(const char* szuid);
	void					PartyDivisionRule();
	void					PartyMessage();

	void					PartyDemand();
	void					PartyDemandAnswer(bool bAccept);

	__int64					GetPartyLeaderUID();
	bool					IsPartyMemberOffline(const char* szuid);
	bool					IsPartyMemberDead(const char* szuid);
	bool					IsPartyMemberHightlight(const char* szuid);
	const char*				GetPartyMemberName(const char* szuid);
	int						GetPartyMemberHP(const char* szuid);
	int						GetPartyMemberEN(const char* szuid);
	int						GetPartyMemberSTA(const char* szuid);
	const char*				GetPartyMemberLevel(const char* szuid);
	const char*				GetPartyMemberClass(const char* szuid);

	void					SetSelectPlayer(const char* szuid);
};
