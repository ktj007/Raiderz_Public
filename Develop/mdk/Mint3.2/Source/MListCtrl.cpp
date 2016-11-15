#include "stdafx.h"
#include "MListCtrl.h"
#include "MScrollBar.h"
#include "Mint.h"
#include "MColorTable.h"
#include <math.h>


namespace mint3
{


#define DEFAULT_ITEMHEIGHT			17




// MLCITEM
MLCITEM::MLCITEM( size_t nCol, const char* szText, MBitmap* pImage)
{
	m_bEnable = true;
	m_strText = szText;
	m_pImage = pImage;
	m_pColor = NULL;
	m_dwItemData = 0;

	if ( nCol > 1)
	{
		nCol = max( nCol - 1, 0);
		for ( size_t i = 0;  i < nCol;  i++)
		{
			MLCSUBITEM* pSubItem = new MLCSUBITEM;
			m_SubItemList.push_back( pSubItem);
		}
	}
}

MLCITEM::~MLCITEM()
{
	if ( m_SubItemList.empty() == false)
	{
		size_t _size = m_SubItemList.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_SubItemList[ i];
		m_SubItemList.clear();
	}

	if ( m_pColor)
	{
		delete m_pColor;
		m_pColor = NULL;
	}
}

void MLCITEM::_set_item_text( size_t nSubItem, const char* szText)
{
	if ( nSubItem == 0)		m_strText = szText;
	else					m_SubItemList[ nSubItem - 1]->m_strText = szText;
}

const char* MLCITEM::_get_item_text( size_t nSubItem)
{
	if ( nSubItem == 0)		return m_strText.c_str();
	return m_SubItemList[ nSubItem - 1]->m_strText.c_str();
}

void MLCITEM::_set_item_color( size_t nSubItem, MCOLOR& nColor)
{
	if ( nSubItem == 0)
	{
		if ( m_pColor == NULL)		m_pColor = new MCOLOR;
		*m_pColor = nColor;
	}
	else
	{
		if ( m_SubItemList[ nSubItem - 1]->m_pColor == NULL)
			m_SubItemList[ nSubItem - 1]->m_pColor = new MCOLOR;
		*m_SubItemList[ nSubItem - 1]->m_pColor = nColor;
	}
}

bool MLCITEM::_get_item_color( size_t nSubItem, MCOLOR& nColor)
{
	if ( nSubItem == 0)
	{
		if ( m_pColor == NULL)		return false;
		nColor = *m_pColor;
	}
	else
	{
		if ( m_SubItemList[ nSubItem - 1]->m_pColor == NULL)
			return false;
		nColor = *m_SubItemList[ nSubItem - 1]->m_pColor;
	}
	return true;
}

















// MListCtrl
MListCtrl::MListCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MListCtrlLook);

	SetEnableFocus( true);

	m_nViewStyle = MLCVS_LIST;
	m_bShowColumnHeader = true;
	m_nHorzMargin = 0;
	m_nVertMargin = 0;
	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_nCaret = -1;
	m_nStartSel = m_nCaret;
	m_nMouseOver = -1;
	m_bDragAndDrop = false;
	m_bMultipleSel = false;
	m_bAlwaysSel = false;
	m_bEnsureCaretVisible = false;
	m_bUpdateScrollBar = false;
	m_bReleaseSelOnLBtnUp = false;
}


// ~MListCtrl
MListCtrl::~MListCtrl()
{
	DeleteAllItems();

	if ( m_ColumnList.empty() == false)
	{
		size_t _size = m_ColumnList.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_ColumnList[ i];
		m_ColumnList.clear();
	}

	if ( m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}


// AddItem
int MListCtrl::AddItem( const char* szText, const char* szImage)
{
	MLCITEM* pItem = new MLCITEM( m_ColumnList.size(), szText, MBitmapManager::Get( szImage));
	m_ItemList.push_back( pItem);

	UpdateScrollBar();

	return (int)(m_ItemList.size() - 1);
}


// InsertItem
int MListCtrl::InsertItem( size_t nIndex, const char* szText, const char* szImage)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= _size)		return AddItem( szText, szImage);


	vector< MLCITEM*>::iterator itr = m_ItemList.begin();
	for ( size_t i = 0;  i < _size;  i++, itr++) ;

	MLCITEM* pItem = new MLCITEM( m_ColumnList.size(), szText, MBitmapManager::Get( szImage));
	m_ItemList.insert( itr, pItem);

	ReleaseSelect();
	UpdateScrollBar();

	return nIndex;
}


