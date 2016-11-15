#include "stdafx.h"
#include "MCategoryCtrl.h"
#include "MScrollBar.h"
#include "Mint.h"
#include "MColorTable.h"



namespace mint3
{


#define DEFAULT_ITEMHEIGHT			17


enum
{
	MCCHT_NONE		= 0,
	MCCHT_EXPAND,
	MCCHT_BKGRND
};





// MLCITEM
MCCITEM::MCCITEM( size_t nCol, const char* szText, MBitmap* pImage)
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
			MCCSUBITEM* pSubItem = new MCCSUBITEM;
			m_SubItemList.push_back( pSubItem);
		}
	}
}

MCCITEM::~MCCITEM()
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

void MCCITEM::_set_item_text( size_t nSubItem, const char* szText)
{
	if ( nSubItem == 0)		m_strText = szText;
	else					m_SubItemList[ nSubItem - 1]->m_strText = szText;
}

const char* MCCITEM::_get_item_text( size_t nSubItem)
{
	if ( nSubItem == 0)		return m_strText.c_str();
	return m_SubItemList[ nSubItem - 1]->m_strText.c_str();
}

void MCCITEM::_set_item_color( size_t nSubItem, MCOLOR& nColor)
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

bool MCCITEM::_get_item_color( size_t nSubItem, MCOLOR& nColor)
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





// MCCCATEGORY
MCCCATEGORY::MCCCATEGORY( const char* szText)
{
	m_strText = szText;
	m_bExpand = false;
}

MCCCATEGORY::~MCCCATEGORY()
{
	if ( m_Items.empty() == false)
	{
		for ( vector< MCCITEM*>::iterator itr = m_Items.begin();  itr != m_Items.end();  itr++)
			delete (*itr);

		m_Items.clear();
	}
}

int MCCCATEGORY::_add_item( MCCITEM* pItem)
{
	if ( pItem == NULL)		return -1;

	m_Items.push_back( pItem);
	return ( m_Items.size() - 1);
}

int MCCCATEGORY::_insert_item( size_t nIndex, MCCITEM* pItem)
{
	if ( pItem == NULL)		return -1;

	size_t _size = m_Items.size();
	if ( nIndex >= _size)	return _add_item( pItem);


	vector< MCCITEM*>::iterator itr = m_Items.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++) ;

	m_Items.insert( itr, pItem);
	return ( nIndex);
}

bool MCCCATEGORY::_delete_item( size_t nIndex)
{
	if ( nIndex >= m_Items.size())		return false;

	vector< MCCITEM*>::iterator itr = m_Items.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++) ;

	delete (*itr);
	m_Items.erase( itr);

	return true;
}











// MCategoryCtrl
MCategoryCtrl::MCategoryCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MCategoryCtrlLook);

	SetEnableFocus( true);

	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_Caret.clear();
	m_StartSel.clear();
	m_MouseOver.clear();
	m_bDragAndDrop = false;
	m_bMultipleSel = false;
	m_bEnsureCaretVisible = false;
	m_bUpdateScrollBar = false;
	m_bReleaseSelOnLBtnUp = false;
}


// ~MCategoryCtrl
MCategoryCtrl::~MCategoryCtrl()
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


// AddCategory
int MCategoryCtrl::AddCategory( const char* szText)
{
	MCCCATEGORY* pCategory = new MCCCATEGORY( szText);
	m_Categories.push_back( pCategory);

	ReleaseSelect();
	UpdateScrollBar();

	m_Caret.clear();
	m_StartSel.clear();
	m_MouseOver.clear();

	return (int)(m_Categories.size() - 1);
}


// InsertCategory
int MCategoryCtrl::InsertCategory( size_t nCategory, const char* szText)
{
	size_t _size = m_Categories.size();
	if ( nCategory >= _size)	return AddCategory( szText);

	vector< MCCCATEGORY*>::iterator itr = m_Categories.begin();
	for ( size_t i = 0;  i < _size;  i++, itr++) ;

	MCCCATEGORY* pCategory = new MCCCATEGORY( szText);
	m_Categories.insert( itr, pCategory);

	ReleaseSelect();
	UpdateScrollBar();

	m_Caret.clear();
	m_StartSel.clear();
	m_MouseOver.clear();

	return nCategory;
}


// DeleteCategory
bool MCategoryCtrl::DeleteCategory( size_t nCategory)
{
	size_t _size = m_Categories.size();
	if ( nCategory >= _size)	return false;

	ReleaseSelect();

	vector< MCCCATEGORY*>::iterator itr = m_Categories.begin();
	for ( size_t i = 0;  i != nCategory;  i++, itr++) ;

	delete (*itr);
	m_Categories.erase( itr);

	ReleaseSelect();
	EnsureCaretVisible();
	UpdateScrollBar();

	m_Caret.clear();
	m_StartSel.clear();
	m_MouseOver.clear();

	return true;
}


// AddItem
int MCategoryCtrl::AddItem( size_t nCategory, const char* szText, const char* szImage)
{
	MCCITEM* pItem = new MCCITEM( m_ColumnList.size(), szText, MBitmapManager::Get( szImage));
	int _index = m_Categories[ nCategory]->_add_item( pItem);

	if ( _index >= 0)
	{
		ReleaseSelect();
		UpdateScrollBar();

		m_Caret.clear();
		m_StartSel.clear();
		m_MouseOver.clear();
	}

	return (int)_index;
}


