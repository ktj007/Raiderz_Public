#include "stdafx.h"
#include "MTreeView.h"
#include "MColorTable.h"
#include "Mint.h"



namespace mint3
{


enum MTREEVIEW_IMAGE
{
	MCOLLAPSED	= 0,
	MEXPAND,
};



IMPLEMENT_LOOK( MTreeView, MTreeViewLook)
IMPLEMENT_NULL_LOOK( MTreeView, MNullTreeViewLook)



// class : MTreeItem

MTreeItem::MTreeItem()
{
	m_nIconIndex = 0;
	m_nData = -1;
	m_bExpand = false;
}

MTreeItem::MTreeItem( const char* szString, int nIconIndex)
{
	m_strString = szString;
	m_nIconIndex = nIconIndex;
	m_nData = -1;
	m_bExpand = false;
}

MTreeItem::~MTreeItem()
{
	if ( m_ChildList.empty() == false)
	{
		size_t _size = m_ChildList.size();
		for ( size_t i = 0;  i < _size;  i++)
			delete m_ChildList[ i];

		m_ChildList.clear();
	}
}


bool MTreeItem::InsertItem( MTreeItem* pItem)
{
	if ( pItem == NULL)
		return false;

	m_ChildList.push_back( pItem);
	return true;
}


bool MTreeItem::DeleteItem( MTreeItem* pItem)
{
	if ( m_ChildList.empty())
		return false;

	size_t _size = m_ChildList.size();
	vector< MTreeItem*>::iterator itr = m_ChildList.begin();
	for ( size_t i = 0;  i < _size;  i++)
	{
		if ( m_ChildList[ i] == pItem)
		{
			delete m_ChildList[ i];

			m_ChildList.erase( itr);
			return true;
		}

		else if ( m_ChildList[ i]->DeleteItem( pItem) == true)
			return true;

		itr++;
	}

	return false;
}

size_t MTreeItem::GetRootItemCount()
{
	return m_ChildList.size();
}

MTreeItem* MTreeItem::GetRootItem( size_t index)
{
	return m_ChildList[ index];
}

void MTreeItem::SetString( const char* szString)
{
	m_strString = szString;
}

const char* MTreeItem::GetString()
{
	return m_strString.c_str();
}


void MTreeItem::SetIcon( int index)
{
	m_nIconIndex = index;
}

int MTreeItem::GetIcon() const
{
	return m_nIconIndex;
}


void MTreeItem::SetUserData( int data)
{
	m_nData = data;
}

int MTreeItem::GetUserData() const
{
	return m_nData;
}


void MTreeItem::SetExpand( bool bExpand)
{
	m_bExpand = bExpand;
}

bool MTreeItem::IsExpanded() const
{
	return m_bExpand;
}


bool MTreeItem::HasChild() const
{
	return (m_ChildList.empty() ? false : true);
}


int MTreeItem::GetCount() const
{
	int nCount = 1;

	size_t _size = m_ChildList.size();
	for ( size_t i = 0;  i < _size;  i++)
		nCount += m_ChildList[ i]->GetCount();

	return nCount;	
}


int MTreeItem::GetVisibledItemHeight( int nDefaultHeight)
{
	int height = GetHeight() ? GetHeight() : nDefaultHeight;

	if ( IsExpanded())
	{
		size_t _size = m_ChildList.size();
		for ( size_t i = 0;  i < _size;  i++)
			height += m_ChildList[ i]->GetVisibledItemHeight( nDefaultHeight);
	}

	return height;
}

MTreeItem* MTreeItem::GetItemPos( int nDefaultHeight, MPOINT& pos, int x, int& y)
{
	int column = GetHeight() ? GetHeight() : nDefaultHeight;

	if ( pos.y < (y + column))
		return this;

	y += column;

	if ( IsExpanded())
	{
		MTreeItem* pItem = NULL;
		size_t _size = m_ChildList.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( pItem = m_ChildList[ i]->GetItemPos( nDefaultHeight, pos, x + MTREEINDENT, y))
				return pItem;
		}
	}

	return NULL;
}

bool MTreeItem::GetItemPos( MTreeItem* pItem, int nDefaultHeight, MPOINT& pos, int x, int& y)
{
	if ( pItem == this)
	{
		pos.x = x;
		pos.y = y;
		return true;
	}

	y += GetHeight() ? GetHeight() : nDefaultHeight;

	if ( IsExpanded())
	{
		size_t _size = m_ChildList.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( m_ChildList[ i]->GetItemPos( pItem, nDefaultHeight, pos, x + MTREEINDENT, y) == true)
				return true;
		}
	}

	return false;
}