// DeleteItem
bool MListCtrl::DeleteItem( size_t nIndex)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= _size)	return false;


	vector< MLCITEM*>::iterator itr = m_ItemList.begin();
	for ( size_t i = 0;  i != nIndex;  i++, itr++) ;

	delete m_ItemList[ nIndex];
	m_ItemList.erase( itr);

	if ( (int)nIndex < m_nCaret)
	{
		m_nCaret--;
		m_nStartSel = m_nCaret;
	}

	ReleaseSelect();
	EnsureCaretVisible();
	UpdateScrollBar();

	return true;
}


// DeleteAllItems
void MListCtrl::DeleteAllItems()
{
	if ( m_ItemList.empty() == false)
	{
		for ( vector< MLCITEM*>::iterator itr = m_ItemList.begin();  itr != m_ItemList.end();  itr++)
			delete (*itr);

		m_ItemList.clear();
	}

	ReleaseSelect();
	EnsureCaretVisible();
	UpdateScrollBar();

	m_nCaret = -1;
	m_nStartSel = m_nCaret;
	m_nMouseOver = -1;
}


// AddColumn
int MListCtrl::AddColumn( const char* szText, int nAlign, int nWidth)
{
	DeleteAllItems();

	MLCCOLUMN* pColumn = new MLCCOLUMN( szText, nAlign, nWidth);
	m_ColumnList.push_back( pColumn);
	return (int)m_ColumnList.size();
}


// DeleteColumn
bool MListCtrl::DeleteColumn( size_t nCol)
{
	if ( nCol == 0)						return false;
	if ( nCol >= m_ColumnList.size())	return false;

	DeleteAllItems();

	vector< MLCCOLUMN*>::iterator itr = m_ColumnList.begin();
	for ( size_t i = 0;  i != nCol;  i++, itr++) ;

	delete m_ColumnList[ nCol];
	m_ColumnList.erase( itr);

	return true;
}


// SetItemEnable
void MListCtrl::SetItemEnable( size_t nIndex, bool bEnable)
{
	m_ItemList[ nIndex]->m_bEnable = bEnable;

	if ( bEnable == false)		m_SelectedItems.erase( nIndex);
}


// GetItemHeight
int MListCtrl::GetItemHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MListCtrlLook*)m_pLookAndFill)->GetItemHeight();
		if ( height <= 0)
			height = m_pLookAndFill->GetFont()->GetHeight() + 2;
	}
	if ( height <= 0)	height = DEFAULT_ITEMHEIGHT;

	return height;
}


// GetColumnHeaderHeight
int MListCtrl::GetColumnHeaderHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MListCtrlLook*)m_pLookAndFill)->GetColumnHeaderHeight();
		if ( height <= 0)	height = GetItemHeight();
	}
	else
		height = DEFAULT_ITEMHEIGHT;
	return height;
}


// SetViewStyle
void MListCtrl::SetViewStyle( int nViewStyle)
{
	if ( m_nViewStyle == nViewStyle)		return;
	m_nViewStyle = nViewStyle;

	EnsureCaretVisible();
	UpdateScrollBar();
}


// ShowColumnHeader
void MListCtrl::ShowColumnHeader( bool bShow)
{
	if ( m_bShowColumnHeader == bShow)		return;
	m_bShowColumnHeader = bShow;

	if ( m_nViewStyle == MLCVS_REPORT)
	{
		EnsureCaretVisible();
		UpdateScrollBar();
	}
}


// SetAlwaysSel
void MListCtrl::SetAlwaysSel( bool bSel)
{
	m_bAlwaysSel = bSel;
}


// GetAlwaysSel
bool MListCtrl::GetAlwaysSel() const
{
	return m_bAlwaysSel;
}


// SetItemMargin
void MListCtrl::SetItemMargin( int nHorzMargin, int nVertMargin)
{
	m_nHorzMargin = nHorzMargin;
	m_nVertMargin = nVertMargin;

	EnsureCaretVisible();
	UpdateScrollBar();
}


