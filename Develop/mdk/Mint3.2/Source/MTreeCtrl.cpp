#include "stdafx.h"
#include "MTreeCtrl.h"
#include "MScrollBar.h"
#include "MColorTable.h"
#include "Mint.h"



namespace mint3
{


#define DEFAULT_ITEMHEIGHT			17
#define DEFAULT_INDENT				15
#define DEFAULT_MARGIN				5

#define MTVP_EXPAND					"expand"
#define MTVP_COLLAPSED				"collapsed"


enum MTREECTRL_HITREGION
{
	MTCHT_NONE		= 0,
	MTCHT_EXPAND,
	MTCHT_IMAGE,
	MTCHT_BKGRND
};



// MTreeItem
MTreeItem::MTreeItem()
{
	 m_pParent = NULL;
	 m_pPrevSibling = NULL;
	 m_pNextSibling = NULL;
	 m_pImage = NULL;
	 m_dwItemData = 0;
	 m_bExpand = false;
	 m_nID = 0;
}


MTreeItem::~MTreeItem()
{
	if ( m_pPrevSibling != NULL)		m_pPrevSibling->m_pNextSibling = m_pNextSibling;
	if ( m_pNextSibling != NULL)		m_pNextSibling->m_pPrevSibling = m_pPrevSibling;

	if ( m_Items.empty() == false)
	{
		size_t _size = m_Items.size();
		for ( size_t i = 0;  i < _size;  i++)		delete m_Items[ i];
		m_Items.clear();
	}
}


bool MTreeItem::_add( MTreeItem* pItem)
{
	if ( m_Items.empty() == false)
	{
		MTreeItem* pPrevSibling = (*m_Items.rbegin());
		pItem->m_pPrevSibling = pPrevSibling;
		pPrevSibling->m_pNextSibling = pItem;
	}

	pItem->m_pParent = this;
	m_Items.push_back( pItem);

	return true;
}


bool MTreeItem::_insert( size_t nIndex, MTreeItem* pItem)
{
	size_t _size = m_Items.size();

	// Add item
	if ( nIndex >= _size)
	{
		if ( _size > 0)
		{
			MTreeItem* pPrevSibling = (*m_Items.rbegin());
			pItem->m_pPrevSibling = pPrevSibling;
			pPrevSibling->m_pNextSibling = pItem;
		}

		pItem->m_pParent = this;
		m_Items.push_back( pItem);
	}

	// Insert item
	else
	{
		vector< MTreeItem*>::iterator itr = m_Items.begin();
		for ( size_t i = 0;  i < nIndex;  i++, itr++) ;

		MTreeItem* pPrevSibling = (*itr)->m_pPrevSibling;
		if ( pPrevSibling != NULL)
		{
			pPrevSibling->m_pNextSibling = pItem;
			pItem->m_pPrevSibling = pPrevSibling;
		}

		MTreeItem* pNextSibling = (*itr);
		if ( pNextSibling != NULL)
		{
			pNextSibling->m_pPrevSibling = pItem;
			pItem->m_pNextSibling = pNextSibling;
		}

		pItem->m_pParent = this;
		m_Items.insert( itr, pItem);
	}

	return true;
}


bool MTreeItem::_delete( MTreeItem* pItem)
{
	vector< MTreeItem*>::iterator itr = m_Items.begin();
	size_t _size = m_Items.size();
	for ( size_t i = 0;  i < _size;  i++, itr++)
	{
		if ( m_Items[ i] == pItem)
		{
			m_Items.erase( itr);
			delete pItem;
			return true;
		}

		else if ( m_Items[ i]->_delete( pItem) == true)
			return true;
	}
	return false;
}


size_t MTreeItem::_get_count()
{
	size_t _count = 1;
	size_t _size = m_Items.size();
	for ( size_t i = 0;  i < _size;  i++)	_count += m_Items[ i]->_get_count();
	return _count;
}


size_t MTreeItem::_get_visible_count()
{
	size_t _count = 1;
	if ( m_bExpand == true)
	{
		size_t _size = m_Items.size();
		for ( size_t i = 0;  i < _size;  i++)	_count += m_Items[ i]->_get_visible_count();
	}
	return _count;
}


MTreeItem* MTreeItem::_get_first_item()
{
	if ( m_Items.empty() == true)						return this;
	return (*m_Items.begin());
}


MTreeItem* MTreeItem::_get_last_item()
{
	if ( m_Items.empty() == true)						return this;
	return (*m_Items.rbegin())->_get_last_item();
}


MTreeItem* MTreeItem::_get_prev_item()
{
	if ( m_pPrevSibling == NULL)						return m_pParent;
	return m_pPrevSibling->_get_last_item();
}


MTreeItem* MTreeItem::_get_next_item()
{
	if ( m_Items.empty() == false)						return (*m_Items.begin());
	else if ( m_pNextSibling != NULL)					return m_pNextSibling;

	MTreeItem* pParent = m_pParent;
	while ( 1)
	{
		if ( pParent == NULL)							return NULL;
		else if ( pParent->m_pNextSibling != NULL)		return pParent->m_pNextSibling;
		pParent = pParent->m_pParent;
	}

	return NULL;
}


MTreeItem* MTreeItem::_get_first_visible_item()
{
	if ( m_bExpand == false  ||  m_Items.empty())		return this;
	return (*m_Items.begin());
}


MTreeItem* MTreeItem::_get_last_visible_item()
{
	if ( m_bExpand == false  ||  m_Items.empty())		return this;
	return (*m_Items.rbegin())->_get_last_visible_item();
}


MTreeItem* MTreeItem::_get_prev_visible_item()
{
	if ( m_pPrevSibling == NULL)						return m_pParent;
	return m_pPrevSibling->_get_last_visible_item();
}


MTreeItem* MTreeItem::_get_next_visible_item()
{
	if ( m_bExpand == true  &&  m_Items.empty() == false)	return (*m_Items.begin());
	else if ( m_pNextSibling != NULL)						return m_pNextSibling;

	MTreeItem* pParent = m_pParent;
	while ( 1)
	{
		if ( pParent == NULL)							return NULL;
		else if ( pParent->m_pNextSibling != NULL)		return pParent->m_pNextSibling;
		pParent = pParent->m_pParent;
	}

	return NULL;
}


int MTreeItem::_get_indent( int indent)
{
	MTreeItem* pParent = m_pParent;
	int _indent = 0;

	while ( pParent)
	{
		_indent += indent;
		pParent = pParent->m_pParent;
	}

	return _indent;
}




// MTreeItemEx
MTreeItemEx::MTreeItemEx()
{
	m_nHeight = 0;
	m_pFont = NULL;
	m_nColor = MCOLOR( 255, 255, 255);
}












// MTreeCtrl
MTreeCtrl::MTreeCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MTreeCtrlLook);

	SetEnableFocus( true);

	m_RootItem.m_strText = "Root";
	m_RootItem.m_bExpand = true;
	m_RootItem.m_nID = 0;
	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_pCaret = NULL;
	m_pStartSel = m_pCaret;
	m_pStartPos = &m_RootItem;
	m_pMouseOver = NULL;
	m_bShowRootItem = true;
	m_bDragAndDrop = false;
	m_bMultipleSel = false;
	m_bEnsureCaretVisible = false;
	m_bUpdateScrollBar = false;
	m_bUpdateStartPos = false;
	m_bReleaseSelOnLBtnUp = false;
	m_nItemIDCounter = 1;
}