// InsertItem
int MCategoryCtrl::InsertItem( size_t nCategory, size_t nItem, const char* szText, const char* szImage)
{
	MCCITEM* pItem = new MCCITEM( m_ColumnList.size(), szText, MBitmapManager::Get( szImage));
	int _index = m_Categories[ nCategory]->_insert_item( nItem, pItem);

	if ( _index >= 0)
	{
		ReleaseSelect();
		UpdateScrollBar();

		m_Caret.clear();
		m_StartSel.clear();
		m_MouseOver.clear();
	}
	return _index;
}


// DeleteItem
bool MCategoryCtrl::DeleteItem( size_t nCategory, size_t nItem)
{
	bool _result = m_Categories[ nCategory]->_delete_item( nItem);

	if ( _result == true)
	{
		ReleaseSelect();
		EnsureCaretVisible();
		UpdateScrollBar();

		m_Caret.clear();
		m_StartSel.clear();
		m_MouseOver.clear();
	}
	return _result;
}


// DeleteAllItems
void MCategoryCtrl::DeleteAllItems()
{
	if ( m_Categories.empty() == false)
	{
		for ( vector< MCCCATEGORY*>::iterator itr = m_Categories.begin();  itr != m_Categories.end();  itr++)
			delete (*itr);

		m_Categories.clear();
	}

	ReleaseSelect();
	EnsureCaretVisible();
	UpdateScrollBar();

	m_Caret.clear();
	m_StartSel.clear();
	m_MouseOver.clear();
}


// AddColumn
int MCategoryCtrl::AddColumn( const char* szText, int nAlign, int nWidth)
{
	DeleteAllItems();

	MCCCOLUMN* pColumn = new MCCCOLUMN( szText, nAlign, nWidth);
	m_ColumnList.push_back( pColumn);
	return (int)m_ColumnList.size();
}


// DeleteColumn
bool MCategoryCtrl::DeleteColumn( size_t nCol)
{
	if ( nCol == 0)						return false;
	if ( nCol >= m_ColumnList.size())	return false;

	DeleteAllItems();

	vector< MCCCOLUMN*>::iterator itr = m_ColumnList.begin();
	for ( size_t i = 0;  i != nCol;  i++, itr++) ;

	delete m_ColumnList[ nCol];
	m_ColumnList.erase( itr);

	return true;
}


// SetCategoryExpand
void MCategoryCtrl::SetCategoryExpand( size_t nCategory, bool bExpand)
{
	m_Categories[ nCategory]->_set_expand( bExpand);

	if ( bExpand == false)
	{
		for ( vector< MCCINDEX>::iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  )
		{
			if ( (*itr).nCategory == nCategory  &&  (*itr).nItem >= 0)
				itr = m_SelectedItems.erase( itr);
			else
				itr++;
		}

		if ( m_Caret.nCategory == nCategory)
		{
			m_Caret.nItem = -1;

			SetSel( m_Caret.nCategory, m_Caret.nItem, true);
		}
	}

	EnsureCaretVisible();
	UpdateScrollBar();
}


// SetItemEnable
void MCategoryCtrl::SetItemEnable( size_t nCategory, size_t nItem, bool bEnable)
{
	if ( nItem < 0)				return;

	m_Categories[ nCategory]->m_Items[ nItem]->m_bEnable = bEnable;

	if ( bEnable == false)		SetSel( nCategory, nItem, false);
}


// GetItemEnable
bool MCategoryCtrl::GetItemEnable( size_t nCategory, size_t nItem) const
{
	if ( nItem < 0)				return false;
	if ( nItem == -1)			return true;

	return m_Categories[ nCategory]->m_Items[ nItem]->m_bEnable;
}


// GetCategoryHeight
int MCategoryCtrl::GetCategoryHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MCategoryCtrlLook*)m_pLookAndFill)->GetCategoryHeight();
		if ( height <= 0)
			height = m_pLookAndFill->GetFont()->GetHeight() + 2;
	}
	if ( height <= 0)	height = DEFAULT_ITEMHEIGHT;

	return height;
}


// GetItemHeight
int MCategoryCtrl::GetItemHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MCategoryCtrlLook*)m_pLookAndFill)->GetItemHeight();
		if ( height <= 0)
			height = m_pLookAndFill->GetFont()->GetHeight() + 2;
	}
	if ( height <= 0)	height = DEFAULT_ITEMHEIGHT;

	return height;
}


// SetCurSel
MCCINDEX MCategoryCtrl::SetCurSel( int nCategory, int nItem)
{
	ReleaseSelect();

	m_Caret.clear();
	m_StartSel.clear();


	if ( nCategory >= 0  &&  nItem >= -1)
	{
		nCategory = min( nCategory, (int)m_Categories.size() - 1);
		if ( nCategory >= 0)
		{
			nItem = min( nItem, (int)m_Categories[ nCategory]->_get_count() - 1);
			if ( nItem >= 0)
			{
				m_Caret = MCCINDEX( nCategory, nItem);
				m_StartSel = m_Caret;

				SetSel( m_Caret.nCategory, m_Caret.nItem, true);
			}
		}
	}
 
 	return m_Caret;
}


// GetCurSel
MCCINDEX MCategoryCtrl::GetCurSel() const
{
	MCCINDEX _sel;

	if ( m_SelectedItems.empty() == false)		_sel = (*m_SelectedItems.begin());
	return _sel;
}