// GetItemMargin
void MListCtrl::GetItemMargin( int* pnHorzMargin, int* pnVertMargin)
{
	*pnHorzMargin = m_nHorzMargin;
	*pnVertMargin = m_nVertMargin;
}


// SetCurSel
int MListCtrl::SetCurSel( int nIndex)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= (int)_size)		nIndex = -1;

	ReleaseSelect();

	if ( nIndex < 0)	m_nCaret = -1;
	else
	{
		m_SelectedItems.insert( nIndex);
		m_nCaret = nIndex;
	}

	return m_nCaret;
}


// SetSel
int MListCtrl::SetSel( size_t nIndex, bool bSelect)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= (int)_size)		return -1;

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
int MListCtrl::GetSelItem( size_t nSelIndex) const
{
	size_t _size = m_SelectedItems.size();
	if ( nSelIndex >= _size)	return -1;

	set< size_t>::const_iterator itr = m_SelectedItems.begin();
	for ( size_t i = 0;  i != nSelIndex;  i++, itr++) ;
	return (*itr);
}


// ReleaseSelect
void MListCtrl::ReleaseSelect()
{
	m_SelectedItems.clear();
}


// SelItemRange
int MListCtrl::SelItemRange( size_t first, size_t last, bool bSelect)
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
void MListCtrl::SetCaretPos( int nPos)
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
void MListCtrl::EnableMultipleSel( bool bEnable)
{
	if ( m_bMultipleSel == bEnable)		return;

	m_bMultipleSel = bEnable;
	if ( m_bMultipleSel == false)		ReleaseSelect();
}


// ScrollPage
bool MListCtrl::ScrollPage( int delta)
{
	int nScrollMin, nScrollMax;
	m_pScrollBar->GetScrollRange( &nScrollMin, &nScrollMax);
	int nScrollVal = m_pScrollBar->GetScrollValue();
	if ( ( delta < 0  &&  nScrollVal <= nScrollMin)  ||  ( delta > 0  &&  nScrollVal >= nScrollMax))
		return false;


	float fDelta = (float)delta * 0.3f;
	if ( abs( (int)fDelta) < 1.0f)		fDelta = ( delta < 0.0f)  ?  -1.0f : 1.0f;

	int _value = m_pScrollBar->GetScrollValue() + (int)fDelta;
	m_pScrollBar->SetScrollValue( _value);

	return true;
}


// ItemFromPoint
int MListCtrl::ItemFromPoint( const MPOINT& pt)
{
	if ( m_ItemList.empty())			return -1;
	
	MPOINT _pt = pt;
	MRECT r = GetClientRect();
	if ( r.InPoint( _pt) == false)		return -1;

	int index = -1;
	switch ( m_nViewStyle)
	{
	case MLCVS_ICON :
		{
			MSIZE _Image_size = HasLookAndFill() ? ((MListCtrlLook*)m_pLookAndFill)->GetLargeImageSize() : MSIZE( 32, 32);
			MSIZE _item_size( _Image_size.w + (m_nHorzMargin << 1), GetItemHeight());
			int _item_count_per_line = (int)(GetClientRect().w / _item_size.w);
			int _width = _item_size.w * _item_count_per_line;
			if ( _pt.x > _width)		break;

			int x = (int)( _pt.x / _item_size.w);
			int y = (int)( _pt.y / _item_size.h);

			index = (y * _item_count_per_line + x) + (m_pScrollBar->GetScrollValue() * _item_count_per_line);
			if ( index >= (int)m_ItemList.size())	index = -1;
			break;
		}

	case MLCVS_LIST :
	case MLCVS_REPORT :
		{
			if ( m_nViewStyle == MLCVS_REPORT  &&  m_bShowColumnHeader == true)
				_pt.y -= GetColumnHeaderHeight();

			int _item_height = GetItemHeight();
			index = m_pScrollBar->GetScrollValue() + _pt.y / _item_height;
			if ( index >= (int)m_ItemList.size())	index = -1;
			break;
		}
	}

	return index;
}


