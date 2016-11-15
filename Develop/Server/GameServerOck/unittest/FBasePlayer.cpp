#include "stdafx.h"
#include "FBasePlayer.h"
#include "MockPlayerObject.h"
#include "GEntityPlayer.h"
#include "GUTHelper.h"
#include "GHateTable.h"

MockPlayerObject* FBasePlayer::NewMockPlayerObject( MUID uidPlayerObject )
{
	MockPlayerObject* p = new MockPlayerObject(uidPlayerObject);
	p->Create();
	return p;
}

GEntityPlayer* FBasePlayer::NewEntityPlayer( GField* pField, vec3 pos/*=vec3::ZERO*/, MUID uidNew/*=MUID::Invalid()*/ )
{
	return GUTHelper::NewEntityPlayer(pField, pos, uidNew);
}

void FBasePlayer::DeleteEntityPlayer( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	VALID(pPlayer->GetField());

	GField* pField = pPlayer->GetField();
	pPlayer->DeleteMe();
	pField->Update(0.0f);
}

void FBasePlayer::SetStatus( GEntityPlayer* pEntityPlayer, int nHP, int nEN, int nSTA )
{
	PLAYER_INFO* player_info = pEntityPlayer->GetPlayerInfo();
	player_info->nMaxHP = nHP;
	player_info->nMaxEN = nEN;
	player_info->nMaxSTA = nSTA;

	pEntityPlayer->SetHP(nHP);
	pEntityPlayer->SetEN(nEN);
	pEntityPlayer->SetSTA(nSTA);
}

void FBasePlayer::MakeCombat( GEntityPlayer* pEntityPlayer, GEntityNPC* pOpponent )
{
	VALID(pEntityPlayer);
	VALID(pOpponent);
	pOpponent->GetHateTable().AddPoint_FoundEnemy(pEntityPlayer);
}