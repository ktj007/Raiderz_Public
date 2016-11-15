#include "stdafx.h"

#include "MScriptManager.h"
#include "MWidget.h"
#include "MButton.h"
#include "MFrame.h"
#include "MEdit.h"
#include "MMTimer.h"
#include "MLabel.h"
#include "MListBox.h"
#include "MPanel.h"
#include "MPaint.h"
#include "MTextArea.h"
#include "MLayer.h"
#include "MTextView.h"
#include "MTreeView.h"
//#include "MInput.h"
#include "MWLua.h"
#include "MEventHandler.h"
#include "MTabCtrl.h"
#include "MPicture.h"
#include "MComboBox.h"

namespace mint3 {

#define LUA_EVENTTABLE_NAME			"MintEvent"


MScriptManager::MScriptManager() : m_pLua(NULL)
{

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


bool MScriptManager::InitDeclWidgets()
{
	// helper Funcs
	MWLua::class_<MScriptHelper>(m_pLua->GetState(), "MScriptHelper")
		.def("DrawLine", &MScriptHelper::glueDrawLine)
		.def("DrawBox", &MScriptHelper::glueDrawBox)
		.def("FillBox", &MScriptHelper::glueFillBox)
		.def_readwrite("color_a", &MScriptHelper::m_a)
		.def_readwrite("color_r", &MScriptHelper::m_r)
		.def_readwrite("color_g", &MScriptHelper::m_g)
		.def_readwrite("color_b", &MScriptHelper::m_b)
		;
//	decl("mint", &m_ScriptHelper);
	m_pLua->SetVar("mint", &m_ScriptHelper);
	

	// widget 
	MWLua::class_<MWidget>(m_pLua->GetState(), "Widget")
		.def("Show", &MWidget::glueShow)
		.def("Hide", &MWidget::glueHide)
		.def("ShowModal", &MWidget::glueShowModal)
		.def("SetSize", &MWidget::glueSetSize)
		.def("SetOpacity", &MWidget::glueSetOpacity)
		.def("SetText", &MWidget::glueSetText)
		.def("SetEnabled", &MWidget::glueSetEnabled)
		.def("IsEnabled", &MWidget::glueIsEnabled)
		.def("IsVisible", &MWidget::glueIsVisible)
		.def("GetName", &MWidget::glueGetName)
		.def("GetText", &MWidget::glueGetText)
		.def("GetWidth", &MWidget::glueGetWidth)
		.def("GetHeight", &MWidget::glueGetHeight)
		.def("GetParent", &MWidget::glueGetParent)
		.def("GetPosition", &MWidget::glueGetPosition)
		.def("SetPosition", &MWidget::glueSetPosition)
		// madduck [5/11/2006]
		.def("SetWidth", &MWidget::glueSetWidth)		
		.def("IsFocus", &MWidget::glueIsFocus)
		.def("SetFocus", &MWidget::glueSetFocus)
		.def("Click",	&MWidget::glueClick)
		.def("GetPositionX", &MWidget::glueGetPositionX)
		.def("GetPositionY", &MWidget::glueGetPositionY)
		.def("BringToTop", &MWidget::glueBringToTop)
		;

	// button
	MWLua::class_<MButton>(m_pLua->GetState(), "Button")
		.inh<MWidget>()
		.def("IsCheck",			&MButton::glueIsCheck)
		.def("IsButtonDown",	&MButton::glueIsButtonDown)		
		.def("SetCheck",		&MButton::glueSetCheck)
		.def("SetConfirmMsg",	&MButton::glueSetConfirmMsg)

		// UserData
		.def("SetUserData",		&MButton::glueSetUserData)
		.def("GetUserData",		&MButton::glueGetUserData)
		;

	// frame
	MWLua::class_<MFrame>(m_pLua->GetState(), "Frame")
		.inh<MWidget>()
		;

	// edit
	MWLua::class_<MEdit>(m_pLua->GetState(), "Edit")
		.inh<MWidget>()
		;

	// timer
	MWLua::class_<MMTimer>(m_pLua->GetState(), "Timer")
		.inh<MWidget>()
		.def("SetInterval",&MMTimer::glueSetInterval)
		.def("GetInterval", &MMTimer::glueGetInterval)
		;

	// label
	MWLua::class_<MLabel>(m_pLua->GetState(), "Label")
		.inh<MWidget>()
		.def("SetTextColor",		&MLabel::glueSetTextColor)
		;

	// listbox
	MWLua::class_<MListBox>(m_pLua->GetState(), "ListBox")
		.inh<MWidget>()
		.def("Add",					&MListBox::glueAdd)
		.def("GetCount",			&MListBox::glueGetCount)
		.def("Remove",				&MListBox::glueRemove)
		.def("RemoveAll",			&MListBox::glueRemoveAll)
		.def("MoveEndLine",			&MListBox::glueMoveEndLine)
		.def("GetSelIndex",			&MListBox::glueGetSelectedIndex)
		.def("SetSelIndex",			&MListBox::glueSetSelIndex)
		
		.def("GetSelText",			&MListBox::glueGetSelectedText)
		.def("NoneSelected",		&MListBox::glueNoneSelected)
		.def("EnableDragAndDrop",	&MListBox::glueEnableDragAndDrop)
		
		.def("AddField",			&MListBox::glueAddField)
		.def("RemoveField",			&MListBox::glueRemoveField)

		.def("AddItem",				&MListBox::glueAddItem)
		.def("SetItemColor",		&MListBox::glueSetItemColor)
		.def("SetItemString",		&MListBox::glueSetItemString)
		.def("SetItemBitmapName",	&MListBox::glueSetItemBitmapName)
		.def("SetItemBitmapIndex",	&MListBox::glueSetItemBitmapIndex)
		.def("SetItemEnable",		&MListBox::glueSetItemEnable)
		.def("FindIndex",			&MListBox::glueFindIndex)
		.def("ShowItem",			&MListBox::glueShowItem)
		.def("GetStartItem",		&MListBox::glueGetStartItem)
		.def("GetItemHeight",		&MListBox::glueGetItemHeight)		

		// UserData
		.def("SetUserData",			&MListBox::glueSetUserData)
		.def("GetSelUserData",		&MListBox::glueGetSelUserData)
		.def("GetIndexUserData",	&MListBox::glueGetIndexUserData)
		;

	// textarea
	MWLua::class_<MTextArea>(m_pLua->GetState(), "TextArea")
		.inh<MWidget>()
		.def("Clear",			&MTextArea::glueClear)
		.def("AddText",			&MTextArea::glueAddText)
		.def("AddColorText",	&MTextArea::glueAddColorText)
		.def("GetMaxHeight",	&MTextArea::glueGetMaxHeight)
		.def("MoveFirst",		&MTextArea::glueMoveFirst)		
		;

	// panel
	MWLua::class_<MPanel>(m_pLua->GetState(), "Panel")
		.inh<MWidget>()
		;

	// paint
	MWLua::class_<MPaint>(m_pLua->GetState(), "Paint")
		.inh<MWidget>()
		;

	// combobox
	MWLua::class_<MComboBox>(m_pLua->GetState(), "ComboBox")
		.inh<MWidget>()
		.def("Add",				&MComboBox::glueAdd)
		.def("Remove",			&MComboBox::glueRemove)
		.def("RemoveAll",		&MComboBox::glueRemoveAll)
		.def("GetCount",		&MComboBox::glueGetCount)
		.def("GetSelText",		&MComboBox::glueGetSelectedText)
		.def("GetSelIndex",		&MComboBox::glueGetSelelectedIndex)
		.def("SetSelIndex",		&MComboBox::glueSetSelectedIndex)
		;

	// TabCtrl
	MWLua::class_<MTabCtrl>(m_pLua->GetState(), "TabCtrl")
		.inh<MWidget>()
		.def("GetCount",		&MTabCtrl::glueGetCount)
		.def("GetSelIndex",		&MTabCtrl::glueGetSelIndex)
		.def("SetSelIndex",		&MTabCtrl::glueSetSelIndex)
		.def("SetSelTabName",	&MTabCtrl::glueSetSelTabName)
		;

	// Scroll
	MWLua::class_<MScrollBar>(m_pLua->GetState(), "ScrollBar")
		.inh<MWidget>()
		.def("GetValue",		&MScrollBar::glueGetValue)
		;

	MWLua::class_<MPicture>(m_pLua->GetState(), "Picture")
		.inh<MWidget>()
		.def("SetBitmap",		&MPicture::glueSetBitmapName)
		;

	// TextView
	MWLua::class_<MTextView>(m_pLua->GetState(), "TextView")
		.inh<MWidget>()
		.def("ScrollPage",			&MTextView::glueScrollPage)
		.def("GetPageWidth",		&MTextView::glueGetPageWidth)
		.def("GetPageHeight",		&MTextView::glueGetPageHeight)
		.def("StartAutoScroll",		&MTextView::glueStartAutoScroll)
		.def("StopAutoScroll",		&MTextView::glueStopAutoScroll)		
		;

	// TreeView
	MWLua::class_<MTreeView>(m_pLua->GetState(), "TreeView")
		.inh<MWidget>()
		;

	return true;
}

void MScriptManager::InitDeclEvents()
{
	// eventargs
	MWLua::class_<MEventArgs>(m_pLua->GetState(), "MEventArgs")
		.def_readwrite("KeyValue",	&MEventArgs::m_nKeyValue)
		.def_readwrite("PosX",		&MEventArgs::m_nMouseX)
		.def_readwrite("PosY",		&MEventArgs::m_nMouseY)
		.def_readwrite("Delta",		&MEventArgs::m_nMouseDelta)
		.def_readwrite("Ctrl",		&MEventArgs::m_bCtrl)
		.def_readwrite("Shift",		&MEventArgs::m_bShift)
		;
	m_pLua->SetVar("EventArgs", &MWidget::m_EventArgs);

	// drageventarg
	MWLua::class_<MDragEventArgs>(m_pLua->GetState(), "MDragEventArgs")
		.def_readwrite("Sender",	&MDragEventArgs::m_pSender)
		.def_readwrite("X",			&MDragEventArgs::m_nX)
		.def_readwrite("Y",			&MDragEventArgs::m_nY)
		.def("GetString",			&MDragEventArgs::GetString)
		.def("GetItemString",		&MDragEventArgs::GetItemString)
		.def_readwrite("Ctrl",		&MDragEventArgs::m_bCtrl)
		;
	m_pLua->SetVar("DragEventArgs", &MWidget::m_DragEventArgs);

	// linked text argument
	MWLua::class_<MLinkedTextArgs>(m_pLua->GetState(), "MLinkedTextArgs")
		.def("GetString",			&MLinkedTextArgs::GetString)
		.def("GetArgument",			&MLinkedTextArgs::GetArgument)
		;
	m_pLua->SetVar("LinkedTextArgs", &MWidget::m_LinkedTextArgs);
}


void MScriptManager::DeclWidget(const char* szClass, const char* szName, MWidget* pWidget)
{
	if (m_pLua == NULL) return;

	if(!strcmp(szClass, MINT_WIDGET))
	{
		m_pLua->SetVar(szName, pWidget);
	}
	else if(!strcmp(szClass, MINT_FRAME))
	{
		MFrame* pFrame = (MFrame*)pWidget;
		m_pLua->SetVar(szName, pFrame);
	}
	else if(!strcmp(szClass, MINT_BUTTON))
	{
		MButton* pButton = (MButton*)pWidget;
		m_pLua->SetVar(szName, pButton);
	}
	else if(!strcmp(szClass, MINT_EDIT))
	{
		MEdit* pEdit = (MEdit*)pWidget;
		m_pLua->SetVar(szName, pEdit);
	}
	else if(!strcmp(szClass, MINT_LABEL))
	{
		MLabel* pLabel = (MLabel*)pWidget;
		m_pLua->SetVar(szName, pLabel);
	}
	else if(!strcmp(szClass, MINT_TIMER))
	{
		MMTimer* pTimer = (MMTimer*)pWidget;
		m_pLua->SetVar(szName, pTimer);
	}
	else if(!strcmp(szClass, MINT_LISTBOX))
	{
		MListBox* pListBox = (MListBox*)pWidget;
		m_pLua->SetVar(szName, pListBox);
	}
	else if(!strcmp(szClass, MINT_PANEL))
	{
		MPanel* pPanel = (MPanel*)pWidget;
		m_pLua->SetVar(szName, pPanel);
	}
	else if(!strcmp(szClass, MINT_PAINT))
	{
		MPaint* pPaint = (MPaint*)pWidget;
		m_pLua->SetVar(szName, pPaint);
	}
	else if(!strcmp(szClass, MINT_TEXTAREA))
	{
		MTextArea* pTextArea = (MTextArea*)pWidget;
		m_pLua->SetVar(szName, pTextArea);
	}
	else if(!strcmp(szClass, MINT_COMBOBOX))
	{
		MComboBox* pComboBox = (MComboBox*)pWidget;
		m_pLua->SetVar(szName, pComboBox);
	}
	else if(!strcmp(szClass, MINT_TABCTRL))
	{
		MTabCtrl* pTabCtrl = (MTabCtrl*)pWidget;
		m_pLua->SetVar(szName, pTabCtrl);
	}
	else if(!strcmp(szClass, MINT_SCROLLBAR))
	{
		MScrollBar* pScrollBar = (MScrollBar*)pWidget;
		m_pLua->SetVar(szName, pScrollBar);
	}
	else if(!strcmp(szClass, MINT_PICTURE))
	{
		MPicture* pPicture = (MPicture*)pWidget;
		m_pLua->SetVar(szName, pPicture);
	}
	else if(!strcmp(szClass, MINT_LAYER))
	{
		MLayer* pLayer = (MLayer*)pWidget;
		m_pLua->SetVar(szName, pLayer);
	}
	else if(!strcmp(szClass, MINT_TEXTVIEW))
	{
		MTextView* pTextView = (MTextView*)pWidget;
		m_pLua->SetVar(szName, pTextView);
	}
}

void MScriptManager::ReleaseWidget(const char* szName)
{
	if (m_pLua == NULL) return;
	m_pLua->SetNull(szName);
}

void MScriptManager::AddWidgetEvent(string& strWidgetName, string& strEventName, string& strScript)
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


void MScriptManager::RunScriptWidgetEvent(MWidget* pWidget, const char* szEventName)
{
	if (m_pLua == NULL) return;
	if (pWidget == NULL) return;

	string strFuncName = pWidget->GetName() + string("_") + string(szEventName);

	// TODO: 만약 RunString이 느리면 차후 루아 함수를 직접 호출하는걸로 바꾸자
//	call<void>("foo", strScript.c_str());
//	m_pLua->RunString(strScript.c_str(), WLUA_DEBUG_ARG);
	m_pLua->MemberCall(LUA_EVENTTABLE_NAME, strFuncName.c_str(), pWidget->GetName().c_str(), WLUA_DEBUG_ARG, WNULL);
}


} // namespace mint3