// OnEvent
bool MListCtrl::OnEvent( MEvent* pEvent, MListener* pListener)
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
				if ( m_bAlwaysSel == true)		break;

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
						for ( size_t i = st;  i <= ed;  i++)
						{
							if ( m_ItemList[ i]->m_bEnable == true)
								m_SelectedItems.insert( i);
						}
					}
					else if ( m_ItemList[ m_nCaret]->m_bEnable == true)
						m_SelectedItems.insert( m_nCaret);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_nStartSel = m_nCaret;

					if ( m_bMultipleSel == true)
					{
						if ( m_ItemList[ m_nCaret]->m_bEnable == true)
						{
							if ( m_SelectedItems.find( m_nCaret) == m_SelectedItems.end())
								m_SelectedItems.insert( m_nCaret);
							else
								m_SelectedItems.erase( m_nCaret);
						}
					}
					else
					{
						ReleaseSelect();

						if ( m_ItemList[ m_nCaret]->m_bEnable == true)
							m_SelectedItems.insert( m_nCaret);
					}
				}
				else if ( GetSel( _sel) == true  &&  m_ItemList[ _sel]->m_bEnable == true)
				{
					m_nStartSel = m_nCaret;
					m_bReleaseSelOnLBtnUp = true;
				}
				else
				{
					m_nStartSel = m_nCaret;

					ReleaseSelect();

					if ( m_ItemList[ m_nCaret]->m_bEnable == true)
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
				if ( m_bAlwaysSel == true)		break;

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
				if ( m_bAlwaysSel == true)		break;

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
		{
			return ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		}

	case MWM_KEYDOWN :
		{
			bool bUpdate = false;
			bool bTranslate = false;

			if ( pEvent->nKey == VK_UP  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)							m_nCaret = 0;
				else if ( m_nViewStyle != MLCVS_ICON)		m_nCaret = max( 0, m_nCaret - 1);
				else
				{
					MSIZE _Image_size = HasLookAndFill() ? ((MListCtrlLook*)m_pLookAndFill)->GetLargeImageSize() : MSIZE( 32, 32);
					MSIZE _item_size( _Image_size.w + (m_nHorzMargin << 1), GetItemHeight());
					int _count = (int)( GetClientRect().w / _item_size.w);
					m_nCaret = max( 0, m_nCaret - _count);
				}
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_DOWN  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)							m_nCaret = 0;
				else if ( m_nViewStyle != MLCVS_ICON)		m_nCaret = min( (int)m_ItemList.size() - 1, m_nCaret + 1);
				else 
				{
					MSIZE _Image_size = HasLookAndFill() ? ((MListCtrlLook*)m_pLookAndFill)->GetLargeImageSize() : MSIZE( 32, 32);
					MSIZE _item_size( _Image_size.w + (m_nHorzMargin << 1), GetItemHeight());
					int _count = (int)( GetClientRect().w / _item_size.w);
					m_nCaret = min( (int)m_ItemList.size() - 1, m_nCaret + _count);
				}
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_LEFT  &&  m_nViewStyle == MLCVS_ICON  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)		m_nCaret = 0;
				else					m_nCaret = max( 0, m_nCaret - 1);
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_RIGHT  &&  m_nViewStyle == MLCVS_ICON  &&  m_ItemList.empty() == false)
			{
				if ( m_nCaret < 0)		m_nCaret = 0;
				else					m_nCaret = min( (int)m_ItemList.size() - 1, m_nCaret + 1);
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_PRIOR  &&  m_ItemList.empty() == false)
			{
				m_nCaret -= GetClientRect().h / GetItemHeight();
				if ( m_nCaret < 0)		m_nCaret = 0;
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_NEXT  &&  m_ItemList.empty() == false)
			{
				m_nCaret += GetClientRect().h / GetItemHeight();
				if ( m_nCaret >= (int)m_ItemList.size())	m_nCaret = m_ItemList.size() - 1;
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
						for ( size_t i = st;  i <= ed;  i++)
						{
							if ( m_ItemList[ i]->m_bEnable == true)
								m_SelectedItems.insert( i);
						}
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
void MListCtrl::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	// Ensure caret position
	if ( m_bEnsureCaretVisible == true)
	{
		m_bEnsureCaretVisible = false;

		if ( m_nCaret >= 0)
		{
			switch ( m_nViewStyle)
			{
			case MLCVS_ICON :
				{
					MSIZE _Image_size = HasLookAndFill() ? ((MListCtrlLook*)m_pLookAndFill)->GetLargeImageSize() : MSIZE( 32, 32);
					MSIZE _item_size( _Image_size.w + (m_nHorzMargin << 1), GetItemHeight());
					int _icon_count_per_line = (int)(GetClientRect().w / _item_size.w);

					int _start_line = m_pScrollBar->GetScrollValue();
					int _caret_line = m_nCaret / _icon_count_per_line;
					if ( _caret_line < _start_line)		m_pScrollBar->SetScrollValue( _caret_line);
					else
					{
						MRECT r = GetClientRect();
						int _view_line_count = r.h / _item_size.h;
						if ( (_caret_line - _start_line) >= _view_line_count)
							m_pScrollBar->SetScrollValue( _caret_line - _view_line_count + 1);
					}
					break;
				}

			case MLCVS_LIST :
			case MLCVS_REPORT :
				{
					MRECT r = GetClientRect();
					if ( m_nViewStyle == MLCVS_REPORT  &&  m_bShowColumnHeader == true)
						r.h -= GetColumnHeaderHeight();

					int _start = m_pScrollBar->GetScrollValue();
					if ( m_nCaret < _start)			m_pScrollBar->SetScrollValue( m_nCaret);
					else
					{
						int _view_count = r.h / GetItemHeight();
						if ( (m_nCaret - _start) >= _view_count)
							m_pScrollBar->SetScrollValue( m_nCaret - _view_count + 1);
					}
					break;
				}
			}
		}
	}


	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		m_bUpdateScrollBar = false;

		MRECT r = GetClientRect();

		switch ( m_nViewStyle)
		{
		case MLCVS_ICON :
			{
				MSIZE _Image_size = HasLookAndFill() ? ((MListCtrlLook*)m_pLookAndFill)->GetLargeImageSize() : MSIZE( 32, 32);
				MSIZE _item_size( _Image_size.w + (m_nHorzMargin << 1), GetItemHeight());
				int _line_count = (int)ceil( (double)m_ItemList.size() / (double)(r.w / _item_size.w));
				int _view_count = r.h / _item_size.h;
				if ( (int)m_ItemList.size() <= _line_count)
				{
					m_pScrollBar->SetScrollRange( 0, 0);
					m_pScrollBar->SetScrollValue( 0);
				}
				else
				{
					m_pScrollBar->SetScrollRange( 0, _line_count - _view_count);
				}
				break;
			}

		case MLCVS_LIST :
		case MLCVS_REPORT :
			{
				if ( m_nViewStyle == MLCVS_REPORT  &&  m_bShowColumnHeader == true)
					r.h -= GetColumnHeaderHeight();

				int _item_height = GetItemHeight();
				int _view_count = r.h / _item_height;
				if ( (int)m_ItemList.size() <= _view_count)
				{
					m_pScrollBar->SetScrollRange( 0, 0);
					m_pScrollBar->SetScrollValue( 0);
				}
				else
				{
					m_pScrollBar->SetScrollRange( 0, (int)m_ItemList.size() - _view_count);
				}
				break;
			}
		}
	}
}