MTREEHITINFO MTreeItem::GetSelItem( int nDefaultHeight, MPOINT& pos, int dx, int& dy)
{
	MTREEHITINFO info;
	int column = GetHeight() ? GetHeight() : nDefaultHeight;

	if ( dy >= 0  ||  (dy + column) >= 0)
	{
		if ( pos.y <= column)
		{
			info.m_pItem = this;
			info.m_nHitRegion = MHTSTRING;
			info.m_ptHitPos.x = pos.x - dx;
			info.m_ptHitPos.y = pos.y;

			return info;
		}

		pos.y -= column;
	}
	else
		dy += column;


	if ( IsExpanded())
	{
		size_t _size = m_ChildList.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			info = m_ChildList[ i]->GetSelItem( nDefaultHeight, pos, dx + MTREEINDENT, dy);
			if ( info.m_pItem)
				break;
		}
	}

	return info;
}




// class : MTreeItemEx

MTreeItemEx::MTreeItemEx()
{
	m_nIconIndex = 0;
	m_nData = -1;
	m_bExpand = false;
	m_pFont = NULL;
	m_nFontColor = MCOLOR( 255, 255, 255);
	m_nHeight = 0;
}

MTreeItemEx::MTreeItemEx( const char* szString, int nIconIndex/*=0*/, const char* szFont/*=NULL*/, MCOLOR nFontColor/*=MCOLOR( 255, 255, 255)*/, unsigned char nHeight/*=0*/)
{
	m_strString = szString;
	m_nIconIndex = nIconIndex;
	m_nData = -1;
	m_bExpand = false;
	m_pFont = (szFont[ 0] == NULL) ? NULL : MFontManager::Get( szFont);
	m_nFontColor = nFontColor;
	m_nHeight = nHeight;
}

MTreeItemEx::~MTreeItemEx()
{
	if ( m_ChildList.empty() == false)
	{
		size_t _size = m_ChildList.size();
		for ( size_t i = 0;  i < _size;  i++)
			delete m_ChildList[ i];

		m_ChildList.clear();
	}
}


void MTreeItemEx::SetFont( MFont* pFont)
{
	m_pFont = pFont;
}

MFont* MTreeItemEx::GetFont()
{
	return m_pFont;
}


void MTreeItemEx::SetFontColor( MCOLOR& color)
{
	m_nFontColor = color;
}

MCOLOR MTreeItemEx::GetFontColor( MCOLOR def_color/*=MCOLOR( 255, 255, 255)*/) const
{
	return m_nFontColor;
}


void MTreeItemEx::SetHeight( int height)
{
	m_nHeight = max( 0, height);
}

int MTreeItemEx::GetHeight() const
{
	return (m_nHeight ? m_nHeight : m_pFont->GetHeight());
}














// MTreeViewLook

MTreeViewLook::MTreeViewLook()
{
}


// OnDraw
void MTreeViewLook::OnDraw( MTreeView* pTreeView, MDrawContext* pDC)
{
	MRECT client = pTreeView->GetClientRect();

	// Draw frame
	OnDrawFrame( pTreeView, pDC, client);


	// Draw items
	int x = 0, y = -pTreeView->GetScrollbar()->GetValue();
	size_t _size = pTreeView->GetRootItemCount();
	for ( size_t i = 0;  i < _size;  i++)
	{
		OnDrawItem( pTreeView, pTreeView->GetRootItem( i), pDC, client, x, y);
	}
}


// OnDrawFrame
void MTreeViewLook::OnDrawFrame( MTreeView* pTreeView, MDrawContext* pDC, MRECT& client)
{
	// client edge
#ifdef _DEBUG
	// 텍스트가 "show rect"이면 윈도우 영역을 표시한다
	if ( strcmp( pTreeView->GetText(), "show rect") == 0)
	{
		pDC->SetColor( 255, 255, 255, (int)(25.0 * sin( (double)timeGetTime() * 0.005) + 25.0));
		pDC->Rectangle( client);
	}
#endif
}