// SetSel
MCCINDEX MCategoryCtrl::SetSel( int nCategory, int nItem, bool bSelect)
{
	MCCINDEX _sel;

	if ( nCategory >= 0  &&  nItem >= -1)
	{
		_sel = MCCINDEX( nCategory, nItem);

		bool bExist = false;
		vector< MCCINDEX>::iterator itr;
		for ( itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
		{
			if ( (*itr) == _sel)
			{
				bExist = true;
				break;
			}
		}

		if ( bSelect == true  &&  bExist == false)
		{
			if ( _sel.nItem == -1  ||  (_sel.nItem >= 0  &&  GetItemEnable( nCategory, nItem) == true))
			{
				if ( m_bMultipleSel == false)	ReleaseSelect();

				m_SelectedItems.push_back( _sel);
			}

		}
		else if ( bSelect == false  &&  bExist == true)
		{
			m_SelectedItems.erase( itr);

			if ( m_Caret == _sel)			m_Caret.clear();
			if ( m_StartSel == _sel)		m_StartSel.clear();
			if ( m_MouseOver == _sel)		m_MouseOver.clear();
		}
	}

	return _sel;
}


// GetSel
bool MCategoryCtrl::GetSel( int nCategory, int nItem)
{
	for ( vector< MCCINDEX>::iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
	{
		if ( (*itr).nCategory == nCategory  &&  (*itr).nItem == nItem)
			return true;
	}

	return false;
}


// GetSelItem
MCCINDEX MCategoryCtrl::GetSelItem( size_t nSelIndex) const
{
	return m_SelectedItems[ nSelIndex];
}


// SetCaretPos
void MCategoryCtrl::SetCaretPos( int nCategory, int nItem)
{
	if ( nCategory < 0  ||  nItem < 0)
	{
		m_Caret.clear();
		m_StartSel.clear();
	}

	else
	{
		int _size = m_Categories.size();
		if ( nCategory >= _size)		nCategory = _size - 1;

		_size = m_Categories[ nCategory]->_get_count();
		if ( nItem >= _size)			nItem = _size - 1;

		m_Caret = MCCINDEX( nCategory, nItem);
		m_StartSel = m_Caret;
	}
}


// EnableMultipleSel
void MCategoryCtrl::EnableMultipleSel( bool bEnable)
{
	if ( m_bMultipleSel == bEnable)		return;

	m_bMultipleSel = bEnable;
	if ( m_bMultipleSel == false)		ReleaseSelect();
}


// ScrollPage
void MCategoryCtrl::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);
}


// ItemFromPoint
MCCINDEX MCategoryCtrl::ItemFromPoint( const MPOINT& pt, int* pHitRegion)
{
	MPOINT _pt = pt;

	if ( pHitRegion != NULL)				*pHitRegion = MCCHT_NONE;

	size_t _size_category = m_Categories.size();
	if ( _size_category == 0)				return MCCINDEX( -1, -1);

	MRECT r = GetClientRect();
	if ( r.InPoint( _pt) == false)			return MCCINDEX( -1, -1);

	int _h_category = GetCategoryHeight();
	int _h_item = GetItemHeight();

	// Find first position
	MCCINDEX _pos;
	int _scroll = m_pScrollBar->GetScrollValue() + 1;
	for ( size_t _cat = 0;  _cat < _size_category;  _cat++)
	{
		_pos.nCategory++;
		_pos.nItem = -1;
		_scroll--;
		if ( _scroll <= 0)					break;
		if ( m_Categories[ _cat]->_get_expand() == false)
			continue;

		int _items = min( (int)m_Categories[ _cat]->_get_count(), _scroll);
		_pos.nItem += _items;
		_scroll -= _items;
		if ( _scroll <= 0)					break;
	}

	if ( _pos.empty() == true)				return MCCINDEX( -1, -1);

	
	// Find selected item
	_pt.y -= r.y;
	for ( size_t _cat = _pos.nCategory;  _cat < _size_category;  _cat++)
	{
		int _size_item = m_Categories[ _cat]->_get_expand()  ?  m_Categories[ _cat]->_get_count() : 0;

		for ( int _item = _pos.nItem;  _item < _size_item;  _item++)
		{
			// Category
			if ( _item == -1)
			{
				if ( _pt.y < _h_category)
				{
					if ( pHitRegion != NULL)
					{
						MSIZE _size_image = HasLookAndFill()  ?  ((MCategoryCtrlLook*)GetLookAndFill())->GetExpandIconSize() : MSIZE( 0, 0);
						MRECT _r( r.x + 3, 0 + (( _h_category - _size_image.h) >> 1), _size_image.w, _size_image.h);
						if ( _r.InPoint( _pt) == true)		*pHitRegion = MCCHT_EXPAND;
						else								*pHitRegion = MCCHT_BKGRND;
					}

					return MCCINDEX( _cat, _item);
				}

				_pt.y -= _h_category;
			}

			// Items
			else
			{
				if ( _pt.y < _h_item)
				{
					if ( pHitRegion != NULL)				*pHitRegion = MCCHT_BKGRND;

					return MCCINDEX( _cat, _item);
				}

				_pt.y -= _h_item;
			}
		}

		_pos.nItem = -1;
	}

	return MCCINDEX( -1, -1);
}