// OnSize
void MListCtrl::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	EnsureCaretVisible();
	UpdateScrollBar();
}


// OnDragBegin
bool MListCtrl::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( GetCurSel() < 0)	return false;

	for ( set< size_t>::iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
	{
		int index = (*itr);
		MDragObject* pObject = new MDragObject( m_ItemList[ index]->m_strText, m_ItemList[ index]->m_pImage, m_ItemList[ index]->m_dwItemData);
		pDragData->AddObject( pObject);
	}

	if ( HasLookAndFill())
	{
		pDragData->SetFont( m_pLookAndFill->GetFont());
		switch ( m_nViewStyle)
		{
		case MLCVS_ICON :
			{
				MSIZE _size = ((MListCtrlLook*)m_pLookAndFill)->GetLargeImageSize();
				pDragData->SetImageSize( _size);
				pDragData->SetHotSpot( MPOINT( _size.w >> 1, _size.h >> 1));
				break;
			}

		case MLCVS_LIST :
		case MLCVS_REPORT :
			{
				MSIZE _size = ((MListCtrlLook*)m_pLookAndFill)->GetSmallImageSize();
				pDragData->SetImageSize( _size);
				pDragData->SetHotSpot( MPOINT( _size.w >> 1, _size.h >> 1));
				break;
			}
		}
	}

	m_bReleaseSelOnLBtnUp = false;

	return MWidget::OnDragBegin( pDragData, pt);
}


