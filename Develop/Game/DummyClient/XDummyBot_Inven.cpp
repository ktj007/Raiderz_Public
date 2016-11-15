#include "StdAfx.h"
#include "XDummyBot_Inven.h"
#include "XBirdClient.h"
#include "XBirdDummyAgent.h"


XDummyBot_Inven::XDummyBot_Inven(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo), m_nPhase(PHASE_INIT)
{
}

XDummyBot_Inven::~XDummyBot_Inven()
{
}

MCommandResult XDummyBot_Inven::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_CHANGE_FIELD:
		{
		}
		break;
	case MC_ITEM_ADD:
		{
			vector<TD_ITEM_INSERT> vecTDItemInsert;
			if (pCommand->GetBlob(vecTDItemInsert, 0)==false) return CR_ERROR;

			if (1 <= vecTDItemInsert.size())
			{
				ChangePhase(PHASE_SWAPING_SLOT1_REQ);
			}
		}
		break;
	case MC_ITEM_MOVE:
		{
			if (m_nPhase == PHASE_SWAPING_SLOT1)
				ChangePhase(PHASE_SWAPING_SLOT2_REQ);
			else if (m_nPhase == PHASE_SWAPING_SLOT2)
				ChangePhase(PHASE_SWAPING_SLOT1_REQ);
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Inven::OnRun(float fDelta)
{
	switch (m_nPhase)
	{
	case PHASE_INIT:
		{
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"level"), MCommandParameterWideString(L"50"));
			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_GOD_REQ);

			XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_CLEAR_INVENTORY_REQ);
			XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(1), MCommandParameterInt(1));
			

			ChangePhase(PHASE_INIT_DONE);
		}
		break;
	case PHASE_SWAPING_SLOT1_REQ:
		{
			XBIRDPOSTCMD5(m_pAgent->GetClient(), MC_ITEM_MOVE_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(1), MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(2), MCommandParameterInt(1));

			ChangePhase(PHASE_SWAPING_SLOT1);
		}
		break;
	case PHASE_SWAPING_SLOT2_REQ:
		{
			XBIRDPOSTCMD5(m_pAgent->GetClient(), MC_ITEM_MOVE_REQ, MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(2), MCommandParameterInt(SLOTTYPE_INVENTORY), MCommandParameterInt(1), MCommandParameterInt(1));

			ChangePhase(PHASE_SWAPING_SLOT2);
		}
		break;
	}
	
}