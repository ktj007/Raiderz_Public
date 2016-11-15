#pragma once

class GPlayerObject;

class GGMExpoHandler
{
public:
	GGMExpoHandler(void);
	~GGMExpoHandler(void);

	void ResetCharacters();
	void ResetPublicBoothCharacter(GPlayerObject* pPlayerObject);
	bool IsExpoAccount(wstring strUserID);
	bool IsExpoPublicBoothAccount(wstring strUserID);
};
