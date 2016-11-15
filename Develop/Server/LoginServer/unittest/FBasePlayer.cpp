#include "StdAfx.h"
#include "FBasePlayer.h"
#include "SUnitTestUtil.h"
#include "SAccountInfo.h"
#include "LPlayerObject.h"


LPlayerObject* FBasePlayer::NewTestPlayerObject(MUID uidPlayer)
{
	if (uidPlayer.IsInvalid())
		uidPlayer = SUnitTestUtil::NewUID();

	SCommObject* pPlayer = gmgr.pPlayerObjectManager->NewObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddObject(pPlayer);
	return reinterpret_cast<LPlayerObject*>(pPlayer);
}

LPlayerObject* FBasePlayer::NewLoginedTestPlayerObject(MUID uidPlayer, AID nAID)
{
	if (nAID < 1)
		nAID = SUnitTestUtil::NewID();

	LPlayerObject* pPlayer = NewTestPlayerObject(uidPlayer);

	ACCOUNT_INFO info;
	info.nAID = nAID;
	_tcscpy_s(info.szUserID, ACCOUNT_NAME_LEN + 1, SUnitTestUtil::NewName().c_str());
	pPlayer->InitAccountInfo(info);

	SAccountCharFieldInfo char_info;
	pPlayer->AddCharFieldInfo(char_info);

	return reinterpret_cast<LPlayerObject*>(pPlayer);
}