// GetColumnRatio
void GetColumnRatio( MBitmap* bmp, MRECT& rect, int column, int align)
{
	if ( bmp->GetHeight() > column)
	{
		rect.w = (int)( (float)bmp->GetWidth() * (float)column / (float)bmp->GetHeight());
		rect.h = column;
	}
	else
	{
		rect.w = bmp->GetWidth();
		rect.h = bmp->GetHeight();

		if ( align & MAM_VCENTER)		rect.y += (column - rect.h) >> 1;
		else if ( align & MAM_BOTTOM)	rect.y += column - rect.h;
	}
}


// DrawColumnRatioBmp
int MTreeViewLook::DrawColumnRatioBmp( MDrawContext* pDC, MBitmap* bmp, int x, int y, int column, int align)
{
	if ( bmp == NULL)
		return 0;

	MRECT rect( x, y, 0, 0);
	GetColumnRatio( bmp, rect, column, align);

	pDC->SetBitmap( bmp);
	pDC->Draw( rect);

	return rect.w;
}


// OnDrawItem
void MTreeViewLook::OnDrawItem( MTreeView* pTreeView, MTreeItem* pTreeItem, MDrawContext* pDC, MRECT& client, int x, int& y)
{
	int column = pTreeItem->GetHeight() ? pTreeItem->GetHeight() : pTreeView->GetItemHeight();

	// Check valid rect
	if ( y <= 0  &&  (y + column) >= 0)		y = 0;
	else if ( y > client.h)					return;


	// Draw item
	int px = x, py = y;
	if ( y >= 0)
	{
		// Draw select bar
		if ( pTreeView->GetSelectedItem() == pTreeItem)
		{
			MRECT _r( 0, y, client.w, column);
			OnDrawSelectbar( pTreeView, pDC, _r);
		}


		// Draw expand icon
		if ( pTreeItem->HasChild())
		{
			if ( pTreeItem->IsExpanded())
			{
				if ( pTreeView->GetExpandedImage())
				{
					px += DrawColumnRatioBmp( pDC, pTreeView->GetExpandedImage(), px, py, column, pTreeView->GetAlign());
				}
				else
				{
					pDC->SetColor( 255, 255, 255);
					pDC->FillRectangle( x, y, column, column);
					px += column;
				}
			}

			// Draw collapsed icon
			else
			{
				if ( pTreeView->GetCollapsedImage())
				{
					px += DrawColumnRatioBmp( pDC, pTreeView->GetCollapsedImage(), px, py, column, pTreeView->GetAlign());
				}
				else
				{
					pDC->SetColor( 255, 255, 255);
					pDC->FillRectangle( x, y, column, column);
					px += column;
				}
			}
		}


		// Draw item icon
		if ( pTreeItem->GetIcon() >= 0  &&  pTreeItem->GetIcon() < (int)pTreeView->GetIconCount())
			px += DrawColumnRatioBmp( pDC, pTreeView->GetIcon( pTreeItem->GetIcon()), px, py, column, pTreeView->GetAlign());


		// Draw text
		pDC->SetFont( pTreeItem->GetFont() ? pTreeItem->GetFont() : pTreeView->GetFont());
		pDC->SetColor( pTreeItem->GetFontColor());
		if ( pDC->GetFont()->GetHeight() > column)	pDC->Text( px, py, pTreeItem->GetString());
		else
		{
			if ( pTreeView->GetAlign() & MAM_VCENTER)		pDC->Text( px, py + ((column - pDC->GetFont()->GetHeight()) >> 1), pTreeItem->GetString());
			else if ( pTreeView->GetAlign() & MAM_BOTTOM)	pDC->Text( px, py + (column - pDC->GetFont()->GetHeight()), pTreeItem->GetString());
			else											pDC->Text( px, py, pTreeItem->GetString());
		}
	}


	// Adjust position
	y += column;


	// Draw child item
	if ( pTreeItem->IsExpanded())
	{
		size_t _size = pTreeItem->GetRootItemCount();
		for ( size_t i = 0;  i < _size;  i++)
			OnDrawItem( pTreeView, pTreeItem->GetRootItem( i), pDC, client, x + MTREEINDENT, y);
	}
}


// OnDrawSelectbar
void MTreeViewLook::OnDrawSelectbar( MTreeView* pTreeView, MDrawContext* pDC, MRECT& r)
{
	pDC->SetColor( MColorTable::MLIST_SELECTEDPLANE);
	pDC->FillRectangle( r);
}


