#include "StdAfx.h"
#include "XDummyBot_LevelUp.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyCollision.h"

XDummyBot_LevelUp::XDummyBot_LevelUp(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
{
}

XDummyBot_LevelUp::~XDummyBot_LevelUp(void)
{
	
}

void XDummyBot_LevelUp::OnRun( float fDelta )
{
}

minet::MCommandResult XDummyBot_LevelUp::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	assert(NULL!=pAgent);
	assert(NULL!=pCommand);
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	assert(NULL!=pClient);

	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO: {
		TD_MYINFO vecTDMyInfo;

		if (pCommand->GetSingleBlob(vecTDMyInfo, 0)==false) return CR_FALSE;

		if(3 != vecTDMyInfo.nPlayerGrade)
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
		}

		if (50 != vecTDMyInfo.nLevel)
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"level"), MCommandParameterWideString(L"50"));
		}
		
		pAgent->DelBot(this);
		break;
						 }
	}

	return CR_FALSE;
}
