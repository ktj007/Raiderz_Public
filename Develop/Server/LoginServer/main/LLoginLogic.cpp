#include "stdafx.h"
#include "LLoginLogic.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "SDef.h"
#include "LConfig.h"
#include "LGlobal.h"
#include "LDBManager.h"
#include "LCommandCenter.h"
#include "CCommandTable_Login.h"
#include "CTransData.h"
#include "PmLoginFacade.h"
#include "PmDef.h"
#include "LDBTaskDataAccount.h"
#include "MLocale.h"


LLoginLogic::LLoginLogic()
{
}

bool LLoginLogic::Login(MUID uidPlayer, wstring strUserID, wstring strPassword)
{
	LPlayerObject* pNewPlayerObj = AddPlayerObject(uidPlayer);

	LDBT_ACC_LOGIN data(uidPlayer, strUserID, L"", strUserID, strPassword, pNewPlayerObj->GetRemoteIP());

	bool bRet = gsys.pDBManager->LoginGetInfo(data);

	if (!bRet)
	{
		DelPlayerObject(uidPlayer);
	}

	return bRet;
}

bool LLoginLogic::PmangLogin(MUID uidPlayer, wstring strLoginStream)
{
	return false;
}

bool LLoginLogic::PWELogin(MUID uidPlayer, wstring strUserID, wstring strPassword)
{
	LPlayerObject* pNewPlayerObj = AddPlayerObject(uidPlayer);

	LDBT_ACC_LOGIN data(uidPlayer, strUserID, L"", strUserID, strPassword, pNewPlayerObj->GetRemoteIP());

	bool bRet = gsys.pDBManager->PWELoginGetInfo(data);

	if (!bRet)
	{
		DelPlayerObject(uidPlayer);
	}

	return bRet;
}

LPlayerObject* LLoginLogic::AddPlayerObject( MUID uidPlayer )
{
	LPlayerObject* pNewPlayer = (LPlayerObject*)gmgr.pPlayerObjectManager->NewObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddObject(pNewPlayer);
	return pNewPlayer;
}

void LLoginLogic::DelPlayerObject( MUID uidPlayer )
{
	gmgr.pPlayerObjectManager->DeleteObject(uidPlayer);
}