// OnToolTip
MWidget* MListCtrl::OnToolTip()
{
	if ( m_nMouseOver < 0  ||  m_nMouseOver >= (int)m_ItemList.size())
		return NULL;

	return MWidget::OnToolTip();
}


// OnLeave
void MListCtrl::OnLeave()
{
	if ( m_nMouseOver >= 0)		MGetMint()->GetToolTipManager()->Reset();
	m_nMouseOver = -1;

	MWidget::OnLeave();
}


// glueSetColumnAlign
void MListCtrl::glueSetColumnAlign( int nIndex, const char* szHor, const char* szVer)
{
	int nAlign = 0;

	string _hor( szHor);
	if ( _hor == "center")			nAlign |= MAM_HCENTER;
	else if ( _hor == "right")		nAlign |= MAM_RIGHT;
	else							nAlign |= MAM_LEFT;

	string _ver( szVer);
	if ( _ver == "center")			nAlign |= MAM_VCENTER;
	else if ( _ver == "bottom")		nAlign |= MAM_BOTTOM;
	else							nAlign |= MAM_TOP;

	SetColumnAlign( nIndex, nAlign);
}


// glueGetMouseOver
int MListCtrl::glueGetMouseOver()
{
	if ( m_nMouseOver >= (int)GetItemCount())		m_nMouseOver = -1;

	return m_nMouseOver;
}















// MListCtrlLook
MListCtrlLook::MListCtrlLook()
{
	SetWorkRectOffset( MRECT( 2, 2, -4, -4));
	SetTextAlign( MAM_LEFT | MAM_VCENTER);

	m_nItemHeight = 0;
	m_nColumnHeaderHeight = 0;
	m_LargeImageSize = MSIZE( 32, 32);
	m_SmallImageSize = MSIZE( 16, 16);
}

void MListCtrlLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MLookBase::OnDrawBorder( pWidget, pDC);
}

void MListCtrlLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawItems( (MListCtrl*)pWidget, pDC);
}