// Destructor
MTreeCtrl::~MTreeCtrl()
{
	DeleteAllItems();
}


// InsertItem
MTreeItem* MTreeCtrl::InsertItem( const char* szText, const char* szImage, MTreeItem* pParentItem)
{
	if ( pParentItem == NULL)	pParentItem = &m_RootItem;


	MTreeItem* pItem = new MTreeItem();
	pItem->m_pParent = pParentItem;
	pItem->m_strText = szText;
	pItem->m_pImage = MBitmapManager::Get( szImage);
	pItem->m_nID = m_nItemIDCounter++;
	pParentItem->_add( pItem);

	EnsureCaretVisible();
	UpdateScrollBar();

	return pItem;
}


// InsertItemEx
MTreeItem* MTreeCtrl::InsertItemEx( const char* szText, const char* szImage, int nHeight, const char* szFont, MCOLOR& nFontColor, MTreeItem* pParentItem)
{
	if ( pParentItem == NULL)	pParentItem = &m_RootItem;

	MTreeItemEx* pItem = new MTreeItemEx();
	pItem->m_pParent = pParentItem;
	pItem->m_strText = szText;
	pItem->m_pImage = MBitmapManager::Get( szImage);
	pItem->m_nID = m_nItemIDCounter++;
	pItem->_set_height( nHeight);
	pItem->_set_font( MFontManager::Get( szFont));
	pItem->_set_color( nFontColor);
	pParentItem->_add( pItem);

	EnsureCaretVisible();
	UpdateScrollBar();

	return pItem;
}


// DeleteItem
bool MTreeCtrl::DeleteItem( MTreeItem* pItem)
{
	if ( pItem == NULL)		return false;

	if ( pItem->m_bExpand == true  &&  ItemHasChildren( pItem) == true)
	{
		MTreeItem* pFirst = pItem;
		MTreeItem* pLast = pItem->_get_last_visible_item();

		bool _break = false;
		while ( 1)
		{
			m_SelectedItems.erase( pFirst);

			if ( pFirst == m_pCaret)		m_pCaret = pItem->m_pParent;
			if ( pFirst == m_pStartSel)		m_pStartSel = pItem->m_pParent;
			if ( pFirst == m_pStartPos)		m_pStartPos = pItem->m_pParent;

			if ( _break == true)			break;
			else if ( pFirst == pLast)		_break = true;
			else							pFirst = pFirst->_get_next_visible_item();
		}
	}

	else
	{
		m_SelectedItems.erase( pItem);

		if ( m_pCaret == pItem)				m_pCaret = pItem->m_pParent;
		if ( m_pStartSel == pItem)			m_pStartSel = pItem->m_pParent;
		if ( m_pStartPos == pItem)			m_pStartPos = pItem->m_pParent;
	}


	if ( pItem == m_pMouseOver)				m_pMouseOver = NULL;

	
	EnsureCaretVisible();
	UpdateScrollBar();

	return m_RootItem._delete( pItem);
}


// DeleteAllItems
void MTreeCtrl::DeleteAllItems()
{
	vector< MTreeItem*>& _root = m_RootItem.m_Items;
	if ( _root.empty() == false)
	{
		size_t _size = _root.size();
		for ( size_t i = 0;  i < _size;  i++)	delete _root[ i];
		_root.clear();
	}

	ReleaseSelect();

	m_pCaret = NULL;
	m_pStartSel = m_pCaret;
	m_pStartPos = &m_RootItem;
	m_nItemIDCounter = 1;
	m_pMouseOver = NULL;

	EnsureCaretVisible();
	UpdateScrollBar();
}


// GetItemCount
size_t MTreeCtrl::GetItemCount()
{
	size_t _count = m_RootItem._get_count();
	if ( m_bShowRootItem == false)	_count--;
	return _count;
}


// GetVisibleCount
size_t MTreeCtrl::GetVisibleCount()
{
	size_t _count = 0;

	if ( m_bShowRootItem == false)
	{
		vector< MTreeItem*>& _root = m_RootItem.m_Items;
		size_t _size = _root.size();
		for ( size_t i = 0;  i < _size;  i++)	_count += _root[ i]->_get_visible_count();
	}

	else
		_count = m_RootItem._get_visible_count();

	return _count;
}