// GetClientRect
MRECT MTreeViewLook::GetClientRect( MTreeView* pTreeView, MRECT& client)
{
	return client;
}



















// class : MTreeVeiw

// Constructor
MTreeView::MTreeView( const char* szName, MWidget* pParent, MListener* pListener) : MWidget( szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR();

	m_pScrollbar = new MScrollBar( this, this);
	m_pScrollbar->Show( true);
	m_pScrollbar->SetRange( 0, 400);
	m_pScrollbar->SetValue( 0);

	m_nScrollbarVisible = MSB_SHOW;

	SetFocusEnable( true);
	SetClipByParent( true);

	m_pSelItem = NULL;
	m_nItemHeight = 17;
	m_pImage[ MCOLLAPSED] = NULL;
	m_pImage[ MEXPAND] = NULL;
	m_nAlign = MAM_LEFT | MAM_TOP;
	m_bScrollUpdate = false;
}


// Destructor
MTreeView::~MTreeView()
{
	ResetContents();
}


// InsertItem
MTreeItem* MTreeView::InsertItem( const char* szText, int nIconIndex/*=0*/, MTreeItem* pParentItem/*=NULL*/)
{
	MTreeItem* pTreeItem = new MTreeItem( szText, nIconIndex);

	// 부모가 NULL이면 루트에 추가
	if ( pParentItem == NULL)		m_vItemList.push_back( pTreeItem);
	else							pParentItem->InsertItem( pTreeItem);

	// Update scrollbar
	m_bScrollUpdate = true;

	return pTreeItem;
}

MTreeItem* MTreeView::InsertItem( MTreeItem* pItem, MTreeItem* pParentItem/*=NULL*/)
{
	if ( pItem == NULL)
		return NULL;

	// 부모가 NULL이면 루트에 추가
	if ( pParentItem == NULL)		m_vItemList.push_back( pItem);
	else							pParentItem->InsertItem( pItem);

	// Update scrollbar
	m_bScrollUpdate = true;

	return pItem;
}

MTreeItem* MTreeView::InsertItemEx( const char* szText, int nIconIndex/*=0*/, const char* szFont/*=NULL*/, MCOLOR nFontColor/*=MCOLOR( 255, 255, 255)*/, unsigned char nHeight/*=0*/, MTreeItem* pParentItem/*=NULL*/)
{
	MTreeItemEx* pTreeItem = new MTreeItemEx( szText, nIconIndex, szFont, nFontColor, nHeight);

	// 부모가 NULL이면 루트에 추가
	if ( pParentItem == NULL)		m_vItemList.push_back( pTreeItem);
	else							pParentItem->InsertItem( pTreeItem);

	// Update scrollbar
	m_bScrollUpdate = true;

	return pTreeItem;
}



// DeleteItem
bool MTreeView::DeleteItem( MTreeItem* pItem)
{
	if ( pItem == NULL  ||  m_vItemList.empty())
		return false;

	size_t _size = m_vItemList.size();
	vector< MTreeItem*>::iterator itr = m_vItemList.begin();
	for ( size_t i = 0;  i < _size;  i++)
	{
		if ( m_vItemList[ i] == pItem)
		{
			// Delete item
			delete m_vItemList[ i];
			m_vItemList.erase( itr);

			// Update scrollbar
			m_bScrollUpdate = true;

			return true;
		}

		else if ( m_vItemList[ i]->DeleteItem( pItem) == true)
		{
			// Update scrollbar
			m_bScrollUpdate = true;

			return true;
		}

		itr++;
	}

	return false;
}


// ResetContents
void MTreeView::ResetContents()
{
	// Delete item list
	if ( m_vItemList.empty() == false)
	{
		size_t _size = m_vItemList.size();
		for ( size_t i = 0;  i < _size;  i++)
			delete m_vItemList[ i];

		m_vItemList.clear();
	}

	// Clear selected item
	m_pSelItem = NULL;

	// Update scrollbar
	m_bScrollUpdate = true;
}


// GetRootItemCount
size_t MTreeView::GetRootItemCount()
{
	return m_vItemList.size();
}


// GetRootItem
MTreeItem* MTreeView::GetRootItem( size_t index)
{
	return m_vItemList[ index];
}


// Expand
void MTreeView::Expand( MTreeItem* pItem, bool bExpand)
{
	if ( pItem == NULL)
		return;

	pItem->SetExpand( bExpand);

	// Update scrollbar
	m_bScrollUpdate = true;
}