void MListCtrlLook::OnDrawItems( MListCtrl* pListCtrl, MDrawContext* pDC)
{
	MRECT r = pListCtrl->GetClientRect();

	// Draw items
	switch( pListCtrl->GetViewStyle())
	{
	case MLCVS_ICON:
		{
			int _hmargin, _vmargin;
			pListCtrl->GetItemMargin( &_hmargin, &_vmargin);
			int _item_height = GetItemHeight();
			MSIZE _item_size( m_LargeImageSize.w + (_hmargin << 1), _item_height);
			int _item_count_per_line = (int)( r.w / _item_size.w);
			int _line_width = _item_size.w * _item_count_per_line;
			int _start = pListCtrl->GetScrollBar()->GetScrollValue() * _item_count_per_line;

			int x = 0, y = 0;
			size_t _size = pListCtrl->GetItemCount();
			for ( size_t i = _start;  i < _size;  i++)
			{
				MRECT _r( x + _hmargin, y + _vmargin, m_LargeImageSize.w, _item_height - (_vmargin << 1));
				OnDrawItemBkgrnd( pListCtrl, i, 0, _r, pDC);

				if ( pListCtrl->GetSel( i) == true)
					OnDrawSelectbar( pListCtrl, i, MRECT( _r.x - 1, _r.y - 1, _r.w + 2, _r.h + 2), pDC);

				if ( pListCtrl->EventPreDrawItem( pDC, i, 0, _r) == false)
				{
					OnDrawImage( pListCtrl, i, _r, pDC);

					pListCtrl->EventDrawItem( pDC, i, 0, _r);
				}

				if ( pListCtrl->GetCaretPos() == i)
					OnDrawCaret( pListCtrl, i, MRECT( _r.x - 1, _r.y - 1, _r.w + 2, _r.h + 2), pDC);


				x += _item_size.w;
				if ( x >= _line_width)
				{
					x = 0;
					y += _item_size.h;
					if ( y > r.h)	break;
				}
			}
			break;
		}

	case MLCVS_LIST:
		{
			int _hmargin, _vmargin;
			pListCtrl->GetItemMargin( &_hmargin, &_vmargin);
			int _item_height = pListCtrl->GetItemHeight();

			int x = 0, y = 0;
			size_t _size = pListCtrl->GetItemCount();
			for ( size_t i = pListCtrl->GetScrollBar()->GetScrollValue();  i < _size;  i++)
			{
				MRECT _r( x + _hmargin, y + _vmargin, r.w - (_hmargin << 1), _item_height - (_vmargin << 1));

				OnDrawItemBkgrnd( pListCtrl, i, 0, _r, pDC);

				if ( pListCtrl->GetSel( i) == true)
					OnDrawSelectbar( pListCtrl, i, MRECT( x, y, r.w, _item_height), pDC);

				if ( pListCtrl->EventPreDrawItem( pDC, i, 0, _r) == false)
				{
					if ( pListCtrl->GetItemImage( i) != NULL)
					{
						OnDrawImage( pListCtrl, i, MRECT( _r.x, _r.y + ((_r.h > 0) ? ((_r.h - m_SmallImageSize.h) >> 1) : 0), m_SmallImageSize.w, m_SmallImageSize.h), pDC);
						OnDrawItemText( pListCtrl, i, 0, MRECT( _r.x + m_SmallImageSize.w, _r.y, _r.w - m_SmallImageSize.w, _r.h), GetTextAlign(), pDC);
					}
					else
						OnDrawItemText( pListCtrl, i, 0, _r, GetTextAlign(), pDC);

					pListCtrl->EventDrawItem( pDC, i, 0, _r);
				}

				if ( pListCtrl->GetCaretPos() == i)
					OnDrawCaret( pListCtrl, i, MRECT( x, y, r.w, _item_height), pDC);


				y += _item_height;
				if ( y >= r.h)			break;
			}
			break;
		}

	case MLCVS_REPORT:
		{
			int _hmargin, _vmargin;
			pListCtrl->GetItemMargin( &_hmargin, &_vmargin);
			int _item_height = pListCtrl->GetItemHeight();

			int y = 0;
			if ( pListCtrl->IsShowColumnHeader() == true)
			{
				int _column_height = ( pListCtrl->GetViewStyle() == MLCVS_REPORT  &&  pListCtrl->IsShowColumnHeader() == true) ? pListCtrl->GetColumnHeaderHeight() : 0;
				OnDrawColumnHeader( pListCtrl, MRECT( 0, y, r.w, _column_height), pDC);
				y += _column_height + 2;
			}

			size_t _column_count = pListCtrl->GetColumnCount();
			size_t _size = pListCtrl->GetItemCount();
			for ( size_t i = pListCtrl->GetScrollBar()->GetScrollValue();  i < _size;  i++)
			{
				if ( pListCtrl->GetSel( i) == true)
					OnDrawSelectbar( pListCtrl, i, MRECT( 0, y, r.w, _item_height), pDC);

				int x = 0;
				for ( size_t j = 0;  j < _column_count;  j++)
				{
					int _w = pListCtrl->GetColumn( j)->GetWidth();
					MRECT _r( x + _hmargin, y + _vmargin, _w - (_hmargin << 1), _item_height - (_vmargin << 1));

					OnDrawItemBkgrnd( pListCtrl, i, j, _r, pDC);
					
					if ( pListCtrl->EventPreDrawItem( pDC, i, j, _r) == false)
					{
						if ( j == 0  &&  pListCtrl->GetItemImage( i) != NULL)
						{
							OnDrawImage( pListCtrl, i, MRECT( _r.x, _r.y + ((_r.h > 0) ? ((_r.h - m_SmallImageSize.h) >> 1) : 0), m_SmallImageSize.w, m_SmallImageSize.h), pDC);
							OnDrawItemText( pListCtrl, i, j, MRECT( _r.x + m_SmallImageSize.w, _r.y, _r.w - m_SmallImageSize.w, _r.h), GetTextAlign(), pDC);
						}
						else
							OnDrawItemText( pListCtrl, i, j, _r, pListCtrl->GetColumn( j)->GetAlign(), pDC);

						pListCtrl->EventDrawItem( pDC, i, j, _r);
					}

					x += _w;
				}

				if ( pListCtrl->GetCaretPos() == i)
					OnDrawCaret( pListCtrl, i, MRECT( 0, y, r.w, _item_height), pDC);


				y += _item_height;
				if ( y >= r.h)			break;
			}

			break;
		}
	}
}