// GetItemHeight
int MTreeCtrl::GetItemHeight( MTreeItem* pItem)
{
	int height = pItem->_get_height();
	if ( height <= 0)
	{
		if ( HasLookAndFill())
		{
			height = ((MTreeCtrlLook*)m_pLookAndFill)->GetItemHeight();
			if ( height < 1)
				height = m_pLookAndFill->GetFont()->GetHeight() + 2;
		}
		else
			height = DEFAULT_ITEMHEIGHT;
	}
	return height;
}


// GetItemFont
MFont* MTreeCtrl::GetItemFont( MTreeItem* pItem)
{
	MFont* pFont = pItem->_get_font();
	if ( pFont == NULL)
	{
		if ( HasLookAndFill())	pFont = m_pLookAndFill->GetFont();
		else					pFont = MFontManager::Get( NULL);
	}
	return pFont;
}


// GetItemFontColor
bool MTreeCtrl::GetItemColor( MTreeItem* pItem, MCOLOR& nColor)
{
	MCOLOR _color;
	if ( pItem->_get_color( _color) == true)
	{
		nColor = _color;
		return true;
	}
	return false;
}


// Expand
void MTreeCtrl::Expand( MTreeItem* pItem, bool bExpand)
{
	if ( pItem->m_bExpand == true  &&  bExpand == false  &&  ItemHasChildren( pItem) == true)
	{
		MTreeItem* pFirst = pItem->_get_first_visible_item();
		MTreeItem* pLast = pItem->_get_last_visible_item();

		bool _break = false;
		while ( 1)
		{
			m_SelectedItems.erase( pFirst);

			if ( pFirst == m_pCaret)		m_pCaret = pItem;
			if ( pFirst == m_pStartSel)		m_pStartSel = pItem;
			if ( pFirst == m_pStartPos)		m_pStartPos = pItem;

			if ( _break == true)			break;
			else if ( pFirst == pLast)		_break = true;
			else							pFirst = pFirst->_get_next_visible_item();
		}
	}

	pItem->m_bExpand = bExpand;

	EnsureCaretVisible();
	UpdateScrollBar();
}


// EnsureVisible
void MTreeCtrl::EnsureVisible( MTreeItem* pItem)
{
	MTreeItem* pEnsureItem = pItem;
	while ( 1)
	{
		if ( pEnsureItem->m_pParent == NULL)		break;
		pEnsureItem = pEnsureItem->m_pParent;
		pEnsureItem->m_bExpand = true;
	}

	EnsureCaretVisible();
	UpdateScrollBar();
}


// ShowRootItem
void MTreeCtrl::ShowRootItem( bool bShow)
{
	if ( m_bShowRootItem == bShow)			return;
	m_bShowRootItem = bShow;

	ReleaseSelect();

	m_pCaret = NULL;
	m_pStartSel = m_pCaret;
	m_pStartPos = GetFirstVisibleItem();

	EnsureCaretVisible();
	UpdateScrollBar();
}


// GetPrevSiblingItem
MTreeItem* MTreeCtrl::GetPrevSiblingItem( MTreeItem* pItem)
{
	if ( pItem == NULL)			return pItem;
	return pItem->_get_prev_sibling_item();
}


// GetNextSiblingItem
MTreeItem* MTreeCtrl::GetNextSiblingItem( MTreeItem* pItem)
{
	if ( pItem == NULL)			return pItem;
	return pItem->_get_next_sibling_item();
}


// GetFirstVisibleItem
MTreeItem* MTreeCtrl::GetFirstVisibleItem()
{
	if ( m_bShowRootItem == true)			return &m_RootItem;
	else
	{
		if ( m_RootItem.m_Items.empty())	return &m_RootItem;
		else								return (*m_RootItem.m_Items.begin());
	}
	return &m_RootItem;
}


// GetPrevVisibleItem
MTreeItem* MTreeCtrl::GetPrevVisibleItem( MTreeItem* pItem)
{
	if ( pItem == NULL)			return pItem;
	return pItem->_get_prev_visible_item();
}


// GetNextVisibleItem
MTreeItem* MTreeCtrl::GetNextVisibleItem( MTreeItem* pItem)
{
	if ( pItem == NULL)			return pItem;
	return pItem->_get_next_visible_item();
}


// SetCurSel
void MTreeCtrl::SetCurSel( MTreeItem* pItem)
{
	if ( pItem == NULL)		return;

	ReleaseSelect();

	m_SelectedItems.insert( pItem);

	m_pCaret = pItem;
}


// SetSel
void MTreeCtrl::SetSel( MTreeItem* pItem, bool bSelect)
{
	if ( pItem == NULL)		return;

	if ( bSelect)
	{
		if ( m_bMultipleSel == false)	ReleaseSelect();
		m_SelectedItems.insert( pItem);
	}
	else
		m_SelectedItems.erase( pItem);
}


// GetSelItem
MTreeItem* MTreeCtrl::GetSelItem( size_t sel_index)
{
	size_t _size = m_SelectedItems.size();
	if ( sel_index >= _size)	return NULL;

	set< MTreeItem*>::const_iterator itr = m_SelectedItems.begin();
	for ( size_t i = 0;  i != sel_index;  i++, itr++) ;
	return (*itr);
}


// GetItemByID
MTreeItem* MTreeCtrl::GetItemByID( int nID)
{
	if ( nID < 0)		return NULL;

	for ( MTreeItem* pItem = &m_RootItem;  pItem != NULL;  pItem = pItem->_get_next_item())
	{
		if ( nID == pItem->m_nID)
			return pItem;
	}
	return NULL;
}


// EnableMultipleSel
void MTreeCtrl::EnableMultipleSel( bool bEnable)
{
	if ( m_bMultipleSel == bEnable)		return;

	m_bMultipleSel = bEnable;
	if ( m_bMultipleSel == false)		ReleaseSelect();
}


