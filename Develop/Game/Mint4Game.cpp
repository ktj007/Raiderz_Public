#include "stdafx.h"
#include "Mint4Game.h"
#include "XCommandLogFrm.h"
#include "XUIMapViewer.h"
#include "XUIControlKey.h"
#include "XUIKeyMonitor.h"
#include "XUIHDRMonitor.h"
#include "XWeaponBindingMenu.h"
#include "XLuaLogFrm.h"
#include "XNarrationLabel.h"
#include "XColorPallete.h"
#include "XMLogFrm.h"
#include "XDebugFPSFrm.h"
#include "XDebugObjListFrm.h"
#include "XHelpFrm.h"
#include "XDebugNPCMonitor.h"
#include "XDebugCombat.h"
#include "XDebugRenderGraph.h"
#include "XDebugSoundFrm.h"
#include "XDebugLootFrm.h"
#include "XDebugMovableFrm.h"
#include "XDebugCategoryFrm.h"
#include "XDebugNPCLogFrm.h"
#include "XDebugCollisionFrm.h"

// New UI widgets
#include "XUICustomLook.h"
#include "XUIEquippedItemSlot.h"
#include "XUIPaletteSlot.h"
#include "XUITalentSlot.h"
#include "XUIChattingBox.h"
#include "XUIJournalIndicator.h"
#include "XUIFadeBox.h"
#include "XUIMiniMap.h"
#include "XUIFieldMap.h"
#include "XUIBuffViewer.h"
#include "XUIPresentationBox.h"
#include "XUIGauge.h"
#include "XUITray.h"
#include "XUIVideo.h"



MWidget* Mint4Game::NewWidget(const char* szClass, const char* szName, MWidget* pParent, MListener* pListener)
{
	MWidget* pNewWidget = Mint4R3::NewWidget(szClass, szName, pParent, pListener);
	if (pNewWidget != NULL) return pNewWidget;

	// 여기서 게임에서 새로 만든 위젯을 추가하면 됩니다.
		 if(strcmp(szClass, MINT_COMMANDLOGFRM)		== 0)	pNewWidget = new XCommandLogFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_MAPVIEWER)			== 0)	pNewWidget = new XUIMapViewer(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_CONTROLKEY)		== 0)	pNewWidget = new XUIControlKey(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_KEYMONITOR)		== 0)	pNewWidget = new XUIKeyMonitor(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_HDRMONITOR)		== 0)	pNewWidget = new XUIHDRMonitor(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_WEAPONBINDING_MENU)== 0)	pNewWidget = new XWeaponBindingMenu(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_LUALOGFRM)			== 0)	pNewWidget = new XLuaLogFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_NARRATIONPANEL)	== 0)	pNewWidget = new XNarrationLabel(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_COLOR_PALLETE)		== 0)	pNewWidget = new XColorPallete(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_MLOGFRM)			== 0)	pNewWidget = new XMLogFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGFPSFRM)		== 0)	pNewWidget = new XDebugFPSFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGOBJLISTFRM)	== 0)	pNewWidget = new XDebugObjListFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_HELPFRM)			== 0)	pNewWidget = new XHelpFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGNPCMONITOR)	== 0)	pNewWidget = new XDebugNPCMonitor(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGCOMBAT)		== 0)	pNewWidget = new XDebugCombat(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGRENDERGRAPH)	== 0)	pNewWidget = new XDebugRenderGraph(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGSOUND)		== 0)	pNewWidget = new XDebugSoundFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGLOOT)			== 0)	pNewWidget = new XDebugLootFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGMOVABLE)		== 0)	pNewWidget = new XDebugMovableFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGCATEGORYFRM)	== 0)	pNewWidget = new XDebugCategoryFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGNPCLOG)		== 0)	pNewWidget = new XDebugNPCLogFrm(szName, pParent, pListener);
	else if(strcmp(szClass, MINT_DEBUGCOLLISION)	== 0)	pNewWidget = new XDebugCollisionFrm(szName, pParent, pListener);
	// New UI widgets
	else if ( strcmp( szClass, MINT_EQUIPPEDITEMSLOT) == 0)		pNewWidget = new XUIEquippedItemSlot( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_PALETTESLOT) == 0)			pNewWidget = new XUIPaletteSlot( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_TALENTSLOT) == 0)			pNewWidget = new XUITalentSlot( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_CHATTINGBOX) == 0)			pNewWidget = new XUIChattingBox( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_JOURNALINDICATOR) == 0)		pNewWidget = new XUIJournalIndicator( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_FADEBOX) == 0)				pNewWidget = new XUIFadeBox( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_MINIMAP) == 0)				pNewWidget = new XUIMiniMap( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_FIELDMAP) == 0)				pNewWidget = new XUIFieldMap( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_BUFFVIEWER) == 0)			pNewWidget = new XUIBuffViewer( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_PRESENTATIONBOX) == 0)		pNewWidget = new XUIPresentationBox( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_GAUGE) == 0)				pNewWidget = new XUIGauge( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_TRAY) == 0)					pNewWidget = new XUITray( szName, pParent, pListener);
	else if ( strcmp( szClass, MINT_VIDEO) == 0)				pNewWidget = new XUIVideo( szName, pParent, pListener);
	else 
	{
		_ASSERT(false);
		pNewWidget = new MWidget(szName, pParent, pListener);
	}

	return pNewWidget;
}

Mint4Game::Mint4Game() : Mint4R3()
{

}

Mint4Game::~Mint4Game()
{

}

void Mint4Game::BeginDrawWidget(const wchar_t* szClassName)
{
	m_dwStartTime = timeGetTime();
}

void Mint4Game::EndDrawWidget(const wchar_t* szClassName)
{
	// Mint 위젯별로 그리는 속도를 추적하기 위해 아래의 주석을 풀면 됩니다
	REngine::GetResourceProfiler().UpdateTimeQueue( MLocale::ConvUTF16ToAnsi(szClassName).c_str(), timeGetTime() - m_dwStartTime );
}