void MListCtrlLook::OnDrawItemText( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, int align, MDrawContext* pDC)
{
	MCOLOR _color;
	bool bHasColor = pListCtrl->GetItemColor( index, subitem, _color);

	if ( pListCtrl->GetItemEnable( index) == false)
		pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DISABLE, MColorTable::TEXT[ MWSN_DISABLE]));

	else if ( bHasColor == true)
		pDC->SetColor( _color);

	else if ( pListCtrl->GetSel( index) == true)
		pDC->SetColor( GetFontColor( MWP_SELECTBAR, pListCtrl->GetState(), MColorTable::TEXT_SELECTED));

	else
	{
		if ( pListCtrl->IsEnabled() == false)
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DISABLE, MColorTable::TEXT[ MWSN_DISABLE]));

		else if ( pListCtrl->GetCaretPos() == index)
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MColorTable::TEXT[ MWSN_FOCUS]));

		else
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]));

	}

	pDC->SetFont( GetFont());
	pDC->Text( r, pListCtrl->GetItemText( index, subitem), align);
}

void MListCtrlLook::OnDrawColumnHeader( MListCtrl* pListCtrl, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_COLUMNHEADER, pListCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::TITLEBAR[ pListCtrl->GetStateNum()]);
		pDC->FillRectangle( r.x + 1, r.y + 1, r.w - 1, r.h - 1);
	}


	// Draw text
	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( MWP_COLUMNHEADER, pListCtrl->GetState(), MColorTable::TITLE[ pListCtrl->GetStateNum()]));

	int x = 0;
	size_t _size = pListCtrl->GetColumnCount();
	for ( size_t i = 0;  i < _size;  i++)
	{
		MLCCOLUMN* pColumn = pListCtrl->GetColumn( i);
		pDC->Text( MRECT( x, 0, pColumn->GetWidth(), r.h), pColumn->GetText(), MAM_HCENTER | MAM_VCENTER);
		x += pColumn->GetWidth();
	}


	// Draw separate bar
	MCOLOR _color = pDC->GetColor();
	_color.a = _color.a >> 2;
	pDC->SetColor( _color);
	x = 0;
	for ( size_t i = 0;  i < _size;  i++)
	{
		x += pListCtrl->GetColumn( i)->GetWidth();
		if ( x >= r.w)		break;
		pDC->Line( x, r.y + 3, x, r.y + r.h - 4);
	}
}

void MListCtrlLook::OnDrawItemBkgrnd( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, MDrawContext* pDC)
{
	Draw( pDC, r, MWP_ITEMBKGRND, pListCtrl->GetState());

	pListCtrl->EventDrawItemBkgrnd( pDC, index, subitem, r);
}

void MListCtrlLook::OnDrawCaret( MListCtrl* pListCtrl, size_t index, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_CARET, pListCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::BORDER[ pListCtrl->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MListCtrlLook::OnDrawSelectbar( MListCtrl* pListCtrl, size_t index, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_SELECTBAR, pListCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::SELECTBAR[ pListCtrl->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}


void MListCtrlLook::OnDrawImage( MListCtrl* pListCtrl, size_t index, MRECT& r, MDrawContext* pDC)
{
	MBitmap* pBitmap = pListCtrl->GetItemImage( index);
	if ( pBitmap != NULL)
	{
		pDC->SetBitmap( pBitmap);

		if ( pListCtrl->GetItemEnable( index) == false)
		{
			MCOLOR _col = pDC->SetBitmapColor( 100, 100, 100);
			pDC->Draw( r);
			pDC->SetBitmapColor( _col);
		}
		else
			pDC->Draw( r);
	}
}


} // namespace mint3