// SetItemString
void MTreeView::SetItemString( MTreeItem* pItem, const char* szString)
{
	if ( pItem == NULL)
		return;

	pItem->SetString( szString);
}


// SetItemIcon
void MTreeView::SetItemIcon( MTreeItem* pItem, int index)
{
	if ( pItem == NULL)
		return;

	pItem->SetIcon( index);
}


// SetUserData
void MTreeView::SetUserData( MTreeItem* pItem, int data)
{
	if ( pItem == NULL)
		return;

	pItem->SetUserData( data);
}


// SetItemFont
void MTreeView::SetItemFont( MTreeItem* pItem, MFont* pFont)
{
	if ( pItem == NULL)
		return;

	pItem->SetFont( pFont);

	// Update scrollbar
	m_bScrollUpdate = true;
}


// SetItemFontColor
void MTreeView::SetItemFontColor( MTreeItem* pItem, MCOLOR& color)
{
	if ( pItem == NULL)
		return;

	pItem->SetFontColor( color);
}


// SetItemHeight
void MTreeView::SetItemHeight( MTreeItem* pItem, int height)
{
	if ( pItem == NULL)
		return;

	pItem->SetHeight( height);

	// Update scrollbar
	m_bScrollUpdate = true;
}

void MTreeView::SetItemHeight( int height)
{
	if ( m_nItemHeight == height)
		return;

	m_nItemHeight = height;

	// Update scrollbar
	m_bScrollUpdate = true;
}

// GetItemHeight
int MTreeView::GetItemHeight()
{
	return m_nItemHeight;
}


// SetCollapsedImage
void MTreeView::SetCollapsedImage( const char* szAliasName)
{
	m_pImage[ MCOLLAPSED] = MBitmapManager::Get( szAliasName);
}

void MTreeView::SetCollapsedImage( MBitmap* pBitmap)
{
	m_pImage[ MCOLLAPSED] = pBitmap;
}


// GetCollapsedImage
MBitmap* MTreeView::GetCollapsedImage()
{
	return m_pImage[ MCOLLAPSED];
}


// SetExpandedImage
void MTreeView::SetExpandedImage( const char* szAliasName)
{
	m_pImage[ MEXPAND] = MBitmapManager::Get( szAliasName);
}

void MTreeView::SetExpandedImage( MBitmap* pBitmap)
{
	m_pImage[ MEXPAND] = pBitmap;
}


// GetExpandedImage
MBitmap* MTreeView::GetExpandedImage()
{
	return m_pImage[ MEXPAND];
}


// AddIcon
int MTreeView::AddIcon( const char* szAliasName)
{
	MBitmap* pIcon = MBitmapManager::Get( szAliasName);
	return AddIcon( pIcon);
}

int MTreeView::AddIcon( MBitmap* pIcon)
{
	if ( pIcon == NULL)
		return -1;

	m_vIcon.push_back( pIcon);

	return (int)( m_vIcon.size() - 1);
}


// GetIcon
MBitmap* MTreeView::GetIcon( size_t index)
{
	return m_vIcon[ index];
}

size_t MTreeView::GetIconCount()
{
	return m_vIcon.size();
}


// ClearIcon
void MTreeView::ClearIcon()
{
	if ( m_vIcon.empty())
		return;

	size_t _size = m_vIcon.size();
	for ( size_t i = 0;  i < _size;  i++)
		delete m_vIcon[ i];

	m_vIcon.clear();
}



// SetSel
void MTreeView::SelectItem( MTreeItem* pItem)
{
	if ( pItem == NULL)
		return;

	m_pSelItem = pItem;
}


// GetSel
MTreeItem* MTreeView::GetSelectedItem()
{
	return m_pSelItem;
}


// SetAlign
void MTreeView::SetAlign( int align)
{
	m_nAlign = align;
}

// GetAlign
MAlignmentMode MTreeView::GetAlign() const
{
	return m_nAlign;
}


// GetCount
int MTreeView::GetCount()
{
	int nCount = 0;

	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		nCount += m_vItemList[ i]->GetCount();

	return nCount;
}


// GetVisibledItemHeight
int MTreeView::GetVisibledItemHeight()
{
	int height = 0;

	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		height += m_vItemList[ i]->GetVisibledItemHeight( m_nItemHeight);

	return height;
}


