#pragma once

#include "CSChrInfo.h"

class LPlayerObject;
struct TD_INSERT_CHARACTER_INFO;

class LPlayerLogic
{
public:
	bool RequestAccountCharList(MUID uidPlayer);
	bool InsertCharacter(MUID uidPlayer, TD_INSERT_CHARACTER_INFO* pInsertCharInfo);
	bool DeleteCharacter(MUID uidPlayer, int8 nCharIndex);
	bool SelMyChar(MUID uidPlayer, int8 nCharIndex);
	void DuplicatedPlayerLogin(MUID uidPlayer, MUID uidConnectionKey);
	void OnDisconnectPlayer(MUID uidPlayer);

private:
	bool CheckInsertCharacter_Account(LPlayerObject* pPlayerObject);
	bool CheckInsertCharacter_Parameter(MUID uidPlayer, TD_INSERT_CHARACTER_INFO* pInsertCharInfo);
	bool ApplyInsertCharacter(MUID uidPlayer, AID nAID, TD_INSERT_CHARACTER_INFO* pInsertCharInfo);
};
