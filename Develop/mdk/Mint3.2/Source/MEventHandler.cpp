#include "stdafx.h"
#include "MEventHandler.h"
#include "MWidget.h"
#include "MDragManager.h"
#include "Mint.h"
#include "MScriptManager.h"
#include "MResourceManager.h"


namespace mint3 {


static string strEventNames[ MEVENT_MAX] =
{
	/* MEVENT_LOADED */				"OnLoaded",
	/* MEVENT_UPDATE */				"OnUpdate",
	/* MEVENT_DRAWITEMBKGRND */		"OnDrawItemBkgrnd",
	/* MEVENT_PREDRAWITEM */		"OnPreDrawItem",
	/* MEVENT_DRAWITEM */			"OnDrawItem",
	/* MEVENT_PREDRAW */			"OnPreDraw",
	/* MEVENT_DRAW */				"OnDraw",
	/* MEVENT_SHOW */				"OnShow",
	/* MEVENT_ENABLE */				"OnEnable",
	/* MEVENT_SETFOCUS */			"OnSetFocus",
	/* MEVENT_KILLFOCUS */			"OnKillFocus",
	/* MEVENT_POSITION */			"OnPosition",
	/* MEVENT_SIZE */				"OnSize",
	/* MEVENT_LBUTTONDOWN */		"OnLButtonDown",
	/* MEVENT_LBUTTONUP */			"OnLButtonUp",
	/* MEVENT_RBUTTONDOWN */		"OnRButtonDown",
	/* MEVENT_RBUTTONUP */			"OnRButtonUp",
	/* MEVENT_MOUSEMOVE */			"OnMouseMove",
	/* MEVENT_MOUSEWHEEL */			"OnMouseWheel",
	/* MEVENT_CLICK */				"OnClick",
	/* MEVENT_RCLICK */				"OnRClick",
	/* MEVENT_DBLCLICK */			"OnDblClick",
	/* MEVENT_KEYDOWN */			"OnKeyDown",
	/* MEVENT_KEYUP */				"OnKeyUp",
	/* MEVENT_CHAR */				"OnChar",
	/* MEVENT_SELCHANGE */			"OnSelChange",
	/* MEVENT_VALUECHANGED */		"OnValueChanged",
	/* MEVENT_ITEMCLICK */			"OnItemClick",
	/* MEVENT_ITEMDBLCLICK */		"OnItemDblClick",
	/* MEVENT_ITEMRCLICK */			"OnItemRClick",
	/* MEVENT_DRAGBEGIN */			"OnDragBegin",
	/* MEVENT_DRAGEND */			"OnDragEnd",
	/* MEVENT_DROP */				"OnDrop",
	/* MEVENT_TOOLTIP */			"OnToolTip",
	/* MEVENT_OK */					"OnOK",
	/* MEVENT_CANCEL */				"OnCancel",
	/* MEVENT_ENTER */				"OnEnter",
	/* MEVENT_LEAVE */				"OnLeave",
	/* MEVENT_SETCURSOR */			"OnSetCursor",
	/* MEVENT_INITPOPUPMENU */		"OnInitPopupMenu",
	/* MEVENT_TIMER */				"OnTimer",
	/* MEVENT_NCHITTEST */			"OnNcHitTest",
	/* MEVENT_USERARGUMENT */		"OnUserArgument",
};



MWidget* MDragEventArgs::GetSender()
{
	return m_pSender;
}

void MDragEventArgs::AddDragData( const char* szText, const char* szImage, int nItemData)
{
	MBitmap* pBitmap = NULL;
	if ( szImage != NULL)	pBitmap = MBitmapManager::Get( szImage);

	MDragObject* pObj = new MDragObject( string( szText), pBitmap, (DWORD)nItemData);
	m_pDragData->AddObject( pObj);
}

void MDragEventArgs::SetFont( const char* szFont)
{
	MFont* pFont = NULL;
	if ( szFont != NULL)	pFont = MFontManager::Get( szFont);

	if ( pFont != NULL)
		m_pDragData->SetFont( pFont);
}

void MDragEventArgs::SetImageSize( int width, int height)
{
	m_pDragData->SetImageSize( MSIZE( width, height));
}

void MDragEventArgs::SetColor( int r, int g, int b)
{
	m_pDragData->SetColor( MCOLOR( r, g, b));
}

void MDragEventArgs::SetHotSpot( int px, int py)
{
	m_pDragData->SetHotSpot( MPOINT( px, py));
}

void MDragEventArgs::SetOpacity( float opacity)
{
	m_pDragData->SetOpacity( opacity);
}

int MDragEventArgs::GetItemCount() const
{
	return (int)m_pDragData->GetCount();
}

bool MDragEventArgs::IsEmpty() const
{
	return m_pDragData->IsEmpty();
}

void MDragEventArgs::SetItemText( int nIndex, const char* szName)
{
	return m_pDragData->SetItemText( nIndex, szName);
}

const char* MDragEventArgs::GetItemText( int nIndex) const
{
	return m_pDragData->GetItemText( nIndex);
}

void MDragEventArgs::SetItemImage( int nIndex, const char* szImage)
{
	MBitmap* pImage = MBitmapManager::Get( szImage);
	return m_pDragData->SetItemImage( nIndex, pImage);
}

const char* MDragEventArgs::GetItemImage( int nIndex) const
{
	MBitmap* pBitmap = m_pDragData->GetItemImage( nIndex);
	if ( pBitmap != NULL)	return pBitmap->GetName();
	return NULL;
}

int MDragEventArgs::GetItemData( int nIndex) const
{
	return (int)m_pDragData->GetItemData( nIndex);
}


void MDragEventArgs::SetEffect( const char* szEffect)
{
	MDrawEffect _effect = MDE_NORMAL;
	if ( strcmp( szEffect, "add") == 0)				_effect = MDE_ADD;
	else if ( strcmp( szEffect, "multiply") == 0)	_effect = MDE_MULTIPLY;
	m_pDragData->SetEffect( _effect);
}


const char* MDragEventArgs::GetEffect()
{
	MDrawEffect _effect = m_pDragData->GetEffect();
	static string _str;
	switch ( _effect)
	{
	case MDE_NORMAL :		_str = "normal";		break;
	case MDE_ADD :			_str = "add";			break;
	case MDE_MULTIPLY :		_str = "multiply";		break;
	}
	return _str.c_str();
}










MWidget* MPopupEventArgs::GetOwner()
{
	return m_pOwner;
}

void MPopupEventArgs::Set( MPOINT& ptPoint, MWidget* pOwner)
{
	m_ptPoint = ptPoint;
	m_pOwner = pOwner;
}

const char* FindEventNameFromMsg(MEventMsg nEventName)
{
	return strEventNames[ nEventName].c_str();
}


MEventMsg FindEventMsgFromName(const char* szEventName)
{
	string _event = szEventName;

	for ( int i = 0; i < MEVENT_MAX; i++)
	{
		if ( _event == strEventNames[ i])
			return (MEventMsg)i;
	}

	return MEVENT_UNKNOWN;
}


bool MEventHandler::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( m_pListener != NULL)
	{
		if ( m_pListener->OnCommand( pWidget, nMsg, szArgs) == true)
		{
			MWidget::m_EventArgs.Reset();
			return true;
		}
	}