// OnEvent
bool MCategoryCtrl::OnEvent( MEvent* pEvent, MListener* pListener)
{
#define GET_RANGE(s,e,_s,_e)		{	if (_s.nCategory > _e.nCategory)				{s=_e; e=_s;}		\
										else if (_s.nCategory < _e.nCategory)			{s=_s; e=_e;}		\
										else if	(_s.nItem > _e.nItem)					{s=_e; e=_s;}		\
										else											{s=_s; e=_e;}		\
									}


	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	// Translate message
	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		{
			int _hit_region;
			MCCINDEX _sel = ItemFromPoint( pEvent->Pos, &_hit_region);

			if ( _sel.empty() == true)
			{
				ReleaseSelect();

				m_Caret.clear();
				m_StartSel = m_Caret;
			}
			else
			{
				// expand/collapse 영역인지 확인
				if ( _hit_region == MCCHT_EXPAND)
				{
					SetCategoryExpand( _sel.nCategory, !GetCategoryExpand( _sel.nCategory));
					break;
				}


				m_Caret = _sel;
				if ( m_StartSel.empty() == true)	m_StartSel = _sel;

				if ( pEvent->bShift == true)
				{
					ReleaseSelect();

					if ( m_bMultipleSel == true)
					{
						MCCINDEX st, ed;
						GET_RANGE( st, ed, m_Caret, m_StartSel);
						int _size_category = m_Categories.size();
						for ( st.nCategory;  st.nCategory < _size_category;  st.nCategory++)
						{
							int _size_item = ( m_Categories[ st.nCategory]->_get_expand() == true)  ?
								m_Categories[ st.nCategory]->_get_count() : 0;

							for ( st.nItem;  st.nItem < _size_item;  st.nItem++)
							{
								SetSel( st.nCategory, st.nItem, true);

								if ( st == ed)		break;
							}

							if ( st == ed)			break;
							st.nItem = -1;
						}
					}
					else
						SetSel( m_Caret.nCategory, m_Caret.nItem, true);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_StartSel = m_Caret;

					if ( m_bMultipleSel == true)
					{
						if ( GetSel( m_Caret.nCategory, m_Caret.nItem) == false)
							SetSel( m_Caret.nCategory, m_Caret.nItem, true);
						else
							SetSel( m_Caret.nCategory, m_Caret.nItem, false);
					}
					else
					{
						ReleaseSelect();
						SetSel( m_Caret.nCategory, m_Caret.nItem, true);
					}
				}
				else if ( GetSel( _sel.nCategory, _sel.nItem) == true  &&  GetItemEnable( _sel.nCategory, _sel.nItem) == true)
				{
					m_StartSel = m_Caret;
					m_bReleaseSelOnLBtnUp = true;
				}
				else
				{
					m_StartSel = m_Caret;

					ReleaseSelect();
					SetSel( m_Caret.nCategory, m_Caret.nItem, true);
				}
			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemClick( pEvent, -1);
			break;
		}

	case MWM_LBUTTONUP :
		{
			if ( m_bReleaseSelOnLBtnUp == true)
			{
				ReleaseSelect();
				SetSel( m_Caret.nCategory, m_Caret.nItem, true);
				m_bReleaseSelOnLBtnUp = false;
			}
			break;
		}

	case MWM_LBUTTONDBLCLK :
		{
			ReleaseSelect();

			int _hit_region;
			MCCINDEX _sel = ItemFromPoint( pEvent->Pos, &_hit_region);
			if ( _sel.empty() == true)
			{
				m_Caret.clear();
				m_StartSel = m_Caret;
			}
			else
			{
				if ( _hit_region == MCCHT_EXPAND)	break;

				m_Caret = _sel;
				m_StartSel = m_Caret;
				SetSel( m_Caret.nCategory, m_Caret.nItem, true);
			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemDblClick( pEvent, -1);
			break;
		}

	case MWM_RBUTTONDOWN :
		{
			ReleaseSelect();

			MCCINDEX _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel.empty() == true)
			{
				m_Caret.clear();
				m_StartSel = m_Caret;
			}
			else
			{
				m_Caret = _sel;
				m_StartSel = m_Caret;
				SetSel( m_Caret.nCategory, m_Caret.nItem, true);
			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemRClick( pEvent, -1);
			break;
		}

	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;

	case MWM_KEYDOWN :
		{
			bool bUpdate = false;
			bool bTranslate = false;

			if ( pEvent->nKey == VK_UP  &&  m_Categories.empty() == false)
			{
				if ( m_Caret.empty() == true)			m_Caret = MCCINDEX( 0, -1);
				else if ( m_Caret.nItem > -1)			m_Caret.nItem--;
				else
				{
					if ( m_Caret.nCategory > 0)
					{
						m_Caret.nCategory--;

						int _size_item = (m_Categories[ m_Caret.nCategory]->_get_expand() == true)  ?
							m_Categories[ m_Caret.nCategory]->_get_count() : 0;

						m_Caret.nItem = _size_item - 1;
					}
				}
				bUpdate = true;
				bTranslate = true;
			}
			else if ( pEvent->nKey == VK_DOWN  &&  m_Categories.empty() == false)
			{
				if ( m_Caret.empty() == true)			m_Caret = MCCINDEX( 0, -1);
				else
				{
					int _size_item = (m_Categories[ m_Caret.nCategory]->_get_expand() == true)  ?
						m_Categories[ m_Caret.nCategory]->_get_count() : 0;

					if ( (m_Caret.nItem + 1) < _size_item)
						m_Caret.nItem++;
					else if ( (m_Caret.nCategory + 1) < (int)m_Categories.size())
					{
						m_Caret.nCategory++;
						m_Caret.nItem = -1;
					}
				}
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_PRIOR  &&  m_Categories.empty() == false)
			{
				if ( m_Caret.empty() == true)			m_Caret = MCCINDEX( 0, -1);
				else
				{
					int _prev = max( 1, GetClientRect().h / GetItemHeight() - 1);
					int _size_category = m_Categories.size();
					for ( int _cat = m_Caret.nCategory;  _cat >= 0;  _cat--)
					{
						int _size_item = (m_Categories[ _cat]->_get_expand() == true)  ?  m_Categories[ _cat]->_get_count() : 0;
						for ( int _item = m_Caret.nItem;  _item >= -1;  _item--)
						{
							m_Caret.nCategory = _cat;
							m_Caret.nItem = _item;

							_prev--;
							if ( _prev < 0)	break;
						}
						if ( _prev < 0)		break;
						m_Caret.nItem = -1;
					}
				}
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_NEXT  &&  m_Categories.empty() == false)
			{
				if ( m_Caret.empty() == true)			m_Caret = MCCINDEX( 0, -1);
				else
				{
					int _next = max( 1, GetClientRect().h / GetItemHeight() - 1);
					size_t _size_category = m_Categories.size();
					for ( size_t _cat = m_Caret.nCategory;  _cat < _size_category;  _cat++)
					{
						int _size_item = (m_Categories[ _cat]->_get_expand() == true)  ?  m_Categories[ _cat]->_get_count() : 0;
						for ( int _item = m_Caret.nItem;  _item < _size_item;  _item++)
						{
							m_Caret.nCategory = _cat;
							m_Caret.nItem = _item;

							_next--;
							if ( _next < 0)	break;
						}
						if ( _next < 0)		break;
						if ( (m_Caret.nCategory + 1) < (int)_size_category)
							m_Caret.nItem = -1;
					}
				}
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_HOME  &&  m_Categories.empty() == false)
			{
				m_Caret = MCCINDEX( 0, -1);
				bUpdate = true;
				bTranslate = true;
			}

			else if ( pEvent->nKey == VK_END  &&  m_Categories.empty() == false)
			{
				size_t _category = m_Categories.size() - 1;
				size_t _item = (m_Categories[ _category]->_get_expand() == true)  ?  m_Categories[ _category]->_get_count() : 0;
				m_Caret = MCCINDEX( _category, _item - 1);
				bUpdate = true;
				bTranslate = true;
			}

			// Select all
			else if ( pEvent->nKey == 'A'  &&  pEvent->bCtrl == true  &&  m_Categories.empty() == false)
			{
				size_t _category = m_Categories.size() - 1;
				size_t _item = (m_Categories[ _category]->_get_expand() == true)  ?  m_Categories[ _category]->_get_count() : 0;
				m_Caret = MCCINDEX( _category, _item - 1);
 				m_StartSel = m_Caret;
 
 				if ( m_bMultipleSel == true  &&  m_Categories.empty() == false)
 				{
					MCCINDEX st( 0, -1);
					int _size_category = m_Categories.size();
					for ( st.nCategory;  st.nCategory < _size_category;  st.nCategory++)
					{
						int _size_item = ( m_Categories[ st.nCategory]->_get_expand() == true)  ?
							m_Categories[ st.nCategory]->_get_count() : 0;

						for ( st.nItem;  st.nItem < _size_item;  st.nItem++)
						{
							SetSel( st.nCategory, st.nItem, true);

							if ( st == m_Caret)		break;
						}

						if ( st == m_Caret)			break;
						st.nItem = -1;
					}
 				}
 				else
					SetSel( m_Caret.nCategory, m_Caret.nItem, true);
 
 				EnsureCaretVisible();
 				bTranslate = true;
			}

			// Copy
			else if ( pEvent->nKey == 'C'  &&  pEvent->bCtrl == true  &&  m_SelectedItems.empty() == false)
			{
 				string strString;
 				for ( vector< MCCINDEX>::const_iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
 				{
 					if ( strString.empty() == false)	strString += ",";
					if ( (*itr).nItem == -1)			strString += m_Categories[ (*itr).nCategory]->_get_text();
					else								strString += m_Categories[ (*itr).nCategory]->m_Items[ (*itr).nItem]->_get_item_text( 0);
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
						MCCINDEX st, ed;
						GET_RANGE( st, ed, m_Caret, m_StartSel);
						int _size_category = m_Categories.size();
						for ( st.nCategory;  st.nCategory < _size_category;  st.nCategory++)
						{
							int _size_item = ( m_Categories[ st.nCategory]->_get_expand() == true)  ?
								m_Categories[ st.nCategory]->_get_count() : 0;

							for ( st.nItem;  st.nItem < _size_item;  st.nItem++)
							{
								SetSel( st.nCategory, st.nItem, true);

								if ( st == ed)		break;
							}

							if ( st == ed)			break;
							st.nItem = -1;
						}
					}
					else
						SetSel( m_Caret.nCategory, m_Caret.nItem, true);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_StartSel = m_Caret;
				}
				else
				{
					m_StartSel = m_Caret;
					ReleaseSelect();
					SetSel( m_Caret.nCategory, m_Caret.nItem, true);
				}

				EnsureCaretVisible();

				OnSelChanged();
			}

			if ( bTranslate == true)		return true;
		}

	case MWM_MOUSEMOVE :
		{
			MCCINDEX _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel != m_MouseOver)
			{
				m_MouseOver = _sel;
				MGetMint()->ChangedToolTipText( this);
			}
		}

	}

	return false;
}


