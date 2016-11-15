#include "stdafx.h"
#include "MListBox.h"
#include "MScrollBar.h"
#include "Mint.h"
#include "MColorTable.h"



namespace mint3
{


#define DEFAULT_ITEMHEIGHT			17



// MListBox
MListBox::MListBox( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MListBoxLook);

	SetEnableFocus( true);

	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_nCaret = -1;
	m_nStartSel = m_nCaret;
	m_nMouseOver = -1;
	m_bDragAndDrop = false;
	m_bMultipleSel = false;
	m_bEnsureCaretVisible = false;
	m_bUpdateScrollBar = false;
	m_bReleaseSelOnLBtnUp = false;
}


// ~MListBox
MListBox::~MListBox()
{
	ResetContent();

	if ( m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}


// AddString
int MListBox::AddString( const char* szString)
{
	MLBITEM* pItem = new MLBITEM( szString);
	m_ItemList.push_back( pItem);

	UpdateScrollBar();

	return (int)(m_ItemList.size() - 1);
}


// InsertString
int MListBox::InsertString( size_t nIndex, const char* szString)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= _size)		return AddString( szString);

	ReleaseSelect();

	vector< MLBITEM*>::iterator itr = m_ItemList.begin();
	for ( size_t i = 0;  i < _size;  i++, itr++) ;

	MLBITEM* pItem = new MLBITEM( szString);
	m_ItemList.insert( itr, pItem);

	UpdateScrollBar();

	return nIndex;
}


// DeleteString
int MListBox::DeleteString( size_t nIndex)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= _size)	return -1;

	ReleaseSelect();

	vector< MLBITEM*>::iterator itr = m_ItemList.begin();
	for ( size_t i = 0;  i != nIndex;  i++, itr++) ;

	delete m_ItemList[ nIndex];
	m_ItemList.erase( itr);

	if ( (int)nIndex < m_nCaret)
	{
		m_nCaret--;
		m_nStartSel = m_nCaret;
	}

	EnsureCaretVisible();
	UpdateScrollBar();

	return (int)nIndex;
}


// ResetContent
void MListBox::ResetContent()
{
	if ( m_ItemList.empty() == false)
	{
		size_t _size = m_ItemList.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_ItemList[ i];
		m_ItemList.clear();
	}

	ReleaseSelect();

	m_nCaret = -1;
	m_nStartSel = m_nCaret;
	m_nMouseOver = -1;

	EnsureCaretVisible();
	UpdateScrollBar();
}


// GetItemHeight
int MListBox::GetItemHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MListBoxLook*)m_pLookAndFill)->GetItemHeight();
		if ( height <= 0)
			height = m_pLookAndFill->GetFont()->GetHeight() + 2;
	}
	if ( height <= 0)	height = DEFAULT_ITEMHEIGHT;

	return height;
}


// SetCurSel
int MListBox::SetCurSel( int nIndex)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= (int)_size)	nIndex = -1;

	ReleaseSelect();

	if ( nIndex < 0)			m_nCaret = -1;
	else
	{
		m_nCaret = nIndex;
		m_SelectedItems.insert( nIndex);
	}

	return m_nCaret;
}


// SetSel
int MListBox::SetSel( size_t nIndex, bool bSelect)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= (int)_size)	return -1;

	if ( bSelect)
	{
		if ( m_bMultipleSel == false)	ReleaseSelect();
		m_SelectedItems.insert( nIndex);
	}
	else
	{
		m_SelectedItems.erase( nIndex);
	}

	return nIndex;
}


// GetSelItem
int MListBox::GetSelItem( size_t nSelIndex) const
{
	size_t _size = m_SelectedItems.size();
	if ( nSelIndex >= _size)	return -1;

	set< size_t>::const_iterator itr = m_SelectedItems.begin();
	for ( size_t i = 0;  i != nSelIndex;  i++, itr++) ;
	return (*itr);
}


// FindString
int MListBox::FindString( size_t nStartAfter, const char* szText) const
{
	size_t _size = m_ItemList.size();
	if ( nStartAfter >= _size)		return -1;

	string strText( szText);
	for ( size_t i = nStartAfter;  i < _size;  i++)
	{
		if ( strText == m_ItemList[ i]->m_strText)
			return i;
	}

	return -1;
}


