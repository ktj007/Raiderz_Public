#include "stdafx.h"
#include "XUIJournalIndicator.h"
#include "MColorTable.h"
#include "XRecipeInfo.h"
#include "XRecipeInfoMgr.h"
#include "XItemManager.h"
#include "XGameState.h"

#define DEFAULT_ITEMGAP		0




// XJournalItem
XJournalItem::XJournalItem( const char* szName, MWidget* pParent, MListener* pListener)
: MTextView( szName, pParent, pListener)
{
	m_bEnableFocus = true;
	m_bWrap = false;

	GetScrollBar()->SetAlwaysHide( true);
	SetAnchor( MAnchorStyle( true, true, true, false));


	m_nID = 0;
	m_bActivate = false;

	m_pCloseBtn = new MButton( NULL, this, this);
	m_pCloseBtn->SetRect( m_Rect.w - 14 - 2, 0, 14, 14);
	m_pCloseBtn->SetAnchor( MAnchorStyle( false, true, true, false));
}

XJournalItem::~XJournalItem()
{
	Destroy();

	if ( m_pCloseBtn != NULL)
	{
		delete m_pCloseBtn;
		m_pCloseBtn = NULL;
	}
}

const char* XJournalItem::GetState()
{
	if ( IsEnabled() == false)									return MWS_DISABLE;
	else if ( XUIJournalIndicator::GetFocused() == this)		return MWS_DOWN;
	else if ( IsMouseOver() == true)							return MWS_FOCUS;
	return MWS_NORMAL;
}

int XJournalItem::GetStateNum()
{
	if ( IsEnabled() == false)									return MWSN_DISABLE;
	else if ( XUIJournalIndicator::GetFocused() == this)		return MWSN_DOWN;
	else if ( IsMouseOver() == true)							return MWSN_FOCUS;
	return MWSN_NORMAL;
}

bool XJournalItem::Create( int nID)
{
	m_nID = nID;

	RefreshText();
	return true;
}

bool XJournalItem::Destroy()
{
	RemoveFromParent();
	return true;
}

bool XJournalItem::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( MTextView::OnCommand( pWidget, nMsg, szArgs) == true)		return true;

	if ( pWidget == m_pCloseBtn  &&  nMsg == MEVENT_CLICK)
	{
		XUIJournalIndicator* pIndicator = (XUIJournalIndicator*)GetParent();
		if ( pIndicator != NULL)
		{
			// Remove from account info
			XAccountData* pAccount = gvar.Char.m_AccountInfo.GetAccountInfo();
			if ( pAccount != NULL)		pAccount->RemoveJournal( GetType(), GetID());


			// Remove from list
			pIndicator->RemoveItem( GetType(), GetID());

			if (GetType() == XJT_QUEST)	global.script->GetGlueGameEvent().OnGameEvent( "QUEST");
			else						global.script->GetGlueGameEvent().OnGameEvent( "RECIPE", "REFRESH");

			return true;
		}
	}

	return false;
}





// XJournalQuest
XJournalQuest::XJournalQuest( const char* szName, MWidget* pParent, MListener* pListener)
: XJournalItem( szName, pParent, pListener)
{
	m_bActivate = true;
}

