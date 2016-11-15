#include "StdAfx.h"
#include "XDebugMovableFrm.h"
#include "XMovableDebugger.h"
#include "XPost_GM.h"

XDebugMovableFrm::XDebugMovableFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	MRECT r = GetInitialClientRect();

	Mint* pMint = global.ui->GetMint();

	ShowTitleBar( false);
	SetRect(pMint->GetWorkspaceWidth()-770, 0, 770, 460);
//	SetOpacity(0.6f);
	SetText("MovableDebugger Test");
}

void XDebugMovableFrm::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	if (XMovableDebugger::GetInstancePtr())
	{
		XMovableDebugger::GetInstancePtr()->DrawFrm(pDC);
	}
}

void XDebugMovableFrm::OnShow( bool bShow)
{
	MTextFrame::OnShow( bShow);

	if ( bShow == true)
	{
		XPostDebugString(L"show_actor_pos", 1, L"");
		gvar.Debug.bRenderMoveDebug = true;

		BringToTop();
	}
	else
	{
		XPostDebugString(L"show_actor_pos", 0, L"");
		gvar.Debug.bRenderMoveDebug = false;
	}
}