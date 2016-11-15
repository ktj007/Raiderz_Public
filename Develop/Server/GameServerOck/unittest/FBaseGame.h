#pragma once

#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "FBaseMockLink.h"
#include "FBaseNetClient.h"
#include "FBasePlayer.h"
#include "FBaseNPC.h"
#include "FBaseTalent.h"
#include "FBaseCombat.h"
#include "GUTHelper.h"
#include "GCommandTable.h"
#include "GPlayerObjectManager.h"
#include "MockDBManager.h"
#include "MockField.h"

class FBaseGame : public FBaseMockLink, public FBaseNetClient, public FBasePlayer, public FBaseFieldInfo, public FBaseFieldMgr, public FBaseNPC, public FBaseTalent, public FBaseCombat
{
public:
	FBaseGame();
	virtual ~FBaseGame();

	void InitPlayerAndLink();
	void InitPlayerAndLink(GEntityPlayer** outppPlayer, MockLink** outppLink);

	MockField* m_pField;
	MockLink*		m_pLink;
	GEntityPlayer*	m_pPlayer;
};
