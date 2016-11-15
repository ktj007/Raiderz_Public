#include "stdafx.h"
#include "XLoadingChecker_Server.h"
#include "XGame.h"
#include "XWorld.h"
#include "XLoadingMgr.h"
#include "XBaseApplication.h"

bool XLoadingChecker_Server::CheckCompleted()
{
	if (global.app && global.app->GetMode() != EM_GAME)
	{
		return true;
	}

	if (m_bRecevedCommand_MC_FIELD_START_GAME == false) return false;

	return true;
}

void XLoadingChecker_Server::OnEvent( XEvent& evt )
{
	switch (evt.m_nID)
	{
	case XLOADING_EVT_ON_FIELD_START_GAME:
		{
			m_bRecevedCommand_MC_FIELD_START_GAME = true;
		}
		break;
	}
}