// SelItemRange
int MListBox::SelItemRange( size_t first, size_t last, bool bSelect)
{
	if ( first > last)	return 0;

	if ( m_bMultipleSel == false)
	{
		if ( bSelect == true)		m_SelectedItems.insert( first);
		else						m_SelectedItems.erase( first);
		return 1;
	}
	else
	{
		for ( size_t i = first;  i <= last;  i++)
		{
			if ( bSelect == true)	m_SelectedItems.insert( i);
			else					m_SelectedItems.erase( i);
		}

		return ( last - first + 1);
	}

	return 0;
}


// SetCaretPos
void MListBox::SetCaretPos( int nPos)
{
	if ( nPos < 0)
		nPos = -1;
	else
	{
		int _size = (int)m_ItemList.size();
		if ( nPos >= (int)_size)	nPos = _size - 1;
	}

	m_nCaret = nPos;
	m_nStartSel = nPos;
}


// EnableMultipleSel
void MListBox::EnableMultipleSel( bool bEnable)
{
	if ( m_bMultipleSel == bEnable)		return;

	m_bMultipleSel = bEnable;
	if ( m_bMultipleSel == false)		ReleaseSelect();
}


// ScrollPage
void MListBox::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);
}


// ItemFromPoint
int MListBox::ItemFromPoint( const MPOINT& pt)
{
	if ( m_ItemList.empty())				return -1;

	MRECT r = GetClientRect();
	if ( r.InPoint( (MPOINT)pt) == false)	return -1;

	int _item_height = GetItemHeight();
	int index = m_pScrollBar->GetScrollValue() + pt.y / _item_height;
	if ( index >= (int)m_ItemList.size())	return -1;
	return index;
}