// GetIndent
int MTreeCtrl::GetIndent( MTreeItem* pItem)
{
	int _indent;
	if ( HasLookAndFill())	_indent = ((MTreeCtrlLook*)m_pLookAndFill)->GetIndent();
	else					_indent = DEFAULT_INDENT;

	int nIndent = pItem->_get_indent( _indent);
	if ( m_bShowRootItem == false)	nIndent -= _indent;
	return nIndent;
}


// ItemFromPoint
MTreeItem* MTreeCtrl::ItemFromPoint( const MPOINT& pt, int* pHitRegion)
{
	if ( pt.x < 0  ||  pt.y < 0)	return NULL;

	if ( m_bShowRootItem == false  &&  m_pStartPos == &m_RootItem)
		return NULL;

	MTreeItem* pItem = m_pStartPos;
	MRECT r = GetClientRect();


	if ( r.InPoint( (MPOINT)pt) == false)		return NULL;


	int y = 0;
	while ( pItem != NULL)
	{
		int height = GetItemHeight( pItem);
		y += height;

		if ( pt.y < y)
		{
			if ( pHitRegion != NULL)
			{
				MPOINT _pos = pt;
				int _indent = GetIndent( pItem);
				MSIZE _image_size = HasLookAndFill() ? ((MTreeCtrlLook*)m_pLookAndFill)->GetImageSize() : MSIZE( 16, 16);
				MRECT r( _indent + DEFAULT_MARGIN, y - height, r.w, height);

				// Check expand region
				if ( ItemHasChildren( pItem))
				{
					MRECT _r( r.x, r.y + ((r.h - _image_size.h) >> 1), _image_size.w, _image_size.h);
					if ( _r.InPoint( _pos))
					{
						*pHitRegion = MTCHT_EXPAND;
						return pItem;
					}
					r.x += _image_size.w;
				}

				// Check image region
				else if ( pItem->m_pImage != NULL)
				{
					MRECT _r( r.x, r.y + ((r.h - _image_size.h) >> 1), _image_size.w, _image_size.h);
					if ( _r.InPoint( _pos))
					{
						*pHitRegion = MTCHT_IMAGE;
						return pItem;
					}
					r.x += _image_size.w;
				}

				// Check etc...
				*pHitRegion = MTCHT_BKGRND;
			}
			return pItem;
		}

		if ( y > r.h)		return NULL;

		pItem = pItem->_get_next_visible_item();
	}

	if ( pHitRegion != NULL)		*pHitRegion = MTCHT_NONE;
	return NULL;
}


// ScrollPage
void MTreeCtrl::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);

	UpdateStartPos();
}