bool XJournalQuest::RefreshText()
{
	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get( m_nID);
	XQuestInfo* pQuest = info.quest->Get( m_nID);
	if ( pQuest == NULL)	return false;


	wstring strText;
	wchar_t buff[ 256];


	// Title
	{
		MCOLOR col = HasLookAndFill() ? m_pLookAndFill->GetFontColor( "quest", MWS_NORMAL, MColorTable::TEXT[ MWSN_FOCUS]) : MCOLOR( 255, 255, 255);
		swprintf_s( buff, L"{COLOR r=%d g=%d b=%d a=%d}", col.r, col.g, col.b, col.a);
		strText += buff;
		strText += pQuest->GetTitle();
	}


	// Objective
	if ( pPlayerQuest != NULL)
	{
		if ( pQuest->IsEmptyQObjective() == false)
		{
			MCOLOR col = HasLookAndFill() ? m_pLookAndFill->GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]) : MCOLOR( 255, 255, 255);
			swprintf_s( buff, L"{INDENT}{COLOR r=%d g=%d b=%d a=%d}", col.r, col.g, col.b, col.a);
			strText += buff;
		}

		vector<XQObjectiveInfo*> vecQObjectiveInfo = pQuest->GetAllQObjectiveInfo();
		for each (XQObjectiveInfo* pQObjInfo in vecQObjectiveInfo)
		{
			XPlayerQObjective* pQObjective = pPlayerQuest->Get(pQObjInfo->nID);
			if ( pQObjective == NULL)	continue;

			if ( pPlayerQuest->IsComplete() == true  ||  pQObjective->bComplete == true)
			{
				swprintf_s( buff, L"{CR}{/INDENT}{BITMAP name=\"bmpCheck\" w=16 h=16}{CR h=0}{INDENT}%s", pQObjInfo->GetDesc());
				strText += buff;
			}
			else if (pPlayerQuest->IsFail() == true)
			{
				swprintf_s( buff, L"{CR}{/INDENT}{CR h=0}{INDENT}%s (실패)", pQObjInfo->GetDesc());
				strText += buff;
			}
			else
			{
				switch ( pQObjInfo->nType)
				{
				case QOT_ACT :
				case QOT_SCOUT :
				case QOT_ESCORT :
					swprintf_s( buff, L"{CR}%s", pQObjInfo->GetDesc());
					strText += buff;
					break;

				case QOT_INTERACT :
				case QOT_DESTROY :
				case QOT_COLLECT :
					if ( pQObjInfo->nParam2 > 1)	swprintf_s( buff, L"{CR}%s (%d/%d)", pQObjInfo->GetDesc(), pQObjective->nProgress, pQObjInfo->nParam2);
					else							swprintf_s( buff, L"{CR}%s", pQObjInfo->GetDesc());
					strText += buff;
					break;
				}
			}
		}
	}


	// Update text
	if ( strText != MLocale::ConvAnsiToUTF16(m_strText.c_str()))
	{
		SetText( MLocale::ConvUTF16ToAnsi(strText.c_str()).c_str());

//		SetToolTipText( pQuest->GetTitle());

		return true;
	}

	return false;
}

bool XJournalQuest::OnRefresh()
{
	return RefreshText();
}





// XJournalRecipe
XJournalRecipe::XJournalRecipe( const char* szName, MWidget* pParent, MListener* pListener)
: XJournalItem( szName, pParent, pListener)
{
	m_bActivate = true;
}

bool XJournalRecipe::RefreshText()
{
	XRecipeInfo* pRecipeInfo = info.recipe->Get( m_nID);
	if ( pRecipeInfo == NULL)	return false;

	XItemData* pItem = info.item->GetItemData( pRecipeInfo->m_nProductItemID);
	if ( pItem == NULL)			return false;


	wstring strText;
	wchar_t buff[ 256];


	// Title
	{
		MCOLOR col = HasLookAndFill() ? m_pLookAndFill->GetFontColor( "recipe", MWS_NORMAL, MColorTable::TEXT[ MWSN_DOWN]) : MCOLOR( 255, 255, 255);
		swprintf_s( buff, L"{COLOR r=%d g=%d b=%d a=%d}", col.r, col.g, col.b, col.a);
		strText += buff;
		strText += pItem->GetName();
	}


	// Objective
	size_t _size = pRecipeInfo->m_vecMaterial.size();
	if ( _size > 0)
	{
		MCOLOR col = HasLookAndFill() ? m_pLookAndFill->GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]) : MCOLOR( 255, 255, 255);
		swprintf_s( buff, L"{INDENT}{COLOR r=%d g=%d b=%d a=%d}", col.r, col.g, col.b, col.a);
		strText += buff;
	}

	for ( size_t i = 0;  i < _size;  i++)
	{
		RecipeMaterial& _material = pRecipeInfo->m_vecMaterial[ i];
		XItemData* pMaterial = info.item->GetItemData( _material.m_nItemID);

		int nOwnedAmount = gvar.MyInfo.Inventory.GetItemAmountByID( _material.m_nItemID) + gvar.MyInfo.EquipmentSlot.GetItemQuantityByID(_material.m_nItemID);
		int nRequestedAmount = _material.m_nAmount;

		if ( nOwnedAmount >= nRequestedAmount)	swprintf_s( buff, L"{CR}{/INDENT}{BITMAP name=\"bmpCheck\" w=16 h=16}{CR h=0}{INDENT}%s (%d/%d)", pMaterial->GetName(), nOwnedAmount, nRequestedAmount);
		else									swprintf_s( buff, L"{CR}%s (%d/%d)", pMaterial->GetName(), nOwnedAmount, nRequestedAmount);
		strText += buff;
	}


	// Update text
	if ( strText != MLocale::ConvAnsiToUTF16(m_strText.c_str()))
	{
		SetText( MLocale::ConvUTF16ToAnsi(strText.c_str()).c_str());

//		SetToolTipText( pItem->m_strName.c_str());

		return true;
	}

	return false;
}

