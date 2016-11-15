#include "stdafx.h"

#include "MScriptManager.h"
#include "MWidget.h"
#include "MSheet.h"
#include "MLayer.h"
#include "MButton.h"
#include "MFrame.h"
#include "MEdit.h"
#include "MEditCtrl.h"
#include "MLabel.h"
#include "MLabelEx.h"
#include "MListBox.h"
#include "MListCtrl.h"
#include "MPanel.h"
#include "MPaint.h"
#include "MViewCtrl.h"
#include "MTextView.h"
#include "MProgressCtrl.h"
#include "MTreeCtrl.h"
#include "MCategoryCtrl.h"
#include "MSlider.h"
#include "MWLua.h"
#include "MEventHandler.h"
#include "MTabCtrl.h"
#include "MPicture.h"
#include "MComboBox.h"
#include "MSlideShow.h"
#include "MSelector.h"
#include "MGroup.h"
#include "MintProfiler.h"


namespace mint3 {

#define LUA_EVENTTABLE_NAME			"MintEvent"


MScriptManager::MScriptManager()
{
	m_bEnable = true;
}

MScriptManager::~MScriptManager()
{

}

bool MScriptManager::Initialize(MWLua* pLua)
{
	bool ret = true;
	m_pLua = pLua;

	if (m_pLua != NULL)
	{
		ret=InitDeclWidgets();
		InitDeclEvents();

		// 이벤트 함수를 담아둘 테이블 초기화
		string strScript = string(LUA_EVENTTABLE_NAME) + " = { }";
		m_pLua->RunString(strScript, WLUA_DEBUG_ARG);
	}

	return ret;
}


#define SET_VARIABLE(_name,_reference)		{ m_pLua->SetNull(_name);	m_pLua->SetVar(_name, _reference);	}


bool MScriptManager::InitDeclWidgets()
{
	// Widget 
	MWLua::class_<MWidget>(m_pLua->GetState(), "Widget")
		.def( "GetParent",				&MWidget::glueGetParent)
		.def( "GetName",				&MWidget::glueGetName)
		.def( "SetText",				&MWidget::glueSetText)
		.def( "GetText",				&MWidget::glueGetText)
		.def( "SetToolTip",				&MWidget::glueSetToolTip)
		.def( "GetToolTip",				&MWidget::glueGetToolTip)
		.def( "SetPosition",			&MWidget::glueSetPosition)
		.def( "GetPosition",			&MWidget::glueGetPosition)
		.def( "SetSize",				&MWidget::glueSetSize)
		.def( "GetSize",				&MWidget::glueGetSize)
		.def( "GetWidth",				&MWidget::glueGetWidth)
		.def( "GetHeight",				&MWidget::glueGetHeight)
		.def( "SetRect",				&MWidget::glueSetRect)
		.def( "GetRect",				&MWidget::glueGetRect)
		.def( "GetClientRect",			&MWidget::glueGetClientRect)
		.def( "GetWindowRect",			&MWidget::glueGetWindowRect)
		.def( "Show",					&MWidget::glueShow)
		.def( "GetShow",				&MWidget::glueGetShow)
		.def( "IsVisibled",				&MWidget::glueIsVisibled)
		.def( "Enable",					&MWidget::glueEnable)
		.def( "GetEnable",				&MWidget::glueGetEnable)
		.def( "IsEnabled",				&MWidget::glueIsEnabled)
		.def( "SetOpacity",				&MWidget::glueSetOpacity)
		.def( "GetOpacity",				&MWidget::glueGetOpacity)
		.def( "ShowBorder",				&MWidget::glueShowBorder)
		.def( "IsShowBorder",			&MWidget::glueIsShowBorder)
		.def( "SetScrollValue",			&MWidget::glueSetScrollValue)
		.def( "GetScrollValue",			&MWidget::glueGetScrollValue)
		.def( "SetScrollThumbLen",		&MWidget::glueSetScrollThumbLen)
		.def( "SetFocus",				&MWidget::glueSetFocus)
		.def( "SetStatic",				&MWidget::glueSetStatic)
		.def( "IsStatic",				&MWidget::glueIsStatic)
		.def( "IsFocus",				&MWidget::glueIsFocus)
		.def( "IsCapture",				&MWidget::glueIsCapture)
		.def( "IsMouseOver",			&MWidget::glueIsMouseOver)
		.def( "IsMouseIncluded",		&MWidget::glueIsMouseIncluded)
		.def( "BringToTop",				&MWidget::glueBringToTop)
		.def( "TrackPopupMenu",			&MWidget::glueTrackPopupMenu)
		.def( "DoModal",				&MWidget::glueDoModal)
		.def( "SetTimer",				&MWidget::glueSetTimer)
		.def( "KillTimer",				&MWidget::glueKillTimer)
		.def( "SendUserArgument",		&MWidget::glueSendUserArgument)
		.def( "AttachDragItem",			&MWidget::glueAttachDragItem)
		.def( "ModifyToolTip",			&MWidget::glueModifyToolTip)
		;


	// Sheet
	MWLua::class_<MSheet>(m_pLua->GetState(), "Sheet")
		.inh<MWidget>()
		;


	// Layer
	MWLua::class_<MLayer>(m_pLua->GetState(), "Layer")
		.inh<MWidget>()
		;


	// Label
	MWLua::class_<MLabel>(m_pLua->GetState(), "Label")
		.inh<MWidget>()
		;


	// LabelEx
	MWLua::class_<MLabelEx>(m_pLua->GetState(), "LabelEx")
		.inh<MWidget>()
		.def( "SetFont",				&MLabelEx::glueSetFont)
		.def( "SetColor",				&MLabelEx::glueSetColor)
		.def( "SetAlign",				&MLabelEx::glueSetAlign)
		;


	// Frame
	MWLua::class_<MFrame>(m_pLua->GetState(), "Frame")
		.inh<MWidget>()
		.def( "ShowTitlebar",			&MFrame::glueShowTitlebar)
		.def( "IsShowTitlebar",			&MFrame::glueIsShowTitlebar)
		.def( "SetIcon",				&MFrame::glueSetIcon)
		.def( "GetIcon",				&MFrame::glueGetIcon)
		;

	
	// Button
	MWLua::class_<MButton>(m_pLua->GetState(), "Button")
		.inh<MWidget>()
		.def( "SetCheck",				&MButton::glueSetCheck)
		.def( "GetCheck",				&MButton::glueGetCheck)
		.def( "SetHotKey",				&MButton::glueSetHotKey)
		.def( "GetHotKey",				&MButton::glueGetHotKey)
		.def( "SetIcon",				&MButton::glueSetIcon)
		.def( "GetIcon",				&MButton::glueGetIcon)
		.def( "SetIconSize",			&MButton::glueSetIconSize)
		.def( "GetIconSize",			&MButton::glueGetIconSize)
		.def( "SetUserData",			&MButton::glueSetUserData)
		.def( "GetUserData",			&MButton::glueGetUserData)
		;


	// Edit
	MWLua::class_<MEdit>(m_pLua->GetState(), "Edit")
		.inh<MWidget>()
		.def( "SetText",				&MEdit::glueSetText)
		.def( "GetText",				&MEdit::glueGetText)
		.def( "GetLength",				&MEdit::glueGetLength)
		.def( "SetCaretPos",			&MEdit::glueSetCaretPos)
		.def( "GetCaretPos",			&MEdit::glueGetCaretPos)
		.def( "SetSel",					&MEdit::glueSetSel)
		.def( "GetSel",					&MEdit::glueGetSel)
		.def( "IsSelected",				&MEdit::glueIsSelected)
		.def( "ReleaseSelect",			&MEdit::glueReleaseSelect)
		.def( "Insert",					&MEdit::glueInsert)
		.def( "ReplaceSel",				&MEdit::glueReplaceSel)
		.def( "Clear",					&MEdit::glueClear)
		.def( "SetEditable",			&MEdit::glueSetEditable)
		.def( "SetNumber",				&MEdit::glueSetNumber)
		.def( "SetLowerCase",			&MEdit::glueSetLowerCase)
		.def( "SetUpperCase",			&MEdit::glueSetUpperCase)
		.def( "SetPassword",			&MEdit::glueSetPassword)
		.def( "SetPasswordChar",		&MEdit::glueSetPasswordChar)
		.def( "glueSetLimitText",		&MEdit::glueSetLimitText)
		.def( "glueGetLimitText",		&MEdit::glueGetLimitText)
		.def( "PushHistory",			&MEdit::gluePushHistory)
		;


	// EditCtrl
	MWLua::class_<MEditCtrl>(m_pLua->GetState(), "EditCtrl")
		.inh<MWidget>()
		.def( "SetText",				&MEditCtrl::glueSetText)
		.def( "GetText",				&MEditCtrl::glueGetText)
		.def( "GetLength",				&MEditCtrl::glueGetLength)
		.def( "SetCaretPos",			&MEditCtrl::glueSetCaretPos)
		.def( "GetCaretPos",			&MEditCtrl::glueGetCaretPos)
		.def( "SetSel",					&MEditCtrl::glueSetSel)
		.def( "GetSel",					&MEditCtrl::glueGetSel)
		.def( "IsSelected",				&MEditCtrl::glueIsSelected)
		.def( "ReleaseSelect",			&MEditCtrl::glueReleaseSelect)
		.def( "Insert",					&MEditCtrl::glueInsert)
		.def( "ReplaceSel",				&MEditCtrl::glueReplaceSel)
		.def( "Clear",					&MEditCtrl::glueClear)
		.def( "SetEditable",			&MEditCtrl::glueSetEditable)
		;


	// ComboBox
	MWLua::class_<MComboBox>(m_pLua->GetState(), "ComboBox")
		.inh<MWidget>()
		.def( "AddString",				&MComboBox::glueAddString)
		.def( "DeleteString",			&MComboBox::glueDeleteString)
		.def( "InsertString",			&MComboBox::glueInsertString)
		.def( "ResetContent",			&MComboBox::glueResetContent)
		.def( "GetItemCount",			&MComboBox::glueGetItemCount)
		.def( "SetItemText",			&MComboBox::glueSetItemText)
		.def( "GetItemText",			&MComboBox::glueGetItemText)
		.def( "SetItemData",			&MComboBox::glueSetItemData)
		.def( "GetItemData",			&MComboBox::glueGetItemData)
		.def( "SetCurSel",				&MComboBox::glueSetCurSel)
		.def( "GetCurSel",				&MComboBox::glueGetCurSel)
		.def( "SetDroppedWidth",		&MComboBox::glueSetDroppedWidth)
		;


	// ListBox
	MWLua::class_<MListBox>(m_pLua->GetState(), "ListBox")
		.inh<MWidget>()
		.def( "AddString",				&MListBox::glueAddString)
		.def( "InsertString",			&MListBox::glueInsertString)
		.def( "DeleteString",			&MListBox::glueDeleteString)
		.def( "ResetContent",			&MListBox::glueResetContent)
		.def( "GetItemCount",			&MListBox::glueGetItemCount)
		.def( "SetItemText",			&MListBox::glueSetItemText)
		.def( "GetItemText",			&MListBox::glueGetItemText)
		.def( "SetItemData",			&MListBox::glueSetItemData)
		.def( "GetItemData",			&MListBox::glueGetItemData)
		.def( "SetCurSel",				&MListBox::glueSetCurSel)
		.def( "GetCurSel",				&MListBox::glueGetCurSel)
		.def( "SetSel",					&MListBox::glueSetSel)
		.def( "GetSel",					&MListBox::glueGetSel)
		.def( "GetSelCount",			&MListBox::glueGetSelCount)
		.def( "GetSelItem",				&MListBox::glueGetSelItem)
		.def( "ReleaseSelect",			&MListBox::glueReleaseSelect)
		.def( "SelItemRange",			&MListBox::glueSelItemRange)
		.def( "SetCaretPos",			&MListBox::glueSetCaretPos)
		.def( "GetCaretPos",			&MListBox::glueGetCaretPos)
		.def( "GetMouseOver",			&MListBox::glueGetMouseOver)
		.def( "EnableMultipleSel",		&MListBox::glueEnableMultipleSel)
		;


	// ListCtrl
	MWLua::class_<MListCtrl>(m_pLua->GetState(), "ListCtrl")
		.inh<MWidget>()
		.def( "AddItem",				&MListCtrl::glueAddItem)
		.def( "InsertItem",				&MListCtrl::glueInsertItem)
		.def( "DeleteItem",				&MListCtrl::glueDeleteItem)
		.def( "DeleteAllItems",			&MListCtrl::glueDeleteAllItems)
		.def( "GetItemCount",			&MListCtrl::glueGetItemCount)
		.def( "SetItemEnable",			&MListCtrl::glueSetItemEnable)
		.def( "GetItemEnable",			&MListCtrl::glueGetItemEnable)
		.def( "SetItemText",			&MListCtrl::glueSetItemText)
		.def( "GetItemText",			&MListCtrl::glueGetItemText)
		.def( "SetItemImage",			&MListCtrl::glueSetItemImage)
		.def( "SetItemColor",			&MListCtrl::glueSetItemColor)
		.def( "SetItemData",			&MListCtrl::glueSetItemData)
		.def( "GetItemData",			&MListCtrl::glueGetItemData)
		.def( "SetColumnText",			&MListCtrl::glueSetColumnText)
		.def( "GetColumnText",			&MListCtrl::glueGetColumnText)
		.def( "SetColumnAlign",			&MListCtrl::glueSetColumnAlign)
		.def( "GetColumnAlign",			&MListCtrl::glueGetColumnAlign)
		.def( "SetColumnWidth",			&MListCtrl::glueSetColumnWidth)
		.def( "GetColumnWidth",			&MListCtrl::glueGetColumnWidth)
		.def( "SetViewStyle",			&MListCtrl::glueSetViewStyle)
		.def( "SetCurSel",				&MListCtrl::glueSetCurSel)
		.def( "GetCurSel",				&MListCtrl::glueGetCurSel)
		.def( "SetSel",					&MListCtrl::glueSetSel)
		.def( "GetSel",					&MListCtrl::glueGetSel)
		.def( "GetSelCount",			&MListCtrl::glueGetSelCount)
		.def( "GetSelItem",				&MListCtrl::glueGetSelItem)
		.def( "ReleaseSelect",			&MListCtrl::glueReleaseSelect)
		.def( "SelItemRange",			&MListCtrl::glueSelItemRange)
		.def( "SetCaretPos",			&MListCtrl::glueSetCaretPos)
		.def( "GetCaretPos",			&MListCtrl::glueGetCaretPos)
		.def( "GetMouseOver",			&MListCtrl::glueGetMouseOver)
		.def( "SetAlwaysSel",			&MListCtrl::glueSetAlwaysSel)
		.def( "GetAlwaysSel",			&MListCtrl::glueGetAlwaysSel)
		.def( "SetItemMargin",			&MListCtrl::glueSetItemMargin)
		.def( "GetItemMargin",			&MListCtrl::glueGetItemMargin)
		.def( "ShowColumnHeader",		&MListCtrl::glueShowColumnHeader)
		.def( "EnableMultipleSel",		&MListCtrl::glueEnableMultipleSel)
		;


	// TreeCtrl
	MWLua::class_<MTreeCtrl>(m_pLua->GetState(), "TreeCtrl")
		.inh<MWidget>()
		.def( "InsertItem",				&MTreeCtrl::glueInsertItem)
		.def( "InsertItemEx",			&MTreeCtrl::glueInsertItemEx)
 		.def( "DeleteItem",				&MTreeCtrl::glueDeleteItem)
 		.def( "DeleteAllItems",			&MTreeCtrl::glueDeleteAllItems)
 		.def( "GetItemCount",			&MTreeCtrl::glueGetItemCount)
 		.def( "GetVisibleCount",		&MTreeCtrl::glueGetVisibleCount)
 		.def( "SetItemText",			&MTreeCtrl::glueSetItemText)
 		.def( "GetItemText",			&MTreeCtrl::glueGetItemText)
 		.def( "SetItemImage",			&MTreeCtrl::glueSetItemImage)
 		.def( "GetItemImage",			&MTreeCtrl::glueGetItemImage)
 		.def( "SetItemData",			&MTreeCtrl::glueSetItemData)
 		.def( "GetItemData",			&MTreeCtrl::glueGetItemData)
 		.def( "SetItemHeight",			&MTreeCtrl::glueSetItemHeight)
 		.def( "GetItemHeight",			&MTreeCtrl::glueGetItemHeight)
 		.def( "SetItemFont",			&MTreeCtrl::glueSetItemFont)
 		.def( "SetItemColor",			&MTreeCtrl::glueSetItemColor)
 		.def( "Expand",					&MTreeCtrl::glueExpand)
 		.def( "GetRootItem",			&MTreeCtrl::glueGetRootItem)
 		.def( "GetParentItem",			&MTreeCtrl::glueGetParentItem)
 		.def( "ItemHasChildren",		&MTreeCtrl::glueItemHasChildren)
 		.def( "ShowRootItem",			&MTreeCtrl::glueShowRootItem)
 		.def( "IsShowRootItem",			&MTreeCtrl::glueIsShowRootItem)
 		.def( "GetPrevSiblingItem",		&MTreeCtrl::glueGetPrevSiblingItem)
 		.def( "GetNextSiblingItem",		&MTreeCtrl::glueGetNextSiblingItem)
 		.def( "GetFirstVisibleItem",	&MTreeCtrl::glueGetFirstVisibleItem)
 		.def( "GetLastVisibleItem",		&MTreeCtrl::glueGetLastVisibleItem)
 		.def( "GetPrevVisibleItem",		&MTreeCtrl::glueGetPrevVisibleItem)
 		.def( "GetNextVisibleItem",		&MTreeCtrl::glueGetNextVisibleItem)
 		.def( "SetCurSel",				&MTreeCtrl::glueSetCurSel)
 		.def( "GetCurSel",				&MTreeCtrl::glueGetCurSel)
 		.def( "SetSel",					&MTreeCtrl::glueSetSel)
 		.def( "GetSel",					&MTreeCtrl::glueGetSel)
 		.def( "GetSelCount",			&MTreeCtrl::glueGetSelCount)
 		.def( "GetSelItem",				&MTreeCtrl::glueGetSelItem)
 		.def( "ReleaseSelect",			&MTreeCtrl::glueReleaseSelect)
 		.def( "SetCaretPos",			&MTreeCtrl::glueSetCaretPos)
 		.def( "GetCaretPos",			&MTreeCtrl::glueGetCaretPos)
		.def( "GetMouseOver",			&MTreeCtrl::glueGetMouseOver)
 		.def( "EnableMultipleSel",		&MTreeCtrl::glueEnableMultipleSel)
		;


	// CategoryCtrl
	MWLua::class_<MCategoryCtrl>(m_pLua->GetState(), "CategoryCtrl")
		.inh<MWidget>()
		.def( "AddCategory",			&MCategoryCtrl::glueAddCategory)
		.def( "InsertCategory",			&MCategoryCtrl::glueInsertCategory)
		.def( "DeleteCategory",			&MCategoryCtrl::glueDeleteCategory)
		.def( "AddItem",				&MCategoryCtrl::glueAddItem)
		.def( "InsertItem",				&MCategoryCtrl::glueInsertItem)
		.def( "DeleteItem",				&MCategoryCtrl::glueDeleteItem)
		.def( "DeleteAllItems",			&MCategoryCtrl::glueDeleteAllItems)
		.def( "GetCategoryCount",		&MCategoryCtrl::glueGetCategoryCount)
		.def( "SetCategoryText",		&MCategoryCtrl::glueSetCategoryText)
		.def( "GetCategoryText",		&MCategoryCtrl::glueGetCategoryText)
		.def( "SetCategoryExpand",		&MCategoryCtrl::glueSetCategoryExpand)
		.def( "GetCategoryExpand",		&MCategoryCtrl::glueGetCategoryExpand)
		.def( "GetItemCount",			&MCategoryCtrl::glueGetItemCount)
		.def( "SetItemEnable",			&MCategoryCtrl::glueSetItemEnable)
		.def( "GetItemEnable",			&MCategoryCtrl::glueGetItemEnable)
		.def( "SetItemText",			&MCategoryCtrl::glueSetItemText)
		.def( "GetItemText",			&MCategoryCtrl::glueGetItemText)
		.def( "SetItemImage",			&MCategoryCtrl::glueSetItemImage)
		.def( "SetItemColor",			&MCategoryCtrl::glueSetItemColor)
		.def( "SetItemData",			&MCategoryCtrl::glueSetItemData)
		.def( "GetItemData",			&MCategoryCtrl::glueGetItemData)
		.def( "SetCurSel",				&MCategoryCtrl::glueSetCurSel)
		.def( "GetCurSel",				&MCategoryCtrl::glueGetCurSel)
		.def( "SetSel",					&MCategoryCtrl::glueSetSel)
		.def( "GetSel",					&MCategoryCtrl::glueGetSel)
		.def( "GetSelCount",			&MCategoryCtrl::glueGetSelCount)
		.def( "GetSelItem",				&MCategoryCtrl::glueGetSelItem)
		.def( "ReleaseSelect",			&MCategoryCtrl::glueReleaseSelect)
		.def( "SetCaretPos",			&MCategoryCtrl::glueSetCaretPos)
		.def( "GetCaretPos",			&MCategoryCtrl::glueGetCaretPos)
		.def( "GetMouseOver",			&MCategoryCtrl::glueGetMouseOver)
		.def( "EnableMultipleSel",		&MCategoryCtrl::glueEnableMultipleSel)
		;


	// Group
	MWLua::class_<MGroup>(m_pLua->GetState(), "Group")
		.inh<MWidget>()
		;


	// TextView
	MWLua::class_<MTextView>(m_pLua->GetState(), "TextView")
		.inh<MWidget>()
		.def( "Clear",					&MTextView::glueClear)
		.def( "GetRefCount",			&MTextView::glueGetRefCount)
		.def( "GetRefText",				&MTextView::glueGetRefText)
		.def( "GetRefToolTip",			&MTextView::glueGetRefToolTip)
		.def( "GetRefRect",				&MTextView::glueGetRefRect)
		.def( "SetCaretPos",			&MTextView::glueSetCaretPos)
		.def( "GetCaretPos",			&MTextView::glueGetCaretPos)
		.def( "GetMouseOver",			&MTextView::glueGetMouseOver)
		.def( "ScrollPage",				&MTextView::glueScrollPage)
		.def( "GetPageSize",			&MTextView::glueGetPageSize)
		;


	// ProgressCtrl
	MWLua::class_<MProgressCtrl>(m_pLua->GetState(), "ProgressCtrl")
		.inh<MWidget>()
		.def( "SetRange",				&MProgressCtrl::glueSetRange)	
		.def( "GetRange",				&MProgressCtrl::glueGetRange)	
		.def( "SetPos",					&MProgressCtrl::glueSetPos)	
		.def( "GetPos",					&MProgressCtrl::glueGetPos)	
		.def( "EnableSliding",			&MProgressCtrl::glueEnableSliding)	
		;


	// TabCtrl
	MWLua::class_<MTabCtrl>(m_pLua->GetState(), "TabCtrl")
		.inh<MWidget>()
		.def( "GetCount",				&MTabCtrl::glueGetCount)
		.def( "SetSelIndex",			&MTabCtrl::glueSetSelIndex)
		.def( "GetSelIndex",			&MTabCtrl::glueGetSelIndex)
		.def( "SetSelTabName",			&MTabCtrl::glueSetSelTabName)
		.def( "SetTabName",				&MTabCtrl::glueSetTabName)
		;


	// Scroll
	MWLua::class_<MScrollBar>(m_pLua->GetState(), "ScrollBar")
		.inh<MWidget>()
		.def( "SetScrollRange",			&MScrollBar::glueSetScrollRange)
		.def( "GetScrollRange",			&MScrollBar::glueGetScrollRange)
		.def( "SetScrollValue",			&MScrollBar::glueSetScrollValue)
		.def( "GetScrollValue",			&MScrollBar::glueGetScrollValue)
		.def( "SetThumbLength",			&MScrollBar::glueSetThumbLength)
		.def( "GetThumbLength",			&MScrollBar::glueGetThumbLength)
		;

	
	// Slider
	MWLua::class_<MSlider>(m_pLua->GetState(), "Slider")
		.inh<MWidget>()
		.def( "SetScrollRange",			&MScrollBar::glueSetScrollRange)
		.def( "GetScrollRange",			&MScrollBar::glueGetScrollRange)
		.def( "SetScrollValue",			&MScrollBar::glueSetScrollValue)
		.def( "GetScrollValue",			&MScrollBar::glueGetScrollValue)
		.def( "SetThumbLength",			&MScrollBar::glueSetThumbLength)
		.def( "GetThumbLength",			&MScrollBar::glueGetThumbLength)
		;


	// Picture
	MWLua::class_<MPicture>(m_pLua->GetState(), "Picture")
		.inh<MWidget>()
		.def( "SetImage",				&MPicture::glueSetImage)
		.def( "GetImage",				&MPicture::glueGetImage)
		.def( "GetImageWidth",			&MPicture::glueGetImageWidth)
		.def( "GetImageHeight",			&MPicture::glueGetImageHeight)
		.def( "SetStretch",				&MPicture::glueSetStretch)
		.def( "GetStretch",				&MPicture::glueGetStretch)
		.def( "SetEffect",				&MPicture::glueSetEffect)
		.def( "GetEffect",				&MPicture::glueGetEffect)
		.def( "SetColorize",			&MPicture::glueSetColorize)
		.def( "GetColorize",			&MPicture::glueGetColorize)
		.def( "SetFlipHorizontal",		&MPicture::glueSetFlipHorizontal)
		.def( "SetFlipVertical",		&MPicture::glueSetFlipVertical)
		.def( "SetRotate90CW",			&MPicture::glueSetRotate90CW)
		.def( "SetRotate90CCW",			&MPicture::glueSetRotate90CCW)
		.def( "SetRotate180",			&MPicture::glueSetRotate180)
		.def( "ResetRotate",			&MPicture::glueResetRotate)
		;


	// Panel
	MWLua::class_<MPanel>(m_pLua->GetState(), "Panel")
		.inh<MWidget>()
		;


	// ViewCtrl
	MWLua::class_<MViewCtrl>(m_pLua->GetState(), "ViewCtrl")
		.inh<MWidget>()
		.def( "SetScrollValue",			&MViewCtrl::glueSetScrollValue)
		.def( "GetScrollValue",			&MViewCtrl::glueGetScrollValue)
		.def( "SetScrollRange",			&MViewCtrl::glueSetScrollRange)
		.def( "GetScrollRange",			&MViewCtrl::glueGetScrollRange)
		.def( "GetScrollBarRange",		&MViewCtrl::glueGetScrollBarRange)
		;


	// Paint
	MWLua::class_<MPaint>(m_pLua->GetState(), "Paint")
		.inh<MWidget>()
		.def( "SetColor",				&MPaint::glueSetColor)
		.def( "SetColorEx",				&MPaint::glueSetColorEx)
		.def( "GetColor",				&MPaint::glueGetColor)
		;


	// Selector
	MWLua::class_<MSelector>(m_pLua->GetState(), "Selector")
		.inh<MWidget>()
		.def( "AddItem",				&MSelector::glueAddItem)
		.def( "InsertItem",				&MSelector::glueInsertItem)
		.def( "DeleteItem",				&MSelector::glueDeleteItem)
		.def( "ResetContent",			&MSelector::glueResetContent)
		.def( "GetItemCount",			&MSelector::glueGetItemCount)
		.def( "SetStyle",				&MSelector::glueSetStyle)
		.def( "GetStyle",				&MSelector::glueGetStyle)
		.def( "SetCurSel",				&MSelector::glueSetCurSel)
		.def( "GetCurSel",				&MSelector::glueGetCurSel)
		.def( "GetItemFromValue",		&MSelector::GetItemFromValue)
		.def( "SetItemString",			&MSelector::glueSetItemString)
		.def( "GetItemString",			&MSelector::glueGetItemString)
		.def( "SetItemValue",			&MSelector::glueSetItemValue)
		.def( "GetItemValue",			&MSelector::glueGetItemValue)
		;


	// PopupMenu
	MWLua::class_<MPopupMenu>(m_pLua->GetState(), "PopupMenu")
		.inh<MWidget>()
		;


	// SlideShow
	MWLua::class_<MSlideShow>(m_pLua->GetState(), "SlideShow")
		.inh<MWidget>()
		.def( "SetImage",				&MSlideShow::glueSetImage)
		.def( "GetImage",				&MSlideShow::glueGetImage)
		.def( "GetSceneSize",			&MSlideShow::glueGetSceneSize)
		.def( "SliceScene",				&MSlideShow::glueSliceScene)
		.def( "SetSceneCount",			&MSlideShow::glueSetSceneCount)
		.def( "GetSceneCount",			&MSlideShow::glueGetSceneCount)
		.def( "SetCurScene",			&MSlideShow::glueSetCurScene)
		.def( "GetCurScene",			&MSlideShow::glueGetCurScene)
		.def( "SetSlideShow",			&MSlideShow::glueSetSlideShow)
		.def( "IsSlideShow",			&MSlideShow::glueIsSlideShow)
		.def( "SetDelayTime",			&MSlideShow::glueSetDelayTime)
		.def( "GetDelayTime",			&MSlideShow::glueGetDelayTime)
		.def( "SetFadeTime",			&MSlideShow::glueSetFadeTime)
		.def( "GetFadeTime",			&MSlideShow::glueGetFadeTime)
		.def( "SetPlayOnce",			&MSlideShow::glueSetPlayOnce)
		.def( "IsPlayOnce",				&MSlideShow::glueIsPlayOnce)
		.def( "SetEffect",				&MSlideShow::glueSetEffect)
		.def( "GetEffect",				&MSlideShow::glueGetEffect)
		.def( "SetColorize",			&MSlideShow::glueSetColorize)
		.def( "GetColorize",			&MSlideShow::glueGetColorize)
		.def( "SetFlipHorizontal",		&MSlideShow::glueSetFlipHorizontal)
		.def( "SetFlipVertical",		&MSlideShow::glueSetFlipVertical)
		.def( "SetRotate90CW",			&MSlideShow::glueSetRotate90CW)
		.def( "SetRotate90CCW",			&MSlideShow::glueSetRotate90CCW)
		.def( "SetRotate180",			&MSlideShow::glueSetRotate180)
		.def( "ResetRotate",			&MSlideShow::glueResetRotate)
		;
	

	// Helper functions
	MWLua::class_<MScriptHelper>(m_pLua->GetState(), "MScriptHelper")
		.def( "Line",					&MScriptHelper::Line)
		.def( "Rect",					&MScriptHelper::Rect)
		.def( "FillRect",				&MScriptHelper::FillRect)
		.def( "Draw",					&MScriptHelper::Draw)
		.def( "DrawEx",					&MScriptHelper::DrawEx)
		.def( "Text",					&MScriptHelper::Text)
		.def( "TextEx",					&MScriptHelper::TextEx)
		.def( "SetColor",				&MScriptHelper::SetColor)
		.def( "SetColorEx",				&MScriptHelper::SetColorEx)
		.def( "GetColor",				&MScriptHelper::GetColor)
		.def( "SetBitmapColor",			&MScriptHelper::SetBitmapColor)
		.def( "SetBitmapColorEx",		&MScriptHelper::SetBitmapColorEx)
		.def( "GetBitmapColor",			&MScriptHelper::GetBitmapColor)
		.def( "SetEffect",				&MScriptHelper::SetEffect)
		.def( "GetEffect",				&MScriptHelper::GetEffect)
		.def( "SetOpacity",				&MScriptHelper::SetOpacity)
		.def( "GetOpacity",				&MScriptHelper::GetOpacity)
		.def( "SetBitmap",				&MScriptHelper::SetBitmap)
		.def( "GetBitmap",				&MScriptHelper::GetBitmap)
		.def( "SetFont",				&MScriptHelper::SetFont)
		.def( "SetTextAlign",			&MScriptHelper::SetTextAlign)
		.def( "GetTextWidth",			&MScriptHelper::GetTextWidth)
		.def( "SubTextWidth",			&MScriptHelper::SubTextWidth)
		;
	SET_VARIABLE( "gamedraw", &m_ScriptHelper);


	return true;
}


void MScriptManager::InitDeclEvents()
{
	// MEventArgs
	MWLua::class_<MEventArgs>(m_pLua->GetState(), "MEventArgs")
		.def( "GetOwner",				&MEventArgs::GetOwner)
		.def( "GetCursorPos",			&MEventArgs::GetCursorPos)
		.def( "GetItemIndex",			&MEventArgs::GetItemIndex)
		.def( "GetItemSubItem",			&MEventArgs::GetItemSubItem)
		.def( "SetItemRect",			&MEventArgs::SetItemRect)
		.def( "GetItemRect",			&MEventArgs::GetItemRect)
		.def( "SetUserArgument",		&MEventArgs::SetUserArgument)
		.def( "GetUserArgument",		&MEventArgs::GetUserArgument)
		.def( "SetUserData",			&MEventArgs::SetUserData)
		.def( "GetUserData",			&MEventArgs::GetUserData)
		.def_readwrite( "wheel",		&MEventArgs::m_nWheel)
		.def_readwrite( "key",			&MEventArgs::m_nKey)
		.def_readwrite( "shift",		&MEventArgs::m_bShift)
		.def_readwrite( "ctrl",			&MEventArgs::m_bCtrl)
		.def_readwrite( "hittest",		&MEventArgs::m_nHitTest)
		;
	SET_VARIABLE( "EventArgs", &MWidget::m_EventArgs);


	// MDragEventArgs
	MWLua::class_<MDragEventArgs>(m_pLua->GetState(), "MDragEventArgs")
		.def( "GetSender",				&MDragEventArgs::GetSender)
		.def( "AddDragData",			&MDragEventArgs::AddDragData)
		.def( "SetImageSize",			&MDragEventArgs::SetImageSize)
		.def( "SetFont",				&MDragEventArgs::SetFont)
		.def( "SetColor",				&MDragEventArgs::SetColor)
		.def( "SetHotSpot",				&MDragEventArgs::SetHotSpot)
		.def( "SetOpacity",				&MDragEventArgs::SetOpacity)
		.def( "GetItemCount",			&MDragEventArgs::GetItemCount)
		.def( "IsEmpty",				&MDragEventArgs::IsEmpty)
		.def( "SetItemText",			&MDragEventArgs::SetItemText)
		.def( "GetItemText",			&MDragEventArgs::GetItemText)
		.def( "SetItemImage",			&MDragEventArgs::SetItemImage)
		.def( "GetItemImage",			&MDragEventArgs::GetItemImage)
		.def( "GetItemData",			&MDragEventArgs::GetItemData)
		.def( "IsDropped",				&MDragEventArgs::IsDropped)
		.def( "SetEffect",				&MDragEventArgs::SetEffect)
		;
	SET_VARIABLE( "DragEventArgs", &MWidget::m_DragEventArgs);


	// MPopupEventArgs
	MWLua::class_<MPopupEventArgs>(m_pLua->GetState(), "MPopupEventArgs")
		.def( "GetOwner",				&MPopupEventArgs::GetOwner)
		.def( "GetPoint",				&MPopupEventArgs::GetPoint)
		;
	SET_VARIABLE( "PopupEventArgs", &MWidget::m_PopupEventArgs);
}


void MScriptManager::DeclWidget(const char* szClass, const char* szName, MWidget* pWidget)
{
	if (m_pLua == NULL) return;

	if(!strcmp(szClass, MINT_WIDGET))				SET_VARIABLE( szName, pWidget)
	else if(!strcmp(szClass, MINT_SHEET))			SET_VARIABLE( szName, (MSheet*)pWidget)
	else if(!strcmp(szClass, MINT_LAYER))			SET_VARIABLE( szName, (MLayer*)pWidget)
	else if(!strcmp(szClass, MINT_FRAME))			SET_VARIABLE( szName, (MFrame*)pWidget)
	else if(!strcmp(szClass, MINT_BUTTON))			SET_VARIABLE( szName, (MButton*)pWidget)
	else if(!strcmp(szClass, MINT_EDIT))			SET_VARIABLE( szName, (MEdit*)pWidget)
	else if(!strcmp(szClass, MINT_EDITCTRL))		SET_VARIABLE( szName, (MEditCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_LABEL))			SET_VARIABLE( szName, (MLabel*)pWidget)
	else if(!strcmp(szClass, MINT_LABELEX))			SET_VARIABLE( szName, (MLabelEx*)pWidget)
	else if(!strcmp(szClass, MINT_LISTBOX))			SET_VARIABLE( szName, (MListBox*)pWidget)
	else if(!strcmp(szClass, MINT_LISTCTRL))		SET_VARIABLE( szName, (MListCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_GROUP))			SET_VARIABLE( szName, (MGroup*)pWidget)
	else if(!strcmp(szClass, MINT_PANEL))			SET_VARIABLE( szName, (MPanel*)pWidget)
	else if(!strcmp(szClass, MINT_PAINT))			SET_VARIABLE( szName, (MPaint*)pWidget)
	else if(!strcmp(szClass, MINT_COMBOBOX))		SET_VARIABLE( szName, (MComboBox*)pWidget)
	else if(!strcmp(szClass, MINT_TABCTRL))			SET_VARIABLE( szName, (MTabCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_TREECTRL))		SET_VARIABLE( szName, (MTreeCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_CATEGORYCTRL))	SET_VARIABLE( szName, (MCategoryCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_SCROLLBAR))		SET_VARIABLE( szName, (MScrollBar*)pWidget)
	else if(!strcmp(szClass, MINT_SLIDER))			SET_VARIABLE( szName, (MSlider*)pWidget)
	else if(!strcmp(szClass, MINT_PICTURE))			SET_VARIABLE( szName, (MPicture*)pWidget)
	else if(!strcmp(szClass, MINT_VIEWCTRL))		SET_VARIABLE( szName, (MViewCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_TEXTVIEW))		SET_VARIABLE( szName, (MTextView*)pWidget)
	else if(!strcmp(szClass, MINT_PROGRESSCTRL))	SET_VARIABLE( szName, (MProgressCtrl*)pWidget)
	else if(!strcmp(szClass, MINT_SELECTOR))		SET_VARIABLE( szName, (MSelector*)pWidget)
	else if(!strcmp(szClass, MINT_SLIDESHOW))		SET_VARIABLE( szName, (MSlideShow*)pWidget)
	else if(!strcmp(szClass, MINT_POPUPMENU))		SET_VARIABLE( szName, (MPopupMenu*)pWidget)
}

void MScriptManager::ReleaseWidget(const char* szName)
{
	if (m_pLua == NULL) return;
	m_pLua->SetNull(szName);
}

void MScriptManager::AddScriptEvent(string& strWidgetName, string& strEventName, string& strScript)
{
	if (m_pLua == NULL) return;
	if ((strWidgetName.empty()) || (strScript.empty())) return;

	// 루아로 이벤트 함수 생성
	string strFuncName = string(LUA_EVENTTABLE_NAME) + string(".") + strWidgetName + string("_") + strEventName;
	string strNewScript = strFuncName + string(" = function(self) \n") + strScript + string("\nend");

	m_pLua->RunString(strNewScript, WLUA_DEBUG_ARG);
}

void MScriptManager::RunScriptFile(const char *szFileName)
{
	if (m_pLua == NULL) return;

//	m_pLua->RunFile(szFileName, pFS);
	m_pLua->RunFile(szFileName, WLUA_DEBUG_ARG);
}

void MScriptManager::RunScriptString(const char* szStream)
{
	if (m_pLua == NULL) return;

	m_pLua->RunString(szStream, WLUA_DEBUG_ARG);
}


bool MScriptManager::RunScriptEvent(MWidget* pWidget, const char* szEventName)
{
	// Begin profile
	bool bUpdateProfile = MGetMintProfiler()->IsBeginedUpdateProfile();
	bool bRenderProfile = MGetMintProfiler()->IsBeginedRenderProfile();
	if ( bUpdateProfile)		MGetMintProfiler()->WriteUpdateProfileBegin( szEventName);
	else if ( bRenderProfile)	MGetMintProfiler()->WriteRenderProfileBegin( szEventName);

	if (m_pLua == NULL  ||  pWidget == NULL)	return false;

	string strFuncName = pWidget->GetName() + string("_") + string(szEventName);

	bool bRetVal = false;
	if ( m_bEnable == true)
		m_pLua->MemberCall(LUA_EVENTTABLE_NAME, strFuncName.c_str(), pWidget->GetName().c_str(), WLUA_DEBUG_ARG, &bRetVal);

	// End profile
	if ( bUpdateProfile)		MGetMintProfiler()->WriteUpdateProfileEnd( szEventName);
	else if ( bRenderProfile)	MGetMintProfiler()->WriteRenderProfileEnd( szEventName);
	return bRetVal;
}

bool MScriptManager::RunScriptEvent( const char* szName, const char* szEvent)
{
	// Begin profile
	bool bUpdateProfile = MGetMintProfiler()->IsBeginedUpdateProfile();
	bool bRenderProfile = MGetMintProfiler()->IsBeginedRenderProfile();
	if ( bUpdateProfile)		MGetMintProfiler()->WriteUpdateProfileBegin( szEvent);
	else if ( bRenderProfile)	MGetMintProfiler()->WriteRenderProfileBegin( szEvent);

	if (m_pLua == NULL)		return false;

	string strFuncName = szName + string("_") + string( szEvent);

	bool bRetVal = false;
	if ( m_bEnable == true)
		m_pLua->MemberCall( LUA_EVENTTABLE_NAME, strFuncName.c_str(), szName, WLUA_DEBUG_ARG, &bRetVal);

	// End profile
	if ( bUpdateProfile)		MGetMintProfiler()->WriteUpdateProfileEnd( szEvent);
	else if ( bRenderProfile)	MGetMintProfiler()->WriteRenderProfileEnd( szEvent);
	return bRetVal;
}

} // namespace mint3