// OnEvent
bool MTreeCtrl::OnEvent( MEvent* pEvent, MListener* pListener)
{
#define GET_RANGE(s,e,_s,_e)		{	MTreeItem* pItem = m_pCaret;													\
										while ( 1) {																	\
											if ( pItem == m_pStartSel) { st = m_pStartSel; ed = m_pCaret; break; }		\
											if ( pItem == &m_RootItem) { st = m_pCaret; ed = m_pStartSel; break; }		\
											pItem = pItem->_get_prev_visible_item();									\
									}	}


	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	// Translate message
	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		{
			int _hit_region;
			MTreeItem* _sel = ItemFromPoint( pEvent->Pos, &_hit_region);
			if ( _sel == NULL)
			{
				ReleaseSelect();

				m_pCaret = NULL;
				m_pStartSel = m_pCaret;
			}
			else
			{
				// expand/collapse 영역인지 확인
				if ( _hit_region == MTCHT_EXPAND)
				{
					Expand( _sel, !_sel->m_bExpand);
					break;
				}


				// 그외 영역이면...
				m_pCaret = _sel;
				if ( m_pStartSel == NULL)		m_pStartSel = _sel;

				if ( pEvent->bShift == true)
				{
					ReleaseSelect();

					if ( m_bMultipleSel == true)
					{
						MTreeItem *st, *ed;
						GET_RANGE( st, ed, m_pCaret, m_pStartSel);
						while ( st != ed)
						{
							m_SelectedItems.insert( st);
							st = st->_get_next_visible_item();
						}
						m_SelectedItems.insert( ed);
					}
					else
						m_SelectedItems.insert( m_pCaret);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_pStartSel = m_pCaret;

					if ( m_bMultipleSel == true)
					{
						if ( m_SelectedItems.find( m_pCaret) == m_SelectedItems.end())
							m_SelectedItems.insert( m_pCaret);
						else
							m_SelectedItems.erase( m_pCaret);
					}
					else
					{
						ReleaseSelect();
						m_SelectedItems.insert( m_pCaret);
					}
				}
				else if ( GetSel( _sel) == true)
				{
					m_pStartSel = m_pCaret;
					m_bReleaseSelOnLBtnUp = true;
				}
				else
				{
					m_pStartSel = m_pCaret;

					ReleaseSelect();
					m_SelectedItems.insert( m_pCaret);
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
				m_SelectedItems.insert( m_pCaret);
				m_bReleaseSelOnLBtnUp = false;
			}
			break;
		}

	case MWM_LBUTTONDBLCLK :
		{
			ReleaseSelect();

			int _hit_region;
			MTreeItem* _sel = ItemFromPoint( pEvent->Pos, &_hit_region);
			if ( _sel == NULL)
			{
				m_pCaret = NULL;
				m_pStartSel = m_pCaret;
			}
			else
			{
				if ( ItemHasChildren( _sel))		Expand( _sel, !_sel->m_bExpand);
				if ( _hit_region == MTCHT_EXPAND)	break;

				m_pCaret = _sel;
				m_pStartSel = m_pCaret;
				m_SelectedItems.insert( m_pCaret);
			}

			EnsureCaretVisible();

			OnSelChanged();
			OnItemDblClick( pEvent, -1);
			break;
		}

	case MWM_RBUTTONDOWN :
		{
			ReleaseSelect();

			MTreeItem* _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel == NULL)
			{
				m_pCaret = NULL;
				m_pStartSel = m_pCaret;
			}
			else
			{
				m_pCaret = _sel;
				m_pStartSel = m_pCaret;
				m_SelectedItems.insert( m_pCaret);
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

			if ( pEvent->nKey == VK_UP)
			{
				MTreeItem* pFirst = GetFirstVisibleItem();
				if ( pFirst != m_pCaret)
				{
					m_pCaret = m_pCaret->_get_prev_visible_item();
					bUpdate = true;
					bTranslate = true;
				}
			}
			else if ( pEvent->nKey == VK_DOWN)
			{
				MTreeItem* pLast = GetLastVisibleItem();
				if ( pLast != m_pCaret)
				{
					m_pCaret = m_pCaret->_get_next_visible_item();
					bUpdate = true;
					bTranslate = true;
				}
			}

			else if ( pEvent->nKey == VK_PRIOR)
			{
				MTreeItem* pFirst = GetFirstVisibleItem();
				MRECT r = GetClientRect();
				int _height = 0;
				int _scrollval = m_pScrollBar->GetScrollValue();

				MTreeItem* pItem = m_pCaret;
				while ( 1)
				{
					_height += GetItemHeight( pItem);
					if ( _height > r.h)
					{
						m_pCaret = pItem;
						m_pScrollBar->SetScrollValue( _scrollval);
						bUpdate = true;
						bTranslate = true;
						break;
					}

					else if ( pItem == pFirst)
					{
						m_pCaret = pFirst;
						m_pScrollBar->SetScrollValue( 0);
						bUpdate = true;
						bTranslate = true;
						break;
					}

					pItem = pItem->_get_prev_visible_item();
				}
			}

			else if ( pEvent->nKey == VK_NEXT)
			{
				MTreeItem* pLast = GetLastVisibleItem();
				MRECT r = GetClientRect();
				int _height = 0;

				MTreeItem* pItem = m_pCaret;
				while ( 1)
				{
					_height += GetItemHeight( pItem);
					if ( _height > r.h)
					{
						m_pCaret = pItem;
						bUpdate = true;
						bTranslate = true;
						break;
					}

					else if ( pItem == pLast)
					{
						m_pCaret = pLast;
						bUpdate = true;
						bTranslate = true;
						break;
					}

					pItem = pItem->_get_next_visible_item();
				}
			}

			else if ( pEvent->nKey == VK_HOME)
			{
				MTreeItem* pItem = GetFirstVisibleItem();
				if ( pItem  &&  pItem != m_pCaret)
				{
					m_pCaret = pItem;
					bUpdate = true;
					bTranslate = true;
				}
			}

			else if ( pEvent->nKey == VK_END)
			{
				MTreeItem* pItem = GetLastVisibleItem();
				if ( pItem  &&  pItem != m_pCaret)
				{
					m_pCaret = pItem;
					bUpdate = true;
					bTranslate = true;
				}
			}

			// Select all
			else if ( pEvent->nKey == 'A'  &&  pEvent->bCtrl == true)
			{
				MTreeItem* pFirst = GetFirstVisibleItem();
				MTreeItem* pLast = GetLastVisibleItem();

				m_pCaret = pLast;
				m_pStartSel = m_pCaret;

				if ( m_bMultipleSel == true)
				{
					while ( pFirst != pLast)
					{
						m_SelectedItems.insert( pFirst);
						pFirst = pFirst->_get_next_visible_item();
					}
					m_SelectedItems.insert( pLast);
				}
				else
					m_SelectedItems.insert( m_pCaret);

				EnsureCaretVisible();
				bTranslate = true;
			}

			// Copy
			else if ( pEvent->nKey == 'C'  &&  pEvent->bCtrl == true  &&  m_SelectedItems.empty() == false)
			{
				string strString;
				for ( set< MTreeItem*>::const_iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
				{
					if ( strString.empty() == false)	strString += ",";
					strString += (*itr)->m_strText;
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
						MTreeItem *st, *ed;
						GET_RANGE( st, ed, m_pCaret, m_pStartSel);
						while ( st != ed)
						{
							m_SelectedItems.insert( st);
							st = st->_get_next_visible_item();
						}
						m_SelectedItems.insert( ed);
					}
					else
						m_SelectedItems.insert( m_pCaret);
				}
				else if ( pEvent->bCtrl == true)
				{
					m_pStartSel = m_pCaret;
				}
				else
				{
					m_pStartSel = m_pCaret;
					ReleaseSelect();
					m_SelectedItems.insert( m_pCaret);
				}

				EnsureCaretVisible();

				OnSelChanged();
			}

			if ( bTranslate == true)		return true;
		}

	case MWM_MOUSEMOVE :
		{
			MTreeItem* _sel = ItemFromPoint( pEvent->Pos);
			if ( _sel != m_pMouseOver)
			{
				m_pMouseOver = _sel;
				MGetMint()->ChangedToolTipText( this);
			}
		}
	}

	return false;
}