bool XJournalRecipe::OnRefresh()
{
	return RefreshText();
}









XJournalItem* XUIJournalIndicator::m_pFocusedItem = NULL;


// XUIJournalIndicator
XUIJournalIndicator::XUIJournalIndicator( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_pTextLook = NULL;
	m_pButtonLook = NULL;
	m_bRefreshAll = false;
	m_bUpdateScrollBar = false;
	m_bArrangeChildren = false;
}


// ~XUIJournalIndicator
XUIJournalIndicator::~XUIJournalIndicator()
{
	Clear();

	if ( m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}

	XUIJournalIndicator::m_pFocusedItem = NULL;
}


// AddItem
bool XUIJournalIndicator::AddItem( int nType, int nID)
{
	for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
	{
		if ( (*itr)->GetType() == nType  &&  (*itr)->GetID() == nID)
			return false;
	}


	XJournalItem* pItem = NULL;

	switch ( nType)
	{
	case XJT_QUEST :
		if ( gvar.MyInfo.Quests.IsPlayerQuest( nID) == true)	pItem = new XJournalQuest( NULL, this, this);
		break;

	case XJT_RECIPE :
		if ( gvar.MyInfo.RecipeInfo.IsExist( nID) == true)		pItem = new XJournalRecipe( NULL, this, this);
		break;

	default :
		break;
	}

	if ( pItem == NULL)		return false;

	
	if ( m_pTextLook != NULL)		pItem->SetLookAndFill( m_pTextLook);
	if ( m_pButtonLook != NULL)		pItem->GetCloseBtn()->SetLookAndFill( m_pButtonLook);
	pItem->SetRect( 0, 0, (m_Rect.w - m_pScrollBar->GetWidth()), 100);
	pItem->Create( nID);
	m_Children.push_back( pItem);


	m_bArrangeChildren = true;
	m_bUpdateScrollBar = true;

	return true;
}


// RemoveItem
bool XUIJournalIndicator::RemoveItem( int nType, int nID)
{
	for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
	{
		XJournalItem* pItem = (*itr);
		if ( pItem->GetType() != nType  ||  pItem->GetID() != nID)		continue;


		m_Children.erase( itr);

		// 아이템 삭제 명령을 받으면 바로 지우는게 아니라 update 타임에 삭제한다
		m_DeleteItemQueue.push_back( pItem);


		m_bArrangeChildren = true;
		m_bUpdateScrollBar = true;

		return true;
	}

	return false;
}


// RefreshItem
bool XUIJournalIndicator::RefreshItem( int nType, int nID)
{
	for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
	{
		XJournalItem* pItem = (*itr);
		if ( pItem->GetType() != nType  ||  pItem->GetID() != nID)		continue;


		if ( pItem->RefreshText() == true)
		{
			m_bArrangeChildren = true;
			m_bUpdateScrollBar = true;
		}

		return true;
	}

	return true;
}


