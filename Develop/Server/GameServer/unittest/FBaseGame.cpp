#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"


FBaseGame::FBaseGame()
{
	m_pField = GUTHelper_Field::DefaultMockField();

	InitPlayerAndLink();
}

FBaseGame::~FBaseGame()
{
	m_pField->Destroy();
}

void FBaseGame::InitPlayerAndLink()
{
	m_pPlayer = NewEntityPlayer(m_pField, vec3::ZERO);

	// -- link
	MUID uidLink = m_pPlayer->GetUID();
	m_pLink = NewLink(m_pPlayer);
}

void FBaseGame::InitPlayerAndLink( GEntityPlayer** outppPlayer, MockLink** outppLink )
{
	*outppPlayer = NewEntityPlayer(m_pField, vec3::ZERO);

	// -- link
	MUID uidLink = (*outppPlayer)->GetUID();
	*outppLink = NewLink(*outppPlayer);
}