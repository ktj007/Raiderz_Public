#include "StdAfx.h"
#include "XDebugNPCMonitor.h"
#include "XNetDebuger.h"

XDebugNPCMonitor::XDebugNPCMonitor( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	Mint* pMint = global.ui->GetMint();

	ShowTitleBar( false);
	SetRect(pMint->GetWorkspaceWidth()-600, pMint->GetWorkspaceHeight()-400, 600, 400);
//	SetOpacity(0.6f);
	SetText("NPC Monitor");
}

void XDebugNPCMonitor::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	MRECT r = GetInitialClientRect();
	XNetDebuger::DebugNPCMonitorRender(pDC, r);
}

void XDebugNPCMonitor::SaveLog()
{
	XNetDebuger::SaveMonitorLog();
}