// OnEvent
bool MTreeView::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;

	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		{
			if ( GetClientRect().InPoint( pEvent->Pos) == false)
				return false;

			MTREEHITINFO info = FindSelItemPos( pEvent->Pos);
			
			if ( info.m_pItem  &&  info.m_nHitRegion > MHTNOWHERE)
			{
				if ( info.m_nHitRegion == MHTEXPAND)
				{
					info.m_pItem->SetExpand( !info.m_pItem->IsExpanded());

					// Update scrollbar
					m_bScrollUpdate = true;
				}
				else if ( info.m_pItem != m_pSelItem)
				{
					m_pSelItem = info.m_pItem;

					EnsureSelItemVisible();
					
					// Event
					OnItemSelected( pEvent);
				}
			}

			return true;
		}

	case MWM_KEYDOWN :
		{
			switch ( pEvent->nKey)
			{
			case VK_UP :		SelPrevItem();			break;
 			case VK_DOWN :		SelNextItem();			break;
			case VK_PRIOR :		SelPrevPageItem();		break;
 			case VK_NEXT :		SelNextPageItem();		break;
 			case VK_HOME :		SelFirstItem();			break;
 			case VK_END :		SelLastItem();			break;
			}

			return true;
		}

	case MWM_MOUSEWHEEL :
		{
			if ( GetClientRect().InPoint( pEvent->Pos))
			{
				ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
				return true;
			}
		}
	}

	return false;
}


// OnCommand
bool MTreeView::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	return false;
}


// OnRun
void MTreeView::OnRun( void)
{
	// Update scrollbar
	if ( m_bScrollUpdate == true)
		UpdateScrollbar();
}


// GetWorkRect
MRECT MTreeView::GetWorkRect()
{
	MRECT cr = GetClientRect();
	if ( m_pScrollbar->IsVisible() == true)
		cr.w -= m_pScrollbar->GetWidth() + 2;

	return cr;
}


// SelPrevItem
void MTreeView::SelPrevItem( bool bPostEvent/*=true*/)
{
	if ( m_pSelItem == NULL)	return;

	// 현재 선택된 아이템의 위치 좌표를 얻어와서
	MPOINT pos( 0, 0);
	int x = 0, y = 0;
	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		if ( m_vItemList[ i]->GetItemPos( m_pSelItem, m_nItemHeight, pos, x, y) == true)	break;

	// 위 좌표에 있는 아이템을 구한다
	x = 0;	y = 0;	pos.y = max( 0, pos.y - 1);
	MTreeItem* pItem = NULL;
	for ( size_t i = 0;  i < _size;  i++)
		if ( pItem = m_vItemList[ i]->GetItemPos( m_nItemHeight, pos, x, y))	break;

	if ( pItem  &&  pItem != m_pSelItem)
	{
		m_pSelItem = pItem;
		
		EnsureSelItemVisible();

		if ( bPostEvent)
		{
			MEvent e;
			OnItemSelected( &e);
		}
	}
}

// SelNextItem
void MTreeView::SelNextItem( bool bPostEvent/*=true*/)
{
	if ( m_pSelItem == NULL)	return;

	// 현재 선택된 아이템의 위치 좌표를 얻어와서
	MPOINT pos( 0, 0);
	int x = 0, y = 0;
	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		if ( m_vItemList[ i]->GetItemPos( m_pSelItem, m_nItemHeight, pos, x, y) == true)	break;

	// 아래 좌표에 있는 아이템을 구한다
	x = 0;	y = 0;	pos.y += m_pSelItem->GetHeight() ? m_pSelItem->GetHeight() : m_nItemHeight;
	MTreeItem* pItem = NULL;
	for ( size_t i = 0;  i < _size;  i++)
		if ( pItem = m_vItemList[ i]->GetItemPos( m_nItemHeight, pos, x, y))	break;

	if ( pItem  &&  pItem != m_pSelItem)
	{
		m_pSelItem = pItem;

		EnsureSelItemVisible();

		if ( bPostEvent)
		{
			MEvent e;
			OnItemSelected( &e);
		}
	}
}

