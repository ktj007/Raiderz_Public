#pragma once

#include "PDef.h"

class PLoginRouter;
class PCheckRunner;

class PLoginLogic
{
public:
	PLoginLogic();
	virtual ~PLoginLogic();

	virtual PLoginRouter* NewRouter(void);
	virtual PCheckRunner* NewCheckRunner(void);
	const PLoginRouter* GetRouter(void) const;

	void RequestLogin(MUID uidClient, const SERVER_LOGIN_DESC& desc) const;

private:
	PLoginRouter* m_pRouter;
	PCheckRunner* m_pChecker;
};
