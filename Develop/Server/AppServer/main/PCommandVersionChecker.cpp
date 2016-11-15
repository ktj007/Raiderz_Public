#include "stdafx.h"
#include "PCommandVersionChecker.h"
#include "CCommandTable.h"
#include "PLoginRouter.h"
#include "PGlobal.h"
#include "PLoginLogic.h"

PCommandVersionChecker::PCommandVersionChecker(MUID uidClient, int nCommandVersion)
: m_uidClient(uidClient)
, m_nCommandVersion(nCommandVersion)
{
	// do nothing
}

bool PCommandVersionChecker::Check(void)
{
	return CCommandVersionChecker::IsValid(m_nCommandVersion);
}

void PCommandVersionChecker::OnFail(void)
{
	const PLoginRouter* pRouter = gsys.pLoginLogic->GetRouter();
	_ASSERT(pRouter != NULL);

	pRouter->RouteLoginResponse(m_uidClient, CRM_FAIL_LOGIN_COMMAND_INVALID_VERSION);
}
