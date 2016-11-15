#include "stdafx.h"
#include "XMintScriptDeclarer.h"
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




void XMintScriptDeclarer::InitDeclWidgets(MWLua* pLua)
{
	MWLua::class_<XUIEquippedItemSlot>(pLua->GetState(), MINT_EQUIPPEDITEMSLOT)
		.inh<MWidget>()
		.def( "GetSlotType",			&XUIEquippedItemSlot::glueGetSlotType)
		.def( "GetItemID",				&XUIEquippedItemSlot::glueGetItemID)
		.def( "GetDurability",			&XUIEquippedItemSlot::glueGetDurability)
		.def( "GetRepairPrice",			&XUIEquippedItemSlot::glueGetRepairPrice)
		.def( "IsEmpty",				&XUIEquippedItemSlot::glueIsEmpty)
		.def( "UpdateInfo",				&XUIEquippedItemSlot::glueUpdateInfo)
		.def( "DoUseItem",				&XUIEquippedItemSlot::glueDoUseItem)
		.def( "DoUnequipItem",			&XUIEquippedItemSlot::glueDoUnequipItem)
		;

	MWLua::class_<XUIPaletteSlot>(pLua->GetState(), MINT_PALETTESLOT)
		.inh<MWidget>()
		.def( "GetSlotIndex",			&XUIPaletteSlot::glueGetSlotIndex)
		.def( "GetID",					&XUIPaletteSlot::glueGetID)
		.def( "GetType",				&XUIPaletteSlot::glueGetType)
		.def( "IsEmpty",				&XUIPaletteSlot::glueIsEmpty)
		.def( "IsTalent",				&XUIPaletteSlot::glueIsTalent)
		.def( "IsItem",					&XUIPaletteSlot::glueIsItem)
		.def( "UpdateInfo",				&XUIPaletteSlot::glueUpdateInfo)
		.def( "DoUsePaletteSlot",		&XUIPaletteSlot::glueDoUsePaletteSlot)
		;

	MWLua::class_<XUITalentSlot>(pLua->GetState(), MINT_TALENTSLOT)
		.inh<MWidget>()
		.def( "GetTalentID",			&XUITalentSlot::glueGetTalentID)
		.def( "GetTalentName",			&XUITalentSlot::glueGetTalentName)
		.def( "UpdateInfo",				&XUITalentSlot::glueUpdateInfo)
		.def( "DoLearnTalent",			&XUITalentSlot::glueDoLearnTalent)
		.def( "IsLearnable",			&XUITalentSlot::glueIsLearnable)
		;

	MWLua::class_<XUIChattingBox>(pLua->GetState(), MINT_CHATTINGBOX)
		.inh<MWidget>()
		.def( "CreateChatBox",			&XUIChattingBox::glueCreateChatBox)
		.def( "GetChatBoxCount",		&XUIChattingBox::glueGetChatBoxCount)
		.def( "GetChatBoxID",			&XUIChattingBox::glueGetChatBoxID)
		.def( "GetChatBoxRect",			&XUIChattingBox::glueGetChatBoxRect)
		.def( "CreateChatTab",			&XUIChattingBox::glueCreateChatTab)
		.def( "DeleteChatTab",			&XUIChattingBox::glueDeleteChatTab)
		.def( "SetChatTabName",			&XUIChattingBox::glueSetChatTabName)
		.def( "GetChatTabName",			&XUIChattingBox::glueGetChatTabName)
		.def( "GetChatBoxTabCount",		&XUIChattingBox::glueGetChatBoxTabCount)
		.def( "GetChatBoxTabID",		&XUIChattingBox::glueGetChatBoxTabID)
		.def( "AddChat",				&XUIChattingBox::glueAddChat)
		.def( "ClearChat",				&XUIChattingBox::glueClearChat)
		.def( "AddTypeFilter",			&XUIChattingBox::glueAddTypeFilter)
		.def( "ClearTypeFilter",		&XUIChattingBox::glueClearTypeFilter)
		.def( "GetTypeFilterCount",		&XUIChattingBox::glueGetTypeFilterCount)
		.def( "GetTypeFilter",			&XUIChattingBox::glueGetTypeFilter)
		.def( "SetTextSize",			&XUIChattingBox::glueSetTextSize)
		.def( "GetTextSize",			&XUIChattingBox::glueGetTextSize)
		.def( "SetTabCurSel",			&XUIChattingBox::glueSetTabCurSel)
		.def( "GetTabCurSel",			&XUIChattingBox::glueGetTabCurSel)
		.def( "ActivateChatInput",		&XUIChattingBox::glueActivateChatInput)
		.def( "IsChatInputActivated",	&XUIChattingBox::glueIsChatInputActivated)
		.def( "SetFixed",				&XUIChattingBox::glueSetFixed)
		.def( "IsFixed",				&XUIChattingBox::glueIsFixed)
		.def( "SetEditText",			&XUIChattingBox::glueSetEditText)
		.def( "SetPrefix",				&XUIChattingBox::glueSetPrefix)
		.def( "GetPrefix",				&XUIChattingBox::glueGetPrefix)
		.def( "GetPrefixName",			&XUIChattingBox::glueGetPrefixName)
		;

	MWLua::class_<XUIJournalIndicator>(pLua->GetState(), MINT_JOURNALINDICATOR)
		.inh<MWidget>()
		.def( "AddItem",				&XUIJournalIndicator::glueAddItem)
		.def( "RemoveItem",				&XUIJournalIndicator::glueRemoveItem)
		.def( "RefreshItem",			&XUIJournalIndicator::glueRefreshItem)
		.def( "IsEmpty",				&XUIJournalIndicator::glueIsEmpty)
		.def( "Clear",					&XUIJournalIndicator::glueClear)
		.def( "RefreshAll",				&XUIJournalIndicator::glueRefreshAll)
		.def( "GetFocused",				&XUIJournalIndicator::glueGetFocused)
		;

	MWLua::class_<XUIFadeBox>(pLua->GetState(), MINT_FADEBOX)
		.inh<MWidget>()
		;

	MWLua::class_<XUIMiniMap>(pLua->GetState(), MINT_MINIMAP)
		.inh<MWidget>()
		.def( "SetScale",				&XUIMiniMap::glueSetScale)
		.def( "GetScale",				&XUIMiniMap::glueGetScale)
		.def( "ZoomIn",					&XUIMiniMap::glueZoomIn)
		.def( "ZoomOut",				&XUIMiniMap::glueZoomOut)
		.def( "RotateMap",				&XUIMiniMap::glueRotateMap)
		.def( "IsRotateMap",			&XUIMiniMap::glueIsRotateMap)
		.def( "UpdateInfo",				&XUIMiniMap::glueUpdateInfo)
		.def( "AddMarkerArtisan",		&XUIMiniMap::glueAddMarkerArtisan)
		;

	MWLua::class_<XUIFieldMap>(pLua->GetState(), MINT_FIELDMAP)
		.inh<MWidget>()
		.def( "SetScale",				&XUIFieldMap::glueSetScale)
		.def( "GetScale",				&XUIFieldMap::glueGetScale)
		.def( "SetFullScale",			&XUIFieldMap::glueFullScale)
		.def( "IsFullScale",			&XUIFieldMap::glueIsFullScale)
		.def( "ZoomIn",					&XUIFieldMap::glueZoomIn)
		.def( "ZoomOut",				&XUIFieldMap::glueZoomOut)
		.def( "UpdateInfo",				&XUIFieldMap::glueUpdateInfo)
		.def( "ResetDragOffset",		&XUIFieldMap::glueResetDragOffset)
		.def( "AddMarkerArtisan",		&XUIFieldMap::glueAddMarkerArtisan)
		;

	MWLua::class_<XUIBuffViewer>(pLua->GetState(), MINT_BUFFVIEWER)
		.inh<MWidget>()
		.def( "Add",					&XUIBuffViewer::glueAdd)
		.def( "Remove",					&XUIBuffViewer::glueRemove)
		.def( "Clear",					&XUIBuffViewer::glueClear)
		;

	MWLua::class_<XUIPresentationBox>(pLua->GetState(), MINT_PRESENTATIONBOX)
		.inh<MWidget>()
		.def( "Add",					&XUIPresentationBox::glueAdd)
		.def( "AddAndSetLifeTime",		&XUIPresentationBox::glueAddAndSetLifeTime)
		.def( "Clear",					&XUIPresentationBox::glueClear)
		;

	MWLua::class_<XUIGauge>(pLua->GetState(), MINT_GAUGE)
		.inh<MWidget>()
		.def( "SetRange",				&XUIGauge::glueSetRange)	
		.def( "GetRange",				&XUIGauge::glueGetRange)	
		.def( "SetPos",					&XUIGauge::glueSetPos)	
		.def( "GetPos",					&XUIGauge::glueGetPos)	
		.def( "EnableSliding",			&XUIGauge::glueEnableSliding)
		;

	MWLua::class_<XUITray>(pLua->GetState(), MINT_TRAY)
		.inh<MWidget>()
		.def( "BindScriptInstance",		&XUITray::glueBindScriptInstance)	
		.def( "GetScriptInstance",		&XUITray::glueGetScriptInstance)	
		.def( "SetHotKey",				&XUITray::glueSetHotKey)	
		.def( "GetHotKey",				&XUITray::glueGetHotKey)	
		;

	MWLua::class_<XUIVideo>(pLua->GetState(), MINT_VIDEO)
		.inh<MWidget>()
		.def( "Open",					&XUIVideo::glueOpen)
		;
}