	if ( pWidget->OnCommand( pWidget, nMsg, szArgs) == true)
	{
		MWidget::m_EventArgs.Reset();
		return true;
	}


	bool bRetVal = false;

	if ( ExistScriptEvent( nMsg) == true)
	{
		// Set 'this' parameter
		MWLua* pLua = Mint::GetInstance()->GetScriptManager()->GetLua();
		if ( pLua != NULL)
		{
			pLua->SetNull( "this");
			pLua->SetVar( "this", pWidget);
		}

		// Run script
		bRetVal = DoScriptEvent( pWidget->GetName(), nMsg, szArgs);
	}

	MWidget::m_EventArgs.Reset();
	return bRetVal;
}

bool MEventHandler::OnScriptEvent( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	bool bRetVal = false;
	if ( ExistScriptEvent( nMsg) == true)
	{
		// Set 'this' parameter
		MWLua* pLua = Mint::GetInstance()->GetScriptManager()->GetLua();
		if ( pLua != NULL)
		{
			pLua->SetNull( "this");
			pLua->SetVar( "this", pWidget);
		}

		// Run script
		bRetVal = DoScriptEvent( pWidget->GetName(), nMsg, szArgs);
	}

	MWidget::m_EventArgs.Reset();
	return bRetVal;
}

bool MEventHandler::OnScriptEvent( string& strName, MEventMsg nMsg, const char* szArgs)
{
	bool bRetVal = false;
	if ( ExistScriptEvent( nMsg) == true)
	{
		// Set 'this' parameter
		MWLua* pLua = Mint::GetInstance()->GetScriptManager()->GetLua();
		if ( pLua != NULL)
			pLua->SetNull( "this");

		// Run script
		bRetVal = DoScriptEvent( strName, nMsg, szArgs);
	}

	MWidget::m_EventArgs.Reset();
	return bRetVal;
}

bool MEventHandler::ExistScriptEvent(MEventMsg nMsg)
{
	if (m_ScriptEvents.empty()) return false;

	set<MEventMsg>::iterator itor = m_ScriptEvents.find(nMsg);
	if (itor != m_ScriptEvents.end()) return true;

	return false;
}

bool MEventHandler::DoScriptEvent( string& strName, MEventMsg nMsg, const char* szArgs)
{
	return Mint::GetInstance()->GetScriptManager()->RunScriptEvent( strName.c_str(), strEventNames[nMsg].c_str());
}


} // namespace mint3