#include "StdAfx.h"
#include "XDebugDashBoard.h"
#include "MMainFrame.h"
#include "XDebugFPSFrm.h"
#include "XDebugObjListFrm.h"
#include "XHelpFrm.h"
#include "XUIKeyMonitor.h"
#include "XDebugNPCMonitor.h"
#include "XDebugCombat.h"
#include "XDebugRenderGraph.h"
#include "XDebugSoundFrm.h"
#include "XDebugLootFrm.h"
#include "XDebugMovableFrm.h"
#include "XDebugCategoryFrm.h"
#include "XDebugNPCLogFrm.h"
#include "XDebugCollisionFrm.h"

XDebugDashBoard::XDebugDashBoard(void)
{
	m_nLayer = 0;
}

XDebugDashBoard::~XDebugDashBoard(void)
{
	m_vDebugFormList.clear();
}

void XDebugDashBoard::Create(Mint* pMint)
{
	m_vDebugFormList.clear();


	m_pCategory = (XDebugCategoryFrm*)pMint->NewWidget(MINT_DEBUGCATEGORYFRM, MINT_DEBUGCATEGORYFRM, pMint->GetMainFrame(), NULL);
	m_pCategory->Show(false);
	m_vDebugFormList.push_back(m_pCategory);

	m_pFPS = (XDebugFPSFrm*)pMint->NewWidget(MINT_DEBUGFPSFRM, MINT_DEBUGFPSFRM, pMint->GetMainFrame(), NULL);
	m_pFPS->Show(false);
	m_vDebugFormList.push_back(m_pFPS);

	m_pRenderGraph = (XDebugRenderGraph*)pMint->NewWidget(MINT_DEBUGRENDERGRAPH, MINT_DEBUGRENDERGRAPH, pMint->GetMainFrame(), NULL);
	m_pRenderGraph->Show(false);
	m_vDebugFormList.push_back(m_pRenderGraph);

	m_pHelp =  (XHelpFrm*)pMint->NewWidget(MINT_HELPFRM, MINT_HELPFRM, pMint->GetMainFrame(), NULL);
	m_pHelp->Show(false);
	m_vDebugFormList.push_back(m_pHelp);

	m_pObjList = (XDebugObjListFrm*)pMint->NewWidget(MINT_DEBUGOBJLISTFRM, MINT_DEBUGOBJLISTFRM, pMint->GetMainFrame(), NULL);
	m_pObjList->Show(false);
	m_vDebugFormList.push_back(m_pObjList);

	m_pCombat = (XDebugCombat*)pMint->NewWidget(MINT_DEBUGCOMBAT, MINT_DEBUGCOMBAT, pMint->GetMainFrame(), NULL);
	m_pCombat->Show(false);
	m_vDebugFormList.push_back(m_pCombat);

	m_pNPCMonitor = (XDebugNPCMonitor*)pMint->NewWidget(MINT_DEBUGNPCMONITOR, MINT_DEBUGNPCMONITOR, pMint->GetMainFrame(), NULL);
	m_pNPCMonitor->Show(false);
	m_vDebugFormList.push_back(m_pNPCMonitor);

	m_pNPCLog = (XDebugNPCLogFrm*)pMint->NewWidget(MINT_DEBUGNPCLOG, MINT_DEBUGNPCLOG, pMint->GetMainFrame(), NULL);
	m_pNPCLog->Show(false);
	m_vDebugFormList.push_back(m_pNPCLog);

	m_pSound = (XDebugSoundFrm*)pMint->NewWidget(MINT_DEBUGSOUND, MINT_DEBUGSOUND, pMint->GetMainFrame(), NULL);
	m_pSound->Show(false);
	m_pSound->SetRect( 20, 50, 700, 250);
	m_vDebugFormList.push_back(m_pSound);

	m_pLoot = (XDebugLootFrm*)pMint->NewWidget(MINT_DEBUGLOOT, MINT_DEBUGLOOT, pMint->GetMainFrame(), NULL);
	m_pLoot->Show(false);
	m_vDebugFormList.push_back(m_pLoot);

	m_pMovable = (XDebugMovableFrm*)pMint->NewWidget(MINT_DEBUGMOVABLE, MINT_DEBUGMOVABLE, pMint->GetMainFrame(), NULL);
	m_pMovable->Show(false);
	m_vDebugFormList.push_back(m_pMovable);

	m_vDebugFormList.push_back(global.ui->GetMLog());
	m_vDebugFormList.push_back(global.ui->GetCommandLog());

	m_pCollision = (XDebugCollisionFrm*)pMint->NewWidget(MINT_DEBUGCOLLISION, MINT_DEBUGCOLLISION, pMint->GetMainFrame(), NULL);
	m_pCollision->Show(false);
	m_vDebugFormList.push_back(m_pCollision);

	MFrame* pWidget = (MFrame*)global.mint->FindWidget("KeyMonitor");
	if(pWidget)
		m_vDebugFormList.push_back(pWidget);

	pWidget = (MFrame*)global.mint->FindWidget("HDRMonitor");
	if(pWidget)
		m_vDebugFormList.push_back(pWidget);
}

void XDebugDashBoard::PrevLayer()
{
	if (--m_nLayer < 0) m_nLayer = m_vDebugFormList.size()-1;
}

