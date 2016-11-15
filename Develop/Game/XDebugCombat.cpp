#include "StdAfx.h"
#include "XDebugCombat.h"
#include "XNetDebuger.h"

XDebugCombat::XDebugCombat( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	Mint* pMint = global.ui->GetMint();

	ShowTitleBar( false);
	SetRect(10, pMint->GetWorkspaceHeight()-440, 350, 440);
//	SetOpacity(0.6f);
	SetText("Combat");
}

void XDebugCombat::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	MRECT r = GetInitialClientRect();
	XNetDebuger::DebugCombatRender(pDC, r);
}