// OnEvent
bool MListBox::OnEvent( MEvent* pEvent, MListener* pListener)
{
#define GET_RANGE(s,e,_s,_e)		{ if (_s>_e) {s=_e; e=_s;}	else {s=_s; e=_e;} }


	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	// Translate message
	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		{
			int _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel < 0)
			{
				ReleaseSelect();

				m_nCaret = -1;
				m_nStartSel = m_nCaret;
			}
			else
			{
				m_nCaret = _sel;
				if ( m_nStartSel < 0)	m_nStartSel = _sel;

				if ( pEvent->bShift == true)
				{
					ReleaseSelect();

					if ( m_bMultipleSel == true)
					{
						size_t st, ed;
						GET_RANGE( st, ed, m_nCaret, m_nStartSel);
						for ( size_t i = st;  i <= ed;  i++)	m_SelectedItems.insert( i);
					}
					else
						m_SelectedItems.insert( m_nCaret);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_nStartSel = m_nCaret;

					if ( m_bMultipleSel == true)
					{
						if ( m_SelectedItems.find( m_nCaret) == m_SelectedItems.end())
							m_SelectedItems.insert( m_nCaret);
						else
							m_SelectedItems.erase( m_nCaret);
					}
					else
					{
						ReleaseSelect();
						m_SelectedItems.insert( m_nCaret);
					}
				}
				else if ( GetSel( _sel) == true)
				{
					m_nStartSel = m_nCaret;
					m_bReleaseSelOnLBtnUp = true;
				}
				else
				{
					m_nStartSel = m_nCaret;

					ReleaseSelect();
					m_SelectedItems.insert( m_nCaret);
				}
			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemClick( pEvent, _sel);
			break;
		}

	case MWM_LBUTTONUP :
		{
			if ( m_bReleaseSelOnLBtnUp == true)
			{
				ReleaseSelect();
				m_SelectedItems.insert( m_nCaret);
				m_bReleaseSelOnLBtnUp = false;
			}
			break;
		}

	case MWM_LBUTTONDBLCLK :
		{
			ReleaseSelect();

			int _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel < 0)
			{
				m_nCaret = -1;
				m_nStartSel = m_nCaret;
			}
			else
			{
				m_nCaret = _sel;
				m_nStartSel = m_nCaret;
				m_SelectedItems.insert( m_nCaret);
			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemDblClick( pEvent, _sel);
			break;
		}

	case MWM_RBUTTONDOWN :
		{
			ReleaseSelect();

			int _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel < 0)
			{
				m_nCaret = -1;
				m_nStartSel = m_nCaret;
			}
			else
			{
				m_nCaret = _sel;
				m_nStartSel = m_nCaret;
				m_SelectedItems.insert( m_nCaret);

			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemRClick( pEvent, _sel);
			break;
		}

	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;

	case MWM_KEYDOWN :
		{
			bool bUpdate = false;
			bool bTranslate = false;

			if ( pEvent->nKey == VK_UP  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)		m_nCaret = 0;
				else					m_nCaret = max( 0, m_nCaret - 1);
				bUpdate = true;
				bTranslate = true;
			}
			else if ( pEvent->nKey == VK_DOWN  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)		m_nCaret = 0;
				else					m_nCaret = min( (int)m_ItemList.size() - 1, m_nCaret + 1);
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_PRIOR  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)		m_nCaret = 0;
				else					m_nCaret = max( 0, m_nCaret - GetClientRect().h / GetItemHeight());
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_NEXT  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)		m_nCaret = 0;
				else					m_nCaret = min( (int)m_ItemList.size() - 1, m_nCaret + GetClientRect().h / GetItemHeight());
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_HOME  &&  m_ItemList.empty() == false)
			{
				m_nCaret = 0;
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_END  &&  m_ItemList.empty() == false)
			{
				m_nCaret = (int)m_ItemList.size() - 1;
				bUpdate = true;
				bTranslate = true;
			}

			// Select all
			else if ( pEvent->nKey == 'A'  &&  pEvent->bCtrl == true  &&  m_ItemList.empty() == false)
			{
				size_t _size = m_ItemList.size() - 1;
				m_nCaret = _size;
				m_nStartSel = m_nCaret;

				if ( m_bMultipleSel == true)
				{
					for ( size_t i = 0;  i <= _size;  i++)
						m_SelectedItems.insert( i);
				}
				else
					m_SelectedItems.insert( m_nCaret);			

				EnsureCaretVisible();
				bTranslate = true;
			}

			// Copy
			else if ( pEvent->nKey == 'C'  &&  pEvent->bCtrl == true  &&  m_SelectedItems.empty() == false)
			{
				string strString;
				for ( set< size_t>::const_iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
				{
					if ( strString.empty() == false)	strString += ",";
					strString += m_ItemList[ (*itr)]->m_strText;
				}
				SetClipboard( strString.c_str());
				bTranslate = true;
			}


			if ( bUpdate == true)
			{
				if ( pEvent->bShift == true)
				{
					ReleaseSelect();

					if ( m_bMultipleSel == true)
					{
						size_t st, ed;
						GET_RANGE( st, ed, m_nCaret, m_nStartSel);
						for ( size_t i = st;  i <= ed;  i++)	m_SelectedItems.insert( i);
					}
					else
						m_SelectedItems.insert( m_nCaret);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_nStartSel = m_nCaret;
				}
				else
				{
					m_nStartSel = m_nCaret;
					ReleaseSelect();
					m_SelectedItems.insert( m_nCaret);
				}

				EnsureCaretVisible();

				OnSelChanged();
			}

			if ( bTranslate == true)		return true;
		}

	case MWM_MOUSEMOVE :
		{
			int _sel = max( -1, ItemFromPoint( pEvent->Pos));
			if ( _sel != m_nMouseOver)
			{
				m_nMouseOver = _sel;
				MGetMint()->ChangedToolTipText( this);
			}
		}
	}

	return false;
}


// OnRun
void MListBox::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		m_bUpdateScrollBar = false;
		m_bEnsureCaretVisible = true;

		MRECT r = GetClientRect();
		int _item_height = GetItemHeight();
		int _height = (m_ItemList.size() + 1) * _item_height;
		if ( _height < r.h)
		{
			m_pScrollBar->SetScrollRange( 0, 0);
			m_pScrollBar->SetScrollValue( 0);
		}
		else
		{
			int _max = (int)( (float)(_height - r.h) / (float)_item_height);
			m_pScrollBar->SetScrollRange( 0, _max);
		}
	}


	// Ensure caret visible
	if ( m_bEnsureCaretVisible == true)
	{
		m_bEnsureCaretVisible = false;

		if ( m_nCaret >= 0)
		{
			int _start = m_pScrollBar->GetScrollValue();
			if ( m_nCaret < _start)		m_pScrollBar->SetScrollValue( m_nCaret);
			else
			{
				MRECT r = GetClientRect();
				int _view_count = (int)( (float)r.h / (float)GetItemHeight());
				if ( (m_nCaret - _start + 1) > _view_count)
					m_pScrollBar->SetScrollValue( m_nCaret - _view_count + 1);
			}
		}
	}
}


