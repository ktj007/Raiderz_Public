#include "StdAfx.h"
#include "XDebugCollisionFrm.h"

XDebugCollisionFrm::XDebugCollisionFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	AddControls();

	SetRect(10, 120, 430, 60);
//	SetOpacity(0.9f);
	SetText("Collision");

	unsigned long int nOutputFlags = MLOG_PROG;
	if (XCONST::DEBUG_LOG_COMMAND) 
	{
		nOutputFlags |= MLOG_FILE;
		mlog("Start logging\n");
	}
}

void XDebugCollisionFrm::AddControls()
{
	m_pTerrainCol = new MButton(NULL, this, this);
	m_pTerrainCol->SetType(MBT_CHECKBTN);
	m_pTerrainCol->SetText("Terrain");
	m_pTerrainCol->SetRect(5, 20, 80, 16);

	m_pObjectCol = new MButton(NULL, this, this);
	m_pObjectCol->SetType(MBT_CHECKBTN);
	m_pObjectCol->SetText("Obejct");
	m_pObjectCol->SetRect(90, 20, 80, 16);

	m_pPlayerCol = new MButton(NULL, this, this);
	m_pPlayerCol->SetType(MBT_CHECKBTN);
	m_pPlayerCol->SetText("For Player");
	m_pPlayerCol->SetRect(175, 20, 100, 16);
}

bool XDebugCollisionFrm::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs/*=NULL*/ )
{
	if(MTextFrame::OnCommand(pWidget, nMsg, szArgs)) return true;

	if (pWidget == m_pTerrainCol || pWidget == m_pObjectCol || pWidget == m_pPlayerCol)
	{
		switch(nMsg)
		{
		case MEVENT_CLICK:
			{
				int nParam = 0;

 				((MButton*)pWidget)->SetCheck(!((MButton*)pWidget)->glueGetCheck());
 
				gvar.Debug.bShowMoveCollision_Real	= m_pTerrainCol->glueGetCheck();
				gvar.Debug.bShowCollision			= m_pObjectCol->glueGetCheck();
				gvar.Debug.bShowMoveCollision		= m_pPlayerCol->glueGetCheck();
 
				((MButton*)pWidget)->SetCheck(!((MButton*)pWidget)->glueGetCheck());

				return true;
			}
		}
	}

	return false;
}

void XDebugCollisionFrm::OnShow( bool bShow)
{
	m_pTerrainCol->glueSetCheck(gvar.Debug.bShowMoveCollision_Real);
	m_pObjectCol->glueSetCheck(gvar.Debug.bShowCollision);
	m_pPlayerCol->glueSetCheck(gvar.Debug.bShowMoveCollision);

	MWidget::OnShow( bShow);

	if ( bShow == true)		BringToTop();
}
