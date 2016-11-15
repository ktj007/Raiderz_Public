#include "stdafx.h"
#include "PWorldIDChecker.h"
#include "PConfig.h"
#include "PLoginRouter.h"
#include "PGlobal.h"
#include "PLoginLogic.h"


PWorldIDChecker::PWorldIDChecker(MUID uidClient, int nWorldID)
: m_uidClient(uidClient)
, m_nWorldID(nWorldID)
{
	// do nothing
}

bool PWorldIDChecker::Check(void)
{
	return m_nWorldID == PConfig::m_nMyWorldID;
}

void PWorldIDChecker::OnFail(void)
{
	const PLoginRouter* pRouter = gsys.pLoginLogic->GetRouter();
	_ASSERT(pRouter != NULL);

	pRouter->RouteLoginResponse(m_uidClient, CRM_FAIL_LOGIN_INVALID_WORLD_ID);
}