// OnRun
void MTreeCtrl::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	// Ensure caret visible
	if ( m_bEnsureCaretVisible == true)
	{
		m_bEnsureCaretVisible = false;

		if ( m_pCaret != NULL  &&  m_pCaret != m_pStartPos)
		{
			bool bFind = false;

			// Caret이 시작 위치보다 이전에 있는지 확인
			{
				MTreeItem* pItem = m_pStartPos;
				int _scrollval = m_pScrollBar->GetScrollValue();
				while ( 1)
				{
					if ( pItem == m_pCaret)
					{
						m_pStartPos = m_pCaret;
						m_pScrollBar->SetScrollValue( _scrollval);
						bFind = true;
						break;
					}
					if ( pItem == &m_RootItem)	break;
					pItem = pItem->_get_prev_visible_item();
					_scrollval--;
				}
			}

			// Caret이 시작 위치보다 나중에 있으면...
			if ( bFind == false)
			{
				MTreeItem* pHead = m_pStartPos;
				MTreeItem* pTail = m_pStartPos;
				int scrollval = m_pScrollBar->GetScrollValue();
				MRECT r = GetClientRect();
				int height = 0;

				while ( 1)
				{
					height += GetItemHeight( pHead);
					if ( height > r.h)		break;
					else if ( pHead == m_pCaret)
					{
						pHead = pTail;
						break;
					}

					pHead = pHead->_get_next_visible_item();
				}

				if ( pHead != pTail)
				{
					while ( 1)
					{
						if ( height > r.h)
						{
							height -= GetItemHeight( pTail);
							pTail = pTail->_get_next_visible_item();
							scrollval++;
						}

						if ( height <= r.h)
						{
							if ( pHead == m_pCaret)
							{
								m_pStartPos = pTail;
								m_pScrollBar->SetScrollValue( scrollval);
								break;
							}
							else
							{
								pHead = pHead->_get_next_visible_item();
								height += GetItemHeight( pHead);
							}
						}
					}
				}
			}
		}

		UpdateStartPos();
	}


	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		m_bUpdateScrollBar = false;

		MTreeItem* pFirst = GetFirstVisibleItem();
		MTreeItem* pLast = GetLastVisibleItem();
		MRECT r = GetClientRect();
		int _count = GetVisibleCount();
		int _height = 0;

		bool _break = false;
		while ( 1)
		{
			_height += GetItemHeight( pFirst);
			if ( _height > r.h)				break;

			if ( _break == true)			break;
			else if ( pFirst == pLast)		_break = true;
			else
			{
				pFirst = pFirst->_get_next_visible_item();
				_count--;
			}
		}

		if ( _height <= r.h)	_count = 0;
		m_pScrollBar->SetScrollRange( 0, _count);

		UpdateStartPos();
	}


	// Find start position
	if ( m_bUpdateStartPos == true)
	{
		MTreeItem* pItem = GetFirstVisibleItem();
		size_t _size = m_pScrollBar->GetScrollValue();
		for ( size_t i = 0;  i != _size;  i++)			pItem = pItem->_get_next_visible_item();
		m_pStartPos = pItem;

		m_bUpdateStartPos = false;
	}
}


// OnSize
void MTreeCtrl::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	EnsureCaretVisible();
	UpdateScrollBar();
}


// OnScrollBarChanged
void MTreeCtrl::OnScrollBarChanged( int nPos)
{
	UpdateStartPos();

	MWidget::OnScrollBarChanged( nPos);
}


// OnDragBegin
bool MTreeCtrl::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( GetCurSel() < 0)	return false;

	for ( set< MTreeItem*>::iterator itr = m_SelectedItems.begin();  itr != m_SelectedItems.end();  itr++)
	{
		MDragObject* pObject = new MDragObject( (*itr)->m_strText, (*itr)->m_pImage, (*itr)->m_dwItemData);
		pDragData->AddObject( pObject);
	}

	if ( HasLookAndFill())
	{
		pDragData->SetFont( m_pLookAndFill->GetFont());

		MSIZE _size = ((MTreeCtrlLook*)m_pLookAndFill)->GetImageSize();
		pDragData->SetImageSize( _size);
		pDragData->SetHotSpot( MPOINT( _size.w >> 1, _size.h >> 1));
	}

	m_bReleaseSelOnLBtnUp = false;

	return MWidget::OnDragBegin( pDragData, pt);
}


// OnToolTip
MWidget* MTreeCtrl::OnToolTip()
{
	if ( m_pMouseOver == NULL)
		return NULL;

	return MWidget::OnToolTip();
}


// OnLeave
void MTreeCtrl::OnLeave()
{
	if ( m_pMouseOver != NULL)		MGetMint()->GetToolTipManager()->Reset();
	m_pMouseOver = NULL;

	MWidget::OnLeave();
}


// glueInsertItem
int MTreeCtrl::glueInsertItem( const char* szText, const char* szImage, int nParentItem)
{
	MTreeItem* pParentItem = GetItemByID( nParentItem);
	if ( pParentItem == NULL)		pParentItem = &m_RootItem;

	MTreeItem* pItem = InsertItem( szText, szImage, pParentItem);
	if ( pItem == NULL)				return -1;
	return pItem->m_nID;
}


// glueInsertItemEx
int MTreeCtrl::glueInsertItemEx( const char* szText, const char* szImage, int nParentItem)
{
	MTreeItem* pParentItem = GetItemByID( nParentItem);
	if ( pParentItem == NULL)		pParentItem = &m_RootItem;

	MTreeItem* pItem = InsertItemEx( szText, szImage, 0, NULL, MCOLOR( 255, 255, 255), pParentItem);
	if ( pItem == NULL)				return -1;
	return pItem->m_nID;
}


// glueDeleteItem
bool MTreeCtrl::glueDeleteItem( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return false;
	return DeleteItem( pItem);
}


// glueSetItemText
void MTreeCtrl::glueSetItemText( int nItem, const char* szText)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return;
	SetItemText( pItem, szText);
}


// glueGetItemText
const char*	MTreeCtrl::glueGetItemText( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return "";
	return GetItemText( pItem);
}


// glueSetItemImage
void MTreeCtrl::glueSetItemImage( int nItem, const char* szImage)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return;
	SetItemImage( pItem, szImage);
}