// Clear
void XUIJournalIndicator::Clear()
{
	if ( m_Children.empty() == false)
	{
		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
			delete (*itr);

		m_Children.clear();

		m_bArrangeChildren = true;
		m_bUpdateScrollBar = true;
	}

	XUIJournalIndicator::m_pFocusedItem = NULL;
}


// GetFocusedTypeID
bool XUIJournalIndicator::GetFocusedTypeID( XJOURNALTYPE* pnType, int* pnID)
{
	if ( XUIJournalIndicator::m_pFocusedItem == NULL)
	{
		*pnType = XJT_NONE;
		*pnID = 0;
		return false;
	}

	*pnType = XUIJournalIndicator::m_pFocusedItem->GetType();
	*pnID = XUIJournalIndicator::m_pFocusedItem->GetID();
	return true;
}


// ScrollPage
void XUIJournalIndicator::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);

	m_bArrangeChildren = true;
}


// OnPreTranslateMsg
MRESULT XUIJournalIndicator::OnPreTranslateMsg( MEvent* pEvent)
{
	if ( pEvent->nMessage == MWM_MOUSEWHEEL)
	{
		if ( GetWindowRect().InPoint( pEvent->Pos) == true)
		{
			ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
			return MTRANSLATED;
		}
	}

	return MWidget::OnPreTranslateMsg( pEvent);
}


// OnCommand
bool XUIJournalIndicator::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( MWidget::OnCommand( pWidget, nMsg, szArgs) == true)		return true;

	if ( pWidget != NULL  &&  strcmp( pWidget->GetClassName(), MINT_JOURNALITEM) == 0  &&  ( nMsg == MEVENT_CLICK  ||  ( nMsg == MEVENT_RCLICK)))
	{
		if ( nMsg == MEVENT_CLICK)
		{
			XUIJournalIndicator::m_pFocusedItem = ( XUIJournalIndicator::m_pFocusedItem == pWidget) ? NULL : (XJournalItem*)pWidget;

			MEvent e;
			e.nKey = 0;
			e.nDelta = 0;
			e.bLButton = false;
			e.bRButton = false;
			e.bAlt = false;
			e.bCtrl = false;
			e.Pos = MGetMint()->GetLastCursorPos();

			e.nMessage = MWM_LBUTTONDOWN;
			OnItemClick( &e, -1);
		}
		else if ( nMsg == MEVENT_RCLICK)
		{
			XUIJournalIndicator::m_pFocusedItem = (XJournalItem*)pWidget;

			MEvent e;
			e.nKey = 0;
			e.nDelta = 0;
			e.bLButton = false;
			e.bRButton = false;
			e.bAlt = false;
			e.bCtrl = false;
			e.Pos = MGetMint()->GetLastCursorPos();

			e.nMessage = MWM_RBUTTONDOWN;
			OnItemRClick( &e, -1);
		}

		return true;
	}

	return false;
}