// SelPrevPageItem
void MTreeView::SelPrevPageItem( bool bPostEvent/*=true*/)
{
	if ( m_pSelItem == NULL)	return;

	// 현재 선택된 아이템의 위치 좌표를 얻어와서
	MPOINT pos( 0, 0);
	int x = 0, y = 0;
	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		if ( m_vItemList[ i]->GetItemPos( m_pSelItem, m_nItemHeight, pos, x, y) == true)	break;

	// 위 좌표에 있는 아이템을 구한다
	x = 0;	y = 0;	pos.y = max( 0, pos.y - GetWorkRect().h);
	MTreeItem* pItem = NULL;
	for ( size_t i = 0;  i < _size;  i++)
		if ( pItem = m_vItemList[ i]->GetItemPos( m_nItemHeight, pos, x, y))	break;

	if ( pItem  &&  pItem != m_pSelItem)
	{
		m_pSelItem = pItem;

		EnsureSelItemVisible();

		if ( bPostEvent)
		{
			MEvent e;
			OnItemSelected( &e);
		}
	}
}

// SelNextPageItem
void MTreeView::SelNextPageItem( bool bPostEvent/*=true*/)
{
	if ( m_pSelItem == NULL)	return;

	// 현재 선택된 아이템의 위치 좌표를 얻어와서
	MPOINT pos( 0, 0);
	int x = 0, y = 0;
	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		if ( m_vItemList[ i]->GetItemPos( m_pSelItem, m_nItemHeight, pos, x, y) == true)	break;

	// 아래 좌표에 있는 아이템을 구한다
	x = 0;	y = 0;	pos.y += GetWorkRect().h;
	MTreeItem* pItem = NULL;
	for ( size_t i = 0;  i < _size;  i++)
		if ( pItem = m_vItemList[ i]->GetItemPos( m_nItemHeight, pos, x, y))	break;

	if ( pItem == NULL)
	{
		SelLastItem( bPostEvent);
	}
	else if ( pItem != m_pSelItem)
	{
		m_pSelItem = pItem;

		EnsureSelItemVisible();

		if ( bPostEvent)
		{
			MEvent e;
			OnItemSelected( &e);
		}
	}
}

// SelFirstItem
void MTreeView::SelFirstItem( bool bPostEvent/*=true*/)
{
	if ( m_vItemList.empty())
		return;

	MTreeItem* pItem = GetRootItem( 0);
	if ( pItem  &&  pItem != m_pSelItem)
	{
		m_pSelItem = pItem;

		m_pScrollbar->SetValue( 0);

		if ( bPostEvent)
		{
			MEvent e;
			OnItemSelected( &e);
		}
	}
}

// SelLastItem
void MTreeView::SelLastItem( bool bPostEvent/*=true*/)
{
	if ( m_vItemList.empty())
		return;

	MTreeItem* pItem = NULL;
	MPOINT pos( 0, max( 0, GetVisibledItemHeight() - 1));
	int x = 0, y = 0;
	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		if ( pItem = m_vItemList[ i]->GetItemPos( m_nItemHeight, pos, x, y))	break;

	if ( pItem  &&  pItem != m_pSelItem)
	{
		m_pSelItem = pItem;

		m_pScrollbar->SetValue( m_pScrollbar->GetMax());

		if ( bPostEvent)
		{
			MEvent e;
			OnItemSelected( &e);
		}
	}
}


// EnsureSelItemVisible
void MTreeView::EnsureSelItemVisible()
{
	if ( m_pSelItem == NULL)	return;

	// 현재 선택된 아이템의 위치 좌표를 얻어와서
	MPOINT pos( 0, 0);
	int x = 0, y = 0;
	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
		if ( m_vItemList[ i]->GetItemPos( m_pSelItem, m_nItemHeight, pos, x, y) == true)	break;

	
	// 화면 위로 넘어갔는지 검사
	int column = m_pSelItem->GetHeight() ? m_pSelItem->GetHeight() : m_nItemHeight;
	if ( m_pScrollbar->GetValue() > (pos.y))	m_pScrollbar->SetValue( pos.y);

	// 화면 아래로 넘어갔는지 검사
	else
	{
		MRECT wr = GetWorkRect();
		int _y = pos.y + column + column;
		int _h = wr.h + m_pScrollbar->GetValue();

		if ( _y > _h)
			m_pScrollbar->SetValue( pos.y - wr.h + column + column);
	}
}


// ShowScrollbar
void MTreeView::ShowScrollbar( int mode)
{
	if ( m_nScrollbarVisible == mode)
		return;

	m_nScrollbarVisible = mode;

	switch ( mode)
	{
	case MSB_HIDE :
	case MSB_AUTO :
		m_pScrollbar->Show( false);
		break;

	case MSB_SHOW :
		m_pScrollbar->Show( true);
		break;
	}
}