// glueGetItemImage
const char* MTreeCtrl::glueGetItemImage( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return "";

	MBitmap* pImage = GetItemImage( pItem);
	if ( pImage == NULL)	return "";
	return pImage->GetName();
}


// glueSetItemData
void MTreeCtrl::glueSetItemData( int nItem, int nData)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return;
	SetItemData( pItem, nData);
}


// glueGetItemData
int MTreeCtrl::glueGetItemData( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return 0;
	return GetItemData( pItem);
}


// glueSetItemHeight
void MTreeCtrl::glueSetItemHeight( int nItem, int nHeight)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return;
	SetItemHeight( pItem, nHeight);
}


// glueGetItemHeight
int MTreeCtrl::glueGetItemHeight( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return 0;
	return GetItemHeight( pItem);
}


// glueSetItemFont
void MTreeCtrl::glueSetItemFont( int nItem, const char* szFont)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return;

	MFont* pFont = MFontManager::Get( szFont);
	SetItemFont( pItem, pFont);
}


// glueSetItemColor
void MTreeCtrl::glueSetItemColor( int nItem, int r, int g, int b)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return;
	SetItemColor( pItem, MCOLOR( r, g, b));
}


// glueExpand
void MTreeCtrl::glueExpand( int nItem, bool bExpand)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem != NULL)		Expand( pItem, bExpand);
}


// glueGetRootItem
int MTreeCtrl::glueGetRootItem()
{
	return m_RootItem.m_nID;
}


// glueGetParentItem
int MTreeCtrl::glueGetParentItem( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)			return -1;
	
	MTreeItem* pParentItem = GetParentItem( pItem);
	if ( pParentItem == NULL)	return -1;
	return pParentItem->m_nID;
}


// glueItemHasChildren
bool MTreeCtrl::glueItemHasChildren( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)			return false;
	return ItemHasChildren( pItem);
}


// glueGetPrevSiblingItem
int MTreeCtrl::glueGetPrevSiblingItem( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)			return -1;
	
	MTreeItem* pPrevSibling = GetPrevSiblingItem( pItem);
	if ( pPrevSibling == NULL)	return -1;
	return pPrevSibling->m_nID;
}


// glueGetNextSiblingItem
int MTreeCtrl::glueGetNextSiblingItem( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)			return -1;

	MTreeItem* pNextSibling = GetNextSiblingItem( pItem);
	if ( pNextSibling == NULL)	return -1;
	return pNextSibling->m_nID;
}


// glueGetFirstVisibleItem
int MTreeCtrl::glueGetFirstVisibleItem()
{
	MTreeItem* pItem = GetFirstVisibleItem();
	if ( pItem == NULL)			return -1;
	return pItem->m_nID;
}


// glueGetLastVisibleItem
int MTreeCtrl::glueGetLastVisibleItem()
{
	MTreeItem* pItem = GetLastVisibleItem();
	if ( pItem == NULL)			return -1;
	return pItem->m_nID;
}


// glueGetPrevVisibleItem
int MTreeCtrl::glueGetPrevVisibleItem( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)			return -1;

	MTreeItem* pPrevVisible = GetPrevVisibleItem( pItem);
	if ( pPrevVisible == NULL)			return -1;
	return pPrevVisible->m_nID;
}


// glueGetNextVisibleItem
int MTreeCtrl::glueGetNextVisibleItem( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)			return -1;

	MTreeItem* pNextVisible = GetNextVisibleItem( pItem);
	if ( pNextVisible == NULL)			return -1;
	return pNextVisible->m_nID;
}


// glueSetCurSel
void MTreeCtrl::glueSetCurSel( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem != NULL)		SetCurSel( pItem);
}


// glueGetCurSel
int MTreeCtrl::glueGetCurSel()
{
	MTreeItem* pItem = GetCurSel();
	if ( pItem == NULL)		return -1;
	return pItem->m_nID;
}


// glueSetSel
void MTreeCtrl::glueSetSel( int nItem, bool bSelect)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem != NULL)		SetSel( pItem, bSelect);
}


// glueGetSel
bool MTreeCtrl::glueGetSel( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem == NULL)		return false;
	return GetSel( pItem);
}


// glueGetSelItem
int MTreeCtrl::glueGetSelItem( int sel_index)
{
	MTreeItem* pItem = GetSelItem( sel_index);
	if ( pItem != NULL)		return -1;
	return pItem->m_nID;
}


// glueSetCaretPos
void MTreeCtrl::glueSetCaretPos( int nItem)
{
	MTreeItem* pItem = GetItemByID( nItem);
	if ( pItem != NULL)		SetCaretPos( pItem);
}


// glueGetCaretPos
int MTreeCtrl::glueGetCaretPos()
{
	MTreeItem* pItem = GetCaretPos();
	if ( pItem != NULL)		return -1;
	return pItem->m_nID;
}


// glueGetMouseOver
int MTreeCtrl::glueGetMouseOver()
{
	if ( m_pMouseOver == NULL)		return -1;
	return m_pMouseOver->m_nID;
}













// MTreeCtrlLook
MTreeCtrlLook::MTreeCtrlLook()
{
	SetTextAlign( MAM_LEFT | MAM_VCENTER);
	m_nIndent = DEFAULT_INDENT;
	m_nItemHeight = 0;
	m_ImageSize = MSIZE( 16, 16);
}


void MTreeCtrlLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MTreeCtrl* pTreeCtrl = (MTreeCtrl*)pWidget;

	OnDrawItems( pTreeCtrl, pDC);
}