void XMintScriptDeclarer::DeclWidget(MWLua* pLua, const char* szClass, const char* szName, MWidget* pWidget)
{
	if ( !strcmp( szClass, MINT_EQUIPPEDITEMSLOT))			pLua->SetVar( szName, (XUIEquippedItemSlot*)pWidget);
	else if ( !strcmp( szClass, MINT_PALETTESLOT))			pLua->SetVar( szName, (XUIPaletteSlot*)pWidget);
	else if ( !strcmp( szClass, MINT_TALENTSLOT))			pLua->SetVar( szName, (XUITalentSlot*)pWidget);
	else if ( !strcmp( szClass, MINT_CHATTINGBOX))			pLua->SetVar( szName, (XUIChattingBox*)pWidget);
	else if ( !strcmp( szClass, MINT_JOURNALINDICATOR))		pLua->SetVar( szName, (XUIJournalIndicator*)pWidget);
	else if ( !strcmp( szClass, MINT_FADEBOX))				pLua->SetVar( szName, (XUIFadeBox*)pWidget);
	else if ( !strcmp( szClass, MINT_MINIMAP))				pLua->SetVar( szName, (XUIMiniMap*)pWidget);
	else if ( !strcmp( szClass, MINT_FIELDMAP))				pLua->SetVar( szName, (XUIFieldMap*)pWidget);
	else if ( !strcmp( szClass, MINT_BUFFVIEWER))			pLua->SetVar( szName, (XUIBuffViewer*)pWidget);
	else if ( !strcmp( szClass, MINT_PRESENTATIONBOX))		pLua->SetVar( szName, (XUIPresentationBox*)pWidget);
	else if ( !strcmp( szClass, MINT_GAUGE))				pLua->SetVar( szName, (XUIGauge*)pWidget);
	else if ( !strcmp( szClass, MINT_TRAY))					pLua->SetVar( szName, (XUITray*)pWidget);
	else if ( !strcmp( szClass, MINT_VIDEO))				pLua->SetVar( szName, (XUIVideo*)pWidget);
}