// SetScrollbarLook
void MTreeView::SetScrollbarLook( MScrollBarLook* pLook)
{
	m_pScrollbar->ChangeCustomLook( pLook);
}


// SetScrollbarWidth
void MTreeView::SetScrollbarWidth( int nWidth)
{
	m_pScrollbar->SetWidth( nWidth);
}


// GetScrollbar
MScrollBar* MTreeView::GetScrollbar()
{
	return m_pScrollbar;
}


// ScrollPage
void MTreeView::ScrollPage( int delta)
{
	int _value = m_pScrollbar->GetValue() + delta;

	if ( _value > m_pScrollbar->GetMax())	m_pScrollbar->SetValue( m_pScrollbar->GetMax());
	else if ( _value < 0)					m_pScrollbar->SetValue( 0);

	m_pScrollbar->SetValue( _value);
}


// UpdateScrollbar
void MTreeView::UpdateScrollbar()
{
	int visibled_h = GetVisibledItemHeight();
	int h = GetClientRect().h;

	if ( visibled_h > h)
	{
		int _val = m_pScrollbar->GetValue();
		m_pScrollbar->SetRange( 0, visibled_h - h + m_nItemHeight);
		if ( _val > m_pScrollbar->GetMax())		m_pScrollbar->SetValue( m_pScrollbar->GetMax());
		else									m_pScrollbar->SetValue( _val);


		switch ( m_nScrollbarVisible)
		{
		case MSB_HIDE :
			m_pScrollbar->Show( false);
			break;

		case MSB_AUTO :
		case MSB_SHOW :
			m_pScrollbar->Show( true);
			break;
		}
	}
	else
	{
		m_pScrollbar->SetRange( 0, 0);
		m_pScrollbar->SetValue( 0);

		switch ( m_nScrollbarVisible)
		{
		case MSB_HIDE :
		case MSB_AUTO :
			m_pScrollbar->Show( false);
			break;

		case MSB_SHOW :
			m_pScrollbar->Show( true);
			break;
		}
	}

	m_bScrollUpdate = false;
}


// FindSelItemPos
MTREEHITINFO MTreeView::FindSelItemPos( MPOINT& pPos)
{
	MTREEHITINFO info;
	MPOINT pos = pPos;
	int dx = 0, dy = -m_pScrollbar->GetValue();

	size_t _size = m_vItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		info = m_vItemList[ i]->GetSelItem( m_nItemHeight, pos, dx, dy);
		MTreeItem* pItem = info.m_pItem;

		if ( pItem)
		{
			MRECT rect( 0, 0, 0, 0);

			// Set expand hit position
			if ( pItem->HasChild())
			{
				rect = MRECT( 0, 0, m_nItemHeight, m_nItemHeight);

				if ( pItem->IsExpanded())
				{
					if ( m_pImage[ MEXPAND])				GetColumnRatio( m_pImage[ MEXPAND], rect, pItem->GetHeight() ? pItem->GetHeight() : m_nItemHeight, m_nAlign);
					if ( rect.InPoint( info.m_ptHitPos))	info.m_nHitRegion = MHTEXPAND;
				}
				else
				{
					if ( m_pImage[ MCOLLAPSED])				GetColumnRatio( m_pImage[ MCOLLAPSED], rect, pItem->GetHeight() ? pItem->GetHeight() : m_nItemHeight, m_nAlign);
					if ( rect.InPoint( info.m_ptHitPos))	info.m_nHitRegion = MHTEXPAND;
				}
			}

			// Set icon hit position
			if ( info.m_nHitRegion != MHTEXPAND  &&  pItem->GetIcon() >= 0  &&  pItem->GetIcon() < (int)m_vIcon.size())
			{
				if ( m_vIcon[ info.m_pItem->GetIcon()])
				{
					rect = MRECT( rect.x + rect.w, rect.y, m_nItemHeight, m_nItemHeight);
					GetColumnRatio( m_vIcon[ info.m_pItem->GetIcon()], rect, pItem->GetHeight() ? pItem->GetHeight() : m_nItemHeight, m_nAlign);
					if ( rect.InPoint( info.m_ptHitPos))
						info.m_nHitRegion = MHTICON;
				}
			}

			break;
		}
	}

	return info;
}



}	// namespace mint3