void MTreeCtrlLook::OnDrawItems( MTreeCtrl* pTreeCtrl, MDrawContext* pDC)
{
	int y = 0;
	MRECT r = pTreeCtrl->GetClientRect();
	MTreeItem* pItem = pTreeCtrl->GetStartPos();
	MTreeItem* pLastVisibleItem = pTreeCtrl->GetLastVisibleItem();

	if ( pTreeCtrl->IsShowRootItem() == false  &&  pItem == pTreeCtrl->GetRootItem())
		return;

	// Draw items
	while ( 1)
	{
		int height = pTreeCtrl->GetItemHeight( pItem);

		// Draw item background
		MRECT _r( 0, y, r.w, height);
		OnDrawItemBkgrnd( pTreeCtrl, pItem,  MRECT( 0, y, r.w, height), pDC);

		
		// Draw selectbar
		if ( pTreeCtrl->GetSel( pItem) == true)		OnDrawSelectbar( pTreeCtrl, pItem, MRECT( 0, y, r.w, height), pDC);


		// Draw item
		int indent = pTreeCtrl->GetIndent( pItem);
		_r.x += indent + DEFAULT_MARGIN;
		_r.w -= indent;


		if ( pTreeCtrl->EventPreDrawItem( pDC, 0, 0, _r) == false)
		{
			// Draw expand/collapsed icon
			if ( pTreeCtrl->ItemHasChildren( pItem))
			{
				if ( pItem->m_bExpand == true)
					OnDrawExpandIcon( pTreeCtrl, pItem, MRECT( _r.x, _r.y + ((height - m_ImageSize.h) >> 1), m_ImageSize.w, m_ImageSize.h), pDC);
				else
					OnDrawCollapsedIcon( pTreeCtrl, pItem, MRECT( _r.x, _r.y + ((height - m_ImageSize.h) >> 1), m_ImageSize.w, m_ImageSize.h), pDC);

				_r.x += m_ImageSize.w;
				_r.w -= m_ImageSize.w;
			}


			// Draw icon
			if ( pItem->m_pImage != NULL)
			{
				OnDrawImage( pTreeCtrl, pItem, MRECT( _r.x, _r.y + ((height - m_ImageSize.h) >> 1), m_ImageSize.w, m_ImageSize.h), pDC);
				_r.x += m_ImageSize.w;
				_r.w -= m_ImageSize.w;
			}


			// Draw item text
			OnDrawItemText( pTreeCtrl, pItem, _r, pDC);


			// Draw event
			pTreeCtrl->EventDrawItem( pDC, 0, 0, r);
		}

		
		// Draw caret
		if ( pTreeCtrl->GetCaretPos() == pItem)		OnDrawCaret( pTreeCtrl, pItem,  MRECT( 0, y, r.w, height), pDC);


		y += height;
		if ( y > r.h  ||  pItem == pLastVisibleItem)	break;

		pItem = pItem->_get_next_visible_item();
	}
}


void MTreeCtrlLook::OnDrawItemText( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC)
{
	MCOLOR _color;
	if ( pTreeCtrl->GetItemColor( pItem, _color) == true)
		pDC->SetColor( _color);

	else
	{
		if ( pTreeCtrl->GetSel( pItem) == true)
			pDC->SetColor( GetFontColor( MWP_SELECTBAR, pTreeCtrl->GetState(), MColorTable::TEXT_SELECTED));
		else
		{
			if ( pTreeCtrl->IsEnabled() == false)
				pDC->SetColor( GetFontColor( MWP_BORDER, MWS_DISABLE, MColorTable::TEXT[ MWSN_DISABLE]));

			else if ( pTreeCtrl->GetCaretPos() == pItem)
				pDC->SetColor( GetFontColor( MWP_BORDER, MWS_FOCUS, MColorTable::TEXT[ MWSN_FOCUS]));

			else
				pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]));

		}
	}

	pDC->SetFont( pTreeCtrl->GetItemFont( pItem));
	pDC->Text( r, pTreeCtrl->GetItemText( pItem), GetTextAlign());
}


void MTreeCtrlLook::OnDrawItemBkgrnd( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC)
{
	Draw( pDC, r, MWP_ITEMBKGRND, pTreeCtrl->GetState());

	pTreeCtrl->EventDrawItemBkgrnd( pDC, 0, 0, r);
}


void MTreeCtrlLook::OnDrawCaret( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_CARET, pTreeCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::BORDER[ pTreeCtrl->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}


void MTreeCtrlLook::OnDrawSelectbar( MTreeCtrl* pTreeCtrl, MTreeItem* pItem, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_SELECTBAR, pTreeCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::SELECTBAR[ pTreeCtrl->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}


void MTreeCtrlLook::OnDrawExpandIcon( MTreeCtrl* pTreeCtrl, MTreeItem* pTreeItem, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MTVP_EXPAND, pTreeCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pTreeCtrl->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pTreeCtrl->GetStateNum()]);
		pDC->Rectangle( r);

		pDC->SetColor( MColorTable::TITLE[ pTreeCtrl->GetStateNum()]);
		pDC->Line( r.x + (r.w >> 1), r.y + 4, r.x + (r.w >> 1), r.y + r.h - 7);
		pDC->Line( r.x + 4, r.y + (r.h >> 1), r.x + r.w - 7, r.y + (r.h >> 1));
	}
}


void MTreeCtrlLook::OnDrawCollapsedIcon( MTreeCtrl* pTreeCtrl, MTreeItem* pTreeItem, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MTVP_COLLAPSED, pTreeCtrl->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pTreeCtrl->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pTreeCtrl->GetStateNum()]);
		pDC->Rectangle( r);

		pDC->SetColor( MColorTable::TITLE[ pTreeCtrl->GetStateNum()]);
		pDC->Line( r.x + 4, r.y + (r.h >> 1), r.x + r.w - 7, r.y + (r.h >> 1));
	}
}


void MTreeCtrlLook::OnDrawImage( MTreeCtrl* pTreeCtrl, MTreeItem* pTreeItem, MRECT& r, MDrawContext* pDC)
{
	pDC->SetBitmap( pTreeItem->m_pImage);
	pDC->Draw( r);
}



}	// namespace mint3