// OnRun
void XUIJournalIndicator::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	static int nFieldID = 0;
	XFieldInfo* pFieldInfo = NULL;
	if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld())
	{
		pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
		if ( pFieldInfo != NULL  &&  nFieldID != pFieldInfo->m_nFieldID)
		{
			nFieldID = pFieldInfo->m_nFieldID;
			m_bArrangeChildren = true;
		}
	}



	// Delete items
	if ( m_DeleteItemQueue.empty() == false)
	{
		for ( list< XJournalItem*>::iterator itr = m_DeleteItemQueue.begin();  itr != m_DeleteItemQueue.end();  itr++)
		{
			XJournalItem* pItem = (*itr);

			if ( XUIJournalIndicator::m_pFocusedItem == pItem)
				XUIJournalIndicator::m_pFocusedItem = NULL;

			pItem->Destroy();
			delete pItem;
		}

		m_DeleteItemQueue.clear();
	}


	// Update children
	if ( m_bRefreshAll == true)
	{
		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		{
			if ( (*itr)->OnRefresh() == true)
			{
				m_bArrangeChildren = true;
				m_bUpdateScrollBar = true;
			}
		}

		m_bRefreshAll = false;
	}

	
	// Arrange child
	if ( m_bArrangeChildren == true)
	{
		int _y = -m_pScrollBar->GetScrollValue();
		int _w = m_Rect.w - m_pScrollBar->GetWidth();

		// Recipe
		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		{
			XJournalItem* pItem = (*itr);
			if ( pItem->GetType() != XJT_RECIPE)		continue;

			MSIZE _size = pItem->GetPageSize();
			pItem->SetRect( 0, _y, _w, _size.h + 1);

			_y += _size.h + DEFAULT_ITEMGAP;
		}


		// 현재 필드에 속하는 퀘스트가 아니면 비활성화 시킨다
		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		{
			XJournalItem* pItem = (*itr);
			if ( pItem->GetType() != XJT_QUEST)		continue;

			bool bFieldIn = false;
			XPlayerQuest* pQuest = gvar.MyInfo.Quests.Get( pItem->GetID());
			if ( pFieldInfo == NULL  ||  pQuest == NULL  ||  pQuest->m_pQuestInfo->IsEmptyQObjective() == true)
				bFieldIn = true;
			else
			{
				vector<XQObjectiveInfo*> vecQObjectiveInfo = pQuest->m_pQuestInfo->GetAllQObjectiveInfo();
				for each (XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
				{
					XPlayerQObjective* pQObjective = pQuest->Get(pQObjectiveInfo->nID);
					if ( pQObjective == NULL)		continue;

					if ( pQObjectiveInfo->nFieldID == 0  ||  pQObjectiveInfo->nFieldID == pFieldInfo->m_nFieldID)
					{
						bFieldIn = true;
						break;
					}
				}
			}

			pItem->SetActivate( bFieldIn);
			pItem->SetOpacity( bFieldIn ? 1.0f : 0.75f);
		}

		// Quest - activated 된것부터 쌓는다
		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		{
			XJournalItem* pItem = (*itr);
			if ( pItem->GetType() != XJT_QUEST)		continue;
			if ( pItem->IsActivate() == false)		continue;

			MSIZE _size = pItem->GetPageSize();
			pItem->SetRect( 0, _y, _w, _size.h);

			_y += _size.h + DEFAULT_ITEMGAP;
		}

		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		{
			XJournalItem* pItem = (*itr);
			if ( pItem->GetType() != XJT_QUEST)		continue;
			if ( pItem->IsActivate() == true)		continue;

			MSIZE _size = pItem->GetPageSize();
			pItem->SetRect( 0, _y, _w, _size.h);

			_y += _size.h + DEFAULT_ITEMGAP;
		}

		m_bArrangeChildren = false;
	}


	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		MRECT r = GetClientRect();

		int nHeight = 0;
		for ( list< XJournalItem*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
			nHeight += (*itr)->GetSize().h + DEFAULT_ITEMGAP;


		if ( nHeight <= r.h)
		{
			m_pScrollBar->SetScrollRange( 0, 0);
			m_pScrollBar->SetScrollValue( 0);
		}
		else
		{
			int _val = m_pScrollBar->GetScrollValue();
			int _max = nHeight - r.h;
			m_pScrollBar->SetScrollRange( 0, _max);
			m_pScrollBar->SetScrollValue( _val);
		}

		m_bUpdateScrollBar = false;
	}
}


// OnSize
void XUIJournalIndicator::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	m_bArrangeChildren = true;
	m_bUpdateScrollBar = true;
}


// OnScrollBarChanged
void XUIJournalIndicator::OnScrollBarChanged( int nPos)
{
	MWidget::OnScrollBarChanged( nPos);

	m_bArrangeChildren = true;
}


MWLua::ret_int2 XUIJournalIndicator::glueGetFocused()
{
	int _type = -1, _id = -1;

	if ( XUIJournalIndicator::m_pFocusedItem != NULL)
	{
		_type = (int)XUIJournalIndicator::m_pFocusedItem->GetType();
		_id = XUIJournalIndicator::m_pFocusedItem->GetID();
	}

	return MWLua::ret_int2( _type, _id);
}