// OnRun
void MCategoryCtrl::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		MRECT r = GetClientRect();
		size_t _size = 0;
		int _h = 0;
		int _h_category = GetCategoryHeight();
		int _h_Item = GetItemHeight();

		size_t _size_category = m_Categories.size();
		for ( size_t _cat = 0;  _cat < _size_category;  _cat++)
		{
			_size++;
			_h += _h_category;

			if ( m_Categories[ _cat]->_get_expand() == true)
			{
				size_t _items = m_Categories[ _cat]->_get_count();
				_size += _items;
				_h += _h_Item * _items;
			}
		}


		if ( _h < r.h  ||  _size_category == 0)
			m_pScrollBar->SetScrollRange( 0, 0);

		else
		{
			_h = 0;
			for ( size_t _cat = (_size_category - 1);  _cat >= 0;  _cat--)
			{
				size_t _items = ( m_Categories[ _cat]->_get_expand() == true)  ?  m_Categories[ _cat]->_get_count() : 0;
				int _h_items = _h_Item * _items;

				if ( (_h + _h_items) >= r.h)
				{
					_size -= (int)( (r.h - _h) / _h_Item);
					break;
				}

				_h += _h_category + _h_items;
				_size -= 1 + _items;

				if ( (_h + _h_category) >= r.h)
				{
					if ( _size > 0  &&  (_h + _h_category) > r.h)		_size++;
					break;
				}
			}

			m_pScrollBar->SetScrollRange( 0, max( 1, _size));
		}

		m_bUpdateScrollBar = false;
		m_bEnsureCaretVisible = true;
	}


	// Ensure caret position
	if ( m_bEnsureCaretVisible == true)
	{
		int _size_category = m_Categories.size();
		if ( _size_category > 0)
		{
			int _count = 0;
			int _scroll = m_pScrollBar->GetScrollValue();
			int _first_type = 0;	// 0=none, 1=caret, 2=start pos
			
			int _cat = 0;
			int _item = -1;
			for ( _cat = 0;  _cat < _size_category;  _cat++)
			{
				int _size_item = (m_Categories[ _cat]->_get_expand() == true)  ?  m_Categories[ _cat]->_get_count() : 0;
				for ( _item = -1;  _item < _size_item;  _item++)
				{
					if ( m_Caret == MCCINDEX( _cat, _item))
					{
						_first_type = 1/*caret*/;
						break;
					}
					else if ( _count == _scroll)
					{
						_first_type = 2/*start pos*/;
						break;
					}

					_count++;
				}

				if ( _first_type > 0/*none*/)		break;
			}


			if ( _first_type == 1/*caret*/)
			{
				int _delta_count = 0;
				bool _break = false;
				for ( _cat;  _cat < _size_category;  _cat++)
				{
					int _size_item = (m_Categories[ _cat]->_get_expand() == true)  ?  m_Categories[ _cat]->_get_count() : 0;
					for ( _item;  _item < _size_item;  _item++)
					{
						if ( _count == _scroll)
						{
							m_pScrollBar->SetScrollValue( m_pScrollBar->GetScrollValue() - _delta_count);
							_break = true;
							break;
						}
						_count++;
						_delta_count++;
					}
					if ( _break == true)	break;
					_item = -1;
				}
			}
			else if ( _first_type == 2/*start pos*/)
			{
				MCCINDEX _head( _cat, _item);
				int _head_item_size = (m_Categories[ _head.nCategory]->_get_expand() == true)  ?  m_Categories[ _head.nCategory]->_get_count() : 0;
				MRECT r = GetClientRect();
				int _h_category = GetCategoryHeight();
				int _h_item = GetItemHeight();
				int _delta_h = 0;
				int _delta_count = 0;
				bool _break = false;
				for ( _cat;  _cat < _size_category;  _cat++)
				{
					int _size_item = (m_Categories[ _cat]->_get_expand() == true)  ?  m_Categories[ _cat]->_get_count() : 0;
					for ( _item;  _item < _size_item;  _item++)
					{
						_delta_h += (_item == -1)  ?  _h_category : _h_item;
						if ( _delta_h > r.h)
						{
							_delta_h -= (_head.nItem == -1)  ?  _h_category : _h_item;
							_delta_count++;

							_head.nItem++;
							if ( _head.nItem >= _head_item_size)
							{
								_head.nCategory++;
								_head.nItem = -1;
								_head_item_size = (m_Categories[ _head.nCategory]->_get_expand() == true)  ?
									m_Categories[ _head.nCategory]->_get_count() : 0;
							}
						}

						if ( m_Caret == MCCINDEX( _cat, _item))
						{
							m_pScrollBar->SetScrollValue( m_pScrollBar->GetScrollValue() + _delta_count);
							_break = true;
							break;
						}
					}
					if ( _break == true)	break;
					_item = -1;
				}
			}
		}

		m_bEnsureCaretVisible = false;
	}
}


// OnSize
void MCategoryCtrl::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	EnsureCaretVisible();
	UpdateScrollBar();
}


// OnDragBegin
bool MCategoryCtrl::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( GetCurSel().empty() == true)	return false;

	for ( vector< MCCINDEX>::iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
	{
		MCCINDEX _sel = (*itr);
		if ( _sel.nCategory < 0  ||  _sel.nItem < 0)
			continue;

		MCCITEM* pItem = m_Categories[ _sel.nCategory]->m_Items[ _sel.nItem];
		MDragObject* pObject = new MDragObject( pItem->m_strText, pItem->m_pImage, pItem->m_dwItemData);
		pDragData->AddObject( pObject);
	}

	if ( HasLookAndFill())
	{
		pDragData->SetFont( m_pLookAndFill->GetFont());
		
		MSIZE _size = ((MCategoryCtrlLook*)m_pLookAndFill)->GetImageSize();
		pDragData->SetImageSize( _size);
		pDragData->SetHotSpot( MPOINT( _size.w >> 1, _size.h >> 1));
	}

	m_bReleaseSelOnLBtnUp = false;

	return MWidget::OnDragBegin( pDragData, pt);
}


// OnToolTip
MWidget* MCategoryCtrl::OnToolTip()
{
	if ( m_MouseOver.empty() == true)		return NULL;

	return MWidget::OnToolTip();
}


// OnLeave
void MCategoryCtrl::OnLeave()
{
	if ( m_MouseOver.empty() == false)		MGetMint()->GetToolTipManager()->Reset();
	m_MouseOver.clear();

	MWidget::OnLeave();
}


// glueSetCurSel
MWLua::ret_int2	MCategoryCtrl::glueSetCurSel( int nCategory, int nItem)
{
	MCCINDEX _sel = SetCurSel( nCategory, nItem);
	return MWLua::ret_int2( _sel.nCategory, _sel.nItem);
}