void XDebugDashBoard::NextLayer()
{
	if (++m_nLayer > (int)m_vDebugFormList.size()-1) m_nLayer=0;
}

bool XDebugDashBoard::OnDebugEvent( MEvent& e )
{
	switch(e.nMessage)
	{
		case MWM_KEYDOWN:
			{
				if (e.bCtrl == true && e.bAlt == false)
				{
					for (int i = 0; i < (int)m_vDebugFormList.size(); i++)
					{
						if (e.nKey == VK_F1 + i)
						{
							if(m_vDebugFormList[i]->GetShow())
							{
								m_vDebugFormList[i]->Show(false);

							}
							else
							{
								m_vDebugFormList[i]->Show(true);
							}
							return true;
						}
					}
				}
				if (e.bCtrl == true && e.bAlt == true)
				{
					if (e.nKey == 'Z')
					{
						m_pNPCMonitor->SaveLog();
						m_pNPCLog->SaveLog();
						return true;
					}
				}
			}
			break;
		case MWM_CHAR:
			{
#ifdef _DEBUG
				switch (e.nKey) 
				{
				case 'N':
					{
						PrevLayer();
						if(m_vDebugFormList[m_nLayer]->GetShow())
						{
							m_vDebugFormList[m_nLayer]->Show(false);

						}
						else
						{
							m_vDebugFormList[m_nLayer]->Show(true);
						}
						return true;
					}
					break;
				case 'n':
					{
						NextLayer();
						if(m_vDebugFormList[m_nLayer]->GetShow())
						{
							m_vDebugFormList[m_nLayer]->Show(false);

						}
						else
						{
							m_vDebugFormList[m_nLayer]->Show(true);
						}

						return true;
					}
					break;
				}
#endif
			}
	}

	return false;
}

void XDebugDashBoard::HideAll()
{
	for (int i = 0; i < (int)m_vDebugFormList.size(); i++)
	{
		if(m_vDebugFormList[i]->GetShow())
		{
			m_vDebugFormList[i]->Show(false);

		}
	}
}

void XDebugDashBoard::ShowCategoryAI()
{
	HideAll();
	m_pCategory->Show();

	m_pFPS->Show();
	m_pObjList->Show();
	m_pNPCMonitor->Show();
	m_pCombat->Show();
}

void XDebugDashBoard::ShowCategorySync()
{
	HideAll();
	m_pCategory->Show();

	m_pFPS->Show();
	m_pObjList->Show();
	m_pMovable->Show();
}

void XDebugDashBoard::ShowNPCMonitor( bool bShow /*= false*/ )
{
	if(m_pNPCMonitor->GetShow() && !bShow)	m_pNPCMonitor->Show(false);
	else									m_pNPCMonitor->Show(true);
}

void XDebugDashBoard::ShowFPS()
{
	if(m_pFPS->GetShow())	m_pFPS->Show(false);
	else					m_pFPS->Show(true);
}

void XDebugDashBoard::ShowObjList()
{
	if(m_pObjList->GetShow())	m_pObjList->Show(false);
	else						m_pObjList->Show(true);
}

void XDebugDashBoard::ShowHelp()
{
	if(m_pHelp->GetShow())		m_pHelp->Show(false);
	else						m_pHelp->Show(true);
}

void XDebugDashBoard::ShowCombat()
{
	if(m_pCombat->GetShow())	m_pCombat->Show(false);
	else						m_pCombat->Show(true);
}

void XDebugDashBoard::ShowRenderGraph()
{
	if(m_pRenderGraph->GetShow())	m_pRenderGraph->Show(false);
	else							m_pRenderGraph->Show(true);
}

void XDebugDashBoard::ShowSound()
{
	if(m_pSound->GetShow())		m_pSound->Show(false);
	else						m_pSound->Show(true);
}

void XDebugDashBoard::ShowLoot()
{
	if(m_pLoot->GetShow())		m_pLoot->Show(false);
	else						m_pLoot->Show(true);
}

void XDebugDashBoard::ShowMovable()
{
	if(m_pMovable->GetShow())	m_pMovable->Show(false);
	else						m_pMovable->Show(true);
}

void XDebugDashBoard::ShowMlog()
{
	MWidget* pWidget = global.ui->GetMLog();

	if(pWidget->GetShow())		pWidget->Show(false);
	else						pWidget->Show(true);
}

void XDebugDashBoard::ShowClog()
{
	MWidget* pWidget = global.ui->GetCommandLog();

	if(pWidget->GetShow())		pWidget->Show(false);
	else						pWidget->Show(true);
}

void XDebugDashBoard::ShowNPCLog()
{
	if(m_pNPCLog->GetShow())	m_pNPCLog->Show(false);
	else						m_pNPCLog->Show(true);
}

void XDebugDashBoard::SetNPCLog(vector<TD_DEBUG_NPCLOG>& pVecTDDebugNPCLog)
{
	m_pNPCLog->SetNPCLog(pVecTDDebugNPCLog);
}

void XDebugDashBoard::ShowCollision()
{
	if(m_pCollision->GetShow())	m_pCollision->Show(false);
	else						m_pCollision->Show(true);
}