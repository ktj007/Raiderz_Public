#include "StdAfx.h"
#include "XDummyBot_FillInventory.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyCollision.h"

namespace
{
	typedef vector<TD_ITEM> INVENTORY;
	const INVENTORY::size_type	RESPECT_ITEM_CNT	= 60;
	const int					RESPECT_ITEM_ID		= 62760;	//todo 리소스에서 얻어오는 식으로 변경 해야 함
}

XDummyBot_FillInventory::XDummyBot_FillInventory(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
{
}

XDummyBot_FillInventory::~XDummyBot_FillInventory(void)
{
	
}

void XDummyBot_FillInventory::OnRun( float fDelta )
{
}

minet::MCommandResult XDummyBot_FillInventory::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	assert(NULL!=pAgent);
	assert(NULL!=pCommand);
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	assert(NULL!=pClient);

	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		OnCommand_MC_CHAR_MYINFO(*pAgent, *pClient, *pCommand);
		pAgent->DelBot(this);
		break;
	}

	return CR_FALSE;
}

void XDummyBot_FillInventory::OnCommand_MC_CHAR_MYINFO(XBirdDummyAgent& agent,XBirdClient& client,MCommand& cmd)
{
	INVENTORY inventory;

	if (false == cmd.GetBlob(inventory, 1))
	{
		assert(NULL!=agent.GetName());
		if (NULL != agent.GetName())
		{
			mlog("%s > XDummyBot_FullEquip::OnCommand_MC_CHAR_MYINFO() - 인벤토리 추출 실패!", agent.GetName());
		}
		
		return;
	}

	int needCnt = RESPECT_ITEM_CNT - inventory.size();
	if (0 >= needCnt)
	{
		return;
	}

	XBIRDPOSTCMD2(&client, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
	for (int i = 0; i < needCnt; ++i)
	{
		XBIRDPOSTCMD2(&client, MC_GM_ITEM_GIVE_REQ, MCommandParameterInt(RESPECT_ITEM_ID), MCommandParameterInt(1));
	}
}
