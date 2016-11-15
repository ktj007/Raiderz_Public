#pragma once

#include "Mint4Game.h"
#include "MWidget.h"

using namespace std;
using namespace mint3;

class XDebugFPSFrm;
class XDebugObjListFrm;
class XHelpFrm;
class XDebugNPCMonitor;
class XDebugCombat;
class XDebugRenderGraph;
class XDebugSoundFrm;
class XDebugLootFrm;
class XDebugMovableFrm;
class XDebugCategoryFrm;
class XDebugNPCLogFrm;
class XDebugCollisionFrm;

class XDebugDashBoard
{
public:

private:
	XDebugFPSFrm*			m_pFPS;
	XDebugObjListFrm*		m_pObjList;
	XHelpFrm*				m_pHelp;
	XDebugNPCMonitor*		m_pNPCMonitor;
	XDebugCombat*			m_pCombat;
	XDebugRenderGraph*		m_pRenderGraph;
	XDebugSoundFrm*			m_pSound;
	XDebugLootFrm*			m_pLoot;
	XDebugMovableFrm*		m_pMovable;
	XDebugNPCLogFrm*		m_pNPCLog;
	XDebugCollisionFrm*		m_pCollision;

	XDebugCategoryFrm*		m_pCategory;

	vector<mint3::MWidget*>		m_vDebugFormList;

	int						m_nLayer;

	void PrevLayer();
	void NextLayer();

	void HideAll();

public:
	XDebugDashBoard(void);
	~XDebugDashBoard(void);
	void Create(Mint* pMint);
	bool OnDebugEvent( MEvent& e );

	void ShowCategoryAI();
	void ShowCategorySync();

	void ShowNPCMonitor(bool bShow = false);
	void ShowFPS();
	void ShowObjList();
	void ShowHelp();
	void ShowCombat();
	void ShowRenderGraph();
	void ShowSound();
	void ShowLoot();
	void ShowMovable();
	void ShowMlog();
	void ShowClog();
	void ShowNPCLog();
	void SetNPCLog(vector<TD_DEBUG_NPCLOG>& pVecTDDebugNPCLog);
	void ShowCollision();
};