// OnSize
void MListBox::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	EnsureCaretVisible();
	UpdateScrollBar();
}


// OnDragBegin
bool MListBox::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( GetCurSel() < 0)	return false;

	for ( set< size_t>::iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
	{
		int index = (*itr);
		MDragObject* pObject = new MDragObject( m_ItemList[ index]->m_strText, NULL, m_ItemList[ index]->m_dwItemData);
		pDragData->AddObject( pObject);
	}

	if ( HasLookAndFill())
	{
		pDragData->SetFont( m_pLookAndFill->GetFont());
	}

	m_bReleaseSelOnLBtnUp = false;

	return MWidget::OnDragBegin( pDragData, pt);
}


// OnToolTip
MWidget* MListBox::OnToolTip()
{
	if ( m_nMouseOver < 0  ||  m_nMouseOver >= (int)m_ItemList.size())
		return NULL;

	return MWidget::OnToolTip();
}


// OnLeave
void MListBox::OnLeave()
{
	MWidget::OnLeave();

	if ( m_nMouseOver >= 0)		MGetMint()->GetToolTipManager()->Reset();
	m_nMouseOver = -1;
}


// glueGetMouseOver
int MListBox::glueGetMouseOver()
{
	if ( m_nMouseOver >= (int)GetItemCount())		m_nMouseOver = -1;

	return m_nMouseOver;
}







// MListBoxLook
MListBoxLook::MListBoxLook()
{
	SetWorkRectOffset( MRECT( 2, 2, -4, -4));
	SetTextAlign( MAM_LEFT | MAM_VCENTER);

	m_nItemHeight = 0;
}

void MListBoxLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawItems( (MListBox*)pWidget, pDC);
}

void MListBoxLook::OnDrawItems( MListBox* pListbox, MDrawContext* pDC)
{
	// Set font
	pDC->SetFont( GetFont());

	// Draw items
	MRECT r = pListbox->GetClientRect();
	int x = 0, y = 0;
	int _item_height = pListbox->GetItemHeight();
	size_t _size = pListbox->GetItemCount();

	for ( size_t i = pListbox->GetScrollBar()->GetScrollValue();  i < _size;  i++)
	{
		MRECT _r( x, y, r.w, _item_height);

		// Draw item background
		OnDrawItemBkgrnd( pListbox, i, _r, pDC);

		// Draw select bar
		if ( pListbox->GetSel( i) == true)
			OnDrawSelectbar( pListbox, i, _r, pDC);

		// Draw caret
		if ( pListbox->GetCaretPos() == i)
			OnDrawCaret( pListbox, i, _r, pDC);


		if ( pListbox->EventPreDrawItem( pDC, i, 0, _r) == false)
		{
			// Draw item
			OnDrawItemText( pListbox, i, _r, pDC);


			pListbox->EventDrawItem( pDC, i, 0, r);
		}


		y += _item_height;
		if ( y > r.h)	break;
	}
}

void MListBoxLook::OnDrawItemText( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC)
{
	if ( pListbox->GetSel( index) == true)
		pDC->SetColor( GetFontColor( MWP_SELECTBAR, pListbox->GetState(), MColorTable::TEXT_SELECTED));
	else
	{
		if ( pListbox->IsEnabled() == false)
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DISABLE, MColorTable::TEXT[ MWSN_DISABLE]));

		else if ( pListbox->GetCaretPos() == index)
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MColorTable::TEXT[ MWSN_FOCUS]));

		else
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]));

	}

	pDC->Text( r, pListbox->GetItemText( index), GetTextAlign());
}

void MListBoxLook::OnDrawItemBkgrnd( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC)
{
	Draw( pDC, r, MWP_ITEMBKGRND, pListbox->GetState());

	pListbox->EventDrawItemBkgrnd( pDC, index, 0, r);
}

void MListBoxLook::OnDrawCaret( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_CARET, pListbox->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::BORDER[ pListbox->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MListBoxLook::OnDrawSelectbar( MListBox* pListbox, size_t index, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_SELECTBAR, pListbox->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::SELECTBAR[ pListbox->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}


} // namespace mint3


