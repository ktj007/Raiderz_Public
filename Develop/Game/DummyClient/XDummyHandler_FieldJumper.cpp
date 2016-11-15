#include "stdafx.h"
#include "XDummyHandler_FieldJumper.h"
#include "XBirdDummyAgent.h"
#include "XFieldInfo.h"
#include "XMyPlayer.h"
#include "XPost_GM.h"
#include "XDummyHelper.h"

MCommandResult XDummyHandler_FieldJumper::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_START_GAME:
		{
			// 이동 필드 선택
			int nIgnoreFieldID = pAgent->GetLoginFieldID();
			int nFieldID = XDummyHelper::GetRandomFieldID(nIgnoreFieldID);
			if (nFieldID == INVALID_ID)		return CR_ERROR;

			// 이동
			vec3 vPos = pAgent->GetLoginFieldStartPos();		
			XBIRDPOSTCMD2(pClient, MC_GM_MOVE_REQ, MCommandParameterInt(nFieldID), MCommandParameterVector(vPos));
		}
		
		return CR_TRUE;	
	}

	return CR_FALSE;
}
