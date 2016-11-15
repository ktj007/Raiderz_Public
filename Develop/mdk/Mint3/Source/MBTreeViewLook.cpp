
#include "stdafx.h"
#include "MBTreeViewLook.h"


// namespace : mint3
namespace mint3
{


// MBTreeViewLook
MBTreeViewLook::MBTreeViewLook()
{
	// 커스텀 Look 사용 안함
	m_bCustomLook = false;


	// 비트맵을 늘려서 찍음
	m_BitmapStyle = DRAWBITMAP_STRETCH;


	// 비트맵 이미지를 타일형으로 찍지 않음
	m_bTiled = false;


	// 폰트 초기화
	m_pFont = NULL;


	// 텍스트 색상 설정
	for ( int i = 0;  i < MTVR_MAX;  i++)
	{
		m_nFontColor[ i][ 0]	= MCOLOR( 200, 200, 200);		// Normal
		m_nFontColor[ i][ 1]	= MCOLOR( 255, 255, 255);		// Focused
		m_nFontColor[ i][ 2]	= MCOLOR( 150, 150, 150);		// Disabled
	}
}


// ~MBTreeViewLook
MBTreeViewLook::~MBTreeViewLook()
{
}


// SetBitmap
void MBTreeViewLook::SetBitmap( const string strFileName)
{
	for ( int i = 0;  i < MTVR_MAX;  i++)
	{
		m_Bitmaps[ i][ 0].strBitmapName = strFileName;
		m_Bitmaps[ i][ 1].strBitmapName = strFileName;
		m_Bitmaps[ i][ 2].strBitmapName = strFileName;
	}
}


// SetAssignRect
void MBTreeViewLook::SetAssignRect( int type, int state, int index, float x1, float y1, float x2, float y2)
{
	// Check valid range
	if ( index >= 9)
		return;


	// Set bitmap assignment
	m_Bitmaps[ type][ state].m_rCoord[ index].x = (int)x1;
	m_Bitmaps[ type][ state].m_rCoord[ index].y = (int)y1;
	m_Bitmaps[ type][ state].m_rCoord[ index].w = (int)x2;
	m_Bitmaps[ type][ state].m_rCoord[ index].h = (int)y2;
}


// SetStretchBlt
void MBTreeViewLook::SetStretchBlt( bool bStretch /*=true*/)
{
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


// OnDrawFrame
void MBTreeViewLook::OnDrawFrame( MTreeView* pTreeView, MDrawContext* pDC, MRECT& client)
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

	int state;
	if ( !pTreeView->IsEnable())		state = 2;
	else if ( pTreeView->IsFocus())		state = 1;
	else								state = 0;

	DrawBitmapByStyle( pDC, client, MBitmapManager::Get( m_Bitmaps[ MTVR_FRAMEBOX][ state].strBitmapName.c_str()),
		m_Bitmaps[ MTVR_FRAMEBOX][ state].m_rCoord, m_BitmapStyle, m_bTiled);
}


// OnDrawItem
void MBTreeViewLook::OnDrawItem( MTreeView* pTreeView, MTreeItem* pTreeItem, MDrawContext* pDC, MRECT& client, int x, int& y)
{
	// Get item column
	int column = pTreeItem->GetHeight() ? pTreeItem->GetHeight() : pTreeView->GetItemHeight();


	// Check valid rect
	if ( y <= 0  &&  (y + column) >= 0)		y = 0;
	else if ( y > client.h)					return;


	// Draw item
	int px = x, py = y;
	if ( y >= 0)
	{
		bool bSelected = (pTreeView->GetSelectedItem() == pTreeItem) ? true : false;

		// Draw select bar
		if ( bSelected == true)
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
		pDC->SetFont( pTreeItem->GetFont() ? pTreeItem->GetFont() : ( m_pFont ? m_pFont : pTreeView->GetFont()));
		MCOLOR _color;
		if ( pTreeView->IsEnable() == false)	_color = m_nFontColor[ MTVR_SELECTBAR][ 2];
		else if ( bSelected == true)			_color = m_nFontColor[ MTVR_SELECTBAR][ 1];
		else									_color = m_nFontColor[ MTVR_SELECTBAR][ 0];
		pDC->SetColor( pTreeItem->GetFontColor( _color));
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
void MBTreeViewLook::OnDrawSelectbar( MTreeView* pTreeView, MDrawContext* pDC, MRECT& r)
{
	int state;
	if ( pTreeView->IsEnable() == false)	state = 2;
	else if ( pTreeView->IsFocus())			state = 1;
	else									state = 0;

	MRECT wr = pTreeView->GetWorkRect();
	r.x = wr.x;
	r.w = wr.w;

	DrawBitmapByStyle( pDC, r, MBitmapManager::Get( m_Bitmaps[ MTVR_SELECTBAR][ state].strBitmapName.c_str()),
		m_Bitmaps[ MTVR_SELECTBAR][ state].m_rCoord, DRAWBITMAP_STRETCH);
}

							  
// GetClientRect
MRECT MBTreeViewLook::GetClientRect( MTreeView* pTreeView, MRECT& client)
{
	return client;
}


} // namespace mint3