// glueGetCurSel
MWLua::ret_int2	MCategoryCtrl::glueGetCurSel()
{
	MCCINDEX _sel = GetCurSel();
	return MWLua::ret_int2( _sel.nCategory, _sel.nItem);
}


// glueSetSel
MWLua::ret_int2	MCategoryCtrl::glueSetSel( int nCategory, int nItem, bool bSelect)
{
	MCCINDEX _sel = SetSel( nCategory, nItem, bSelect);
	return MWLua::ret_int2( _sel.nCategory, _sel.nItem);
}


// glueGetSelItem
MWLua::ret_int2	MCategoryCtrl::glueGetSelItem( int nSelIndex)
{
	MCCINDEX _sel = GetSelItem( nSelIndex);
	return MWLua::ret_int2( _sel.nCategory, _sel.nItem);
}


// glueGetCaretPos
MWLua::ret_int2	MCategoryCtrl::glueGetCaretPos()
{
	MCCINDEX _sel = GetCaretPos();
	return MWLua::ret_int2( _sel.nCategory, _sel.nItem);
}


// glueGetMouseOver
MWLua::ret_int2 MCategoryCtrl::glueGetMouseOver()
{
	return MWLua::ret_int2( m_MouseOver.nCategory, m_MouseOver.nItem);
}














// MCategoryCtrlLook
MCategoryCtrlLook::MCategoryCtrlLook()
{
	SetWorkRectOffset( MRECT( 2, 2, -4, -4));
	SetTextAlign( MAM_LEFT | MAM_VCENTER);

	m_nIndent = 10;
	m_nItemHeight = 40;
	m_nCategoryHeight = 20;
	m_ExpandIconSize = MSIZE( 20, 20);
	m_ImageSize = MSIZE( 16, 16);
}

void MCategoryCtrlLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MLookBase::OnDrawBorder( pWidget, pDC);
}

void MCategoryCtrlLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawItems( (MCategoryCtrl*)pWidget, pDC);
}

void MCategoryCtrlLook::OnDrawItems( MCategoryCtrl* pCategory, MDrawContext* pDC)
{
	size_t _size_category = pCategory->GetCategoryCount();
	if ( _size_category <= 0)		return;

	MRECT r = pCategory->GetClientRect();
	int _h_category = pCategory->GetCategoryHeight();
	int _h_item = pCategory->GetItemHeight();

	// Find first position
	MCCINDEX _pos;
	int _scroll = pCategory->GetScrollBar()->GetScrollValue() + 1;
	for ( size_t _cat = 0;  _cat < _size_category;  _cat++)
	{
		_pos.nCategory++;
		_pos.nItem = -1;
		_scroll--;
		if ( _scroll <= 0)			break;
		if ( pCategory->GetCategoryExpand( _cat) == false)	continue;

		int _items = min( (int)pCategory->GetItemCount( _cat), _scroll);
		_pos.nItem += _items;
		_scroll -= _items;
		if ( _scroll <= 0)			break;
	}

	if ( _pos.empty() == true)		return;


	// Draw
	int x = r.x, y = r.y;
	for ( size_t _cat = _pos.nCategory;  _cat < _size_category;  _cat++)
	{
		int _size_item = pCategory->GetItemCount( _cat);

		for ( int _item = _pos.nItem;  _item < _size_item;  _item++)
		{
			// Draw category
			if ( _item == -1)
			{
				// Draw category background
				MRECT _r( x, y, r.w, _h_category);
				OnDrawCategoryBkgrnd( pCategory, _cat, _r, pDC);

				pCategory->EventDrawItemBkgrnd( pDC, _cat, _item, _r);


				// Draw select bar
				if ( pCategory->GetSel( _cat, _item) == true)
					OnDrawSelectbar( pCategory, _cat, _item, _r, pDC);


				if ( pCategory->EventPreDrawItem( pDC, _cat, _item, _r) == false)
				{
					// Draw category
					if ( pCategory->GetCategoryExpand( _cat) == true)
						OnDrawExpandIcon( pCategory, _cat, MRECT( x + 3, y + ((_h_category - m_ExpandIconSize.h) >> 1), m_ExpandIconSize.w, m_ExpandIconSize.h), pDC);
					else
						OnDrawCollapsedIcon( pCategory, _cat, MRECT( x + 3, y + ((_h_category - m_ExpandIconSize.h) >> 1), m_ExpandIconSize.w, m_ExpandIconSize.h), pDC);

					OnDrawCategory( pCategory, _cat, MRECT( x + 5 + m_ExpandIconSize.w, y, r.w - 5 - m_ExpandIconSize.w, _h_category), pDC);


					pCategory->EventDrawItem( pDC, _cat, _item, _r);
				}


				// Draw caret
				if ( pCategory->GetCaretPos() == MCCINDEX( _cat, _item))
					OnDrawCaret( pCategory, _cat, _item, MRECT( x, y, r.w, _h_category), pDC);


				y += _h_category;

				// Check collapsed
				if ( pCategory->GetCategoryExpand( _cat) == false)
					break;
			}

			// Draw items
			else
			{
				// Draw select bar
				if ( pCategory->GetSel( _cat, _item) == true)
					OnDrawSelectbar( pCategory, _cat, _item, MRECT( x, y, r.w, _h_item), pDC);

				// Draw items
				MRECT _r( x + GetIndent(), y, 0, _h_item);
				size_t _size_col = pCategory->GetColumnCount();
				for ( size_t _sub = 0;  _sub < _size_col;  _sub++)
				{
					_r.w = pCategory->GetColumn( _sub)->GetWidth();
					int _align = pCategory->GetColumn( _sub)->GetAlign();

					OnDrawItemBkgrnd( pCategory, _cat, _item, _sub, _r, pDC);

					pCategory->EventDrawItemBkgrnd( pDC, _cat, _item, _r);


					if ( pCategory->EventPreDrawItem( pDC, _cat, _item, _r) == false)
					{
						if ( _sub == 0  &&  pCategory->GetItemImage( _cat, _item) != NULL)
						{
							if ( _align & MAM_VCENTER)		OnDrawImage( pCategory, _cat, _item, MRECT( _r.x, _r.y + ((_h_item - m_ImageSize.h) >> 1), m_ImageSize.w, m_ImageSize.h), pDC);
							else if ( _align & MAM_BOTTOM)	OnDrawImage( pCategory, _cat, _item, MRECT( _r.x, _r.y + _h_item - m_ImageSize.h, m_ImageSize.w, m_ImageSize.h), pDC);
							else							OnDrawImage( pCategory, _cat, _item, MRECT( _r.x, _r.y, m_ImageSize.w, m_ImageSize.h), pDC);

							OnDrawItemText( pCategory, _cat, _item, _sub, MRECT( _r.x + m_ImageSize.w, _r.y, _r.w - m_ImageSize.w, _r.h), _align, pDC);
						}
						else
							OnDrawItemText( pCategory, _cat, _item, _sub, MRECT( _r.x, _r.y, _r.w, _r.h), _align, pDC);


						pCategory->EventDrawItem( pDC, _cat, _item, _r);
					}

					_r.x += _r.w;
				}

				// Draw caret
				if ( pCategory->GetCaretPos() == MCCINDEX( _cat, _item))
					OnDrawCaret( pCategory, _cat, _item, MRECT( x, y, r.w, _h_item), pDC);

				y += _h_item;
			}


			if ( y > r.h)			return;
		}

		_pos.nItem = -1;
	}
}

void MCategoryCtrlLook::OnDrawItemText( MCategoryCtrl* pCatrgory, size_t category, size_t item, size_t subitem, MRECT& r, int align, MDrawContext* pDC)
{
	MCOLOR _color;
	bool bHasColor = pCatrgory->GetItemColor( category, item, subitem, _color);

	if ( pCatrgory->GetItemEnable( category, item) == false)
		pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DISABLE, MColorTable::TEXT[ MWSN_DISABLE]));

	else if ( bHasColor == true)
		pDC->SetColor( _color);

	else if ( pCatrgory->GetSel( category, item) == true)
		pDC->SetColor( GetFontColor( MWP_SELECTBAR, pCatrgory->GetState(), MColorTable::TEXT_SELECTED));

	else
	{
		if ( pCatrgory->IsEnabled() == false)
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DISABLE, MColorTable::TEXT[ MWSN_DISABLE]));

		else if ( pCatrgory->GetCaretPos() == MCCINDEX( category, item))
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MColorTable::TEXT[ MWSN_FOCUS]));

		else
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]));

	}

	pDC->SetFont( GetFont());
	pDC->Text( r, pCatrgory->GetItemText( category, item, subitem), align);
}

void MCategoryCtrlLook::OnDrawCategoryBkgrnd( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, "category", pCategory->GetState()) == false)
	{
		pDC->SetColor( MColorTable::SELECTBAR[ pCategory->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}

void MCategoryCtrlLook::OnDrawCategory( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
{
	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( "category", pCategory->GetState(), MColorTable::TEXT_SELECTED));
	pDC->Text( r, pCategory->GetCategoryText( category), MAM_LEFT | MAM_VCENTER);
}

void MCategoryCtrlLook::OnDrawItemBkgrnd( MCategoryCtrl* pCategory, size_t category, size_t item, size_t subitem, MRECT& r, MDrawContext* pDC)
{
 	Draw( pDC, r, MWP_ITEMBKGRND, pCategory->GetState());
}

void MCategoryCtrlLook::OnDrawCaret( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC)
{
 	if ( Draw( pDC, r, MWP_CARET, pCategory->GetState()) == false)
 	{
 		// Draw default
 		pDC->SetColor( MColorTable::BORDER[ pCategory->GetStateNum()]);
 		pDC->Rectangle( r);
 	}
}

void MCategoryCtrlLook::OnDrawSelectbar( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_SELECTBAR, pCategory->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::SELECTBAR[ pCategory->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}

void MCategoryCtrlLook::OnDrawExpandIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, "expand", pCategory->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pCategory->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pCategory->GetStateNum()]);
		pDC->Rectangle( r);

		pDC->SetColor( MColorTable::TITLE[ pCategory->GetStateNum()]);
		pDC->Line( r.x + (r.w >> 1), r.y + 4, r.x + (r.w >> 1), r.y + r.h - 7);
		pDC->Line( r.x + 4, r.y + (r.h >> 1), r.x + r.w - 7, r.y + (r.h >> 1));
	}
}

void MCategoryCtrlLook::OnDrawCollapsedIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, "collapsed", pCategory->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pCategory->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pCategory->GetStateNum()]);
		pDC->Rectangle( r);

		pDC->SetColor( MColorTable::TITLE[ pCategory->GetStateNum()]);
		pDC->Line( r.x + 4, r.y + (r.h >> 1), r.x + r.w - 7, r.y + (r.h >> 1));
	}
}

void MCategoryCtrlLook::OnDrawImage( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC)
{
	MBitmap* pBitmap = pCategory->GetItemImage( category, item);
	if ( pBitmap != NULL)
	{
		pDC->SetBitmap( pBitmap);

		if ( pCategory->GetItemEnable( category, item) == false)
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


