#include "stdafx.h"
#include "MColorTable.h"
#include "MScrollBar.h"
#include "XUICustomLook.h"


namespace mint3
{


// XListCtrlCustomLook
XListCtrlCustomLook::XListCtrlCustomLook()
{
	m_pSmallFont = NULL;
	m_nItemBkgrndRectExpand = 1;
}

void XListCtrlCustomLook::OnDrawItems( MListCtrl* pListCtrl, MDrawContext* pDC)
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
			size_t _size = pListCtrl->GetItemCount();
			int x = 0, y = 0;

			// Draw item background
			int _item = _start;
			while ( 1)
			{
				MRECT _r( x + _hmargin - m_nItemBkgrndRectExpand, y + _vmargin - m_nItemBkgrndRectExpand,
							m_LargeImageSize.w + (m_nItemBkgrndRectExpand << 1), _item_height - (_vmargin << 1) + (m_nItemBkgrndRectExpand << 1));

				OnDrawItemBkgrnd( pListCtrl, _item, 0, _r, pDC);
				pListCtrl->EventDrawItemBkgrnd( pDC, _item, 0, _r);

				_item++;
				x += _item_size.w;
				if ( x >= _line_width)
				{
					x = 0;
					y += _item_size.h;
					if ( y > r.h)	break;
				}
			}

			// Draw items
			x = 0;
			y = 0;
			size_t _column_count = pListCtrl->GetColumnCount();
			for ( size_t i = _start;  i < _size;  i++)
			{
				MRECT _r( x + _hmargin, y + _vmargin, m_LargeImageSize.w, _item_height - (_vmargin << 1));

				if ( pListCtrl->EventPreDrawItem( pDC, i, 0, _r) == false)
				{
					OnDrawImage( pListCtrl, i, _r, pDC);
					pListCtrl->EventDrawItem( pDC, i, 0, _r);
				}

				if ( pListCtrl->GetSel( i) == true)
					OnDrawSelectbar( pListCtrl, i, MRECT( _r.x - 1, _r.y - 1, _r.w + 2, _r.h + 2), pDC);

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
			size_t _size = pListCtrl->GetItemCount();
			size_t _scroll = pListCtrl->GetScrollBar()->GetScrollValue();
			int y = 0;

			// Draw item background
			int _item = _scroll;
			while ( 1)
			{
				MRECT _r( _hmargin - m_nItemBkgrndRectExpand, y + _vmargin - m_nItemBkgrndRectExpand, m_SmallImageSize.w + (m_nItemBkgrndRectExpand << 1), m_SmallImageSize.h + (m_nItemBkgrndRectExpand << 1));
				OnDrawItemBkgrnd( pListCtrl, _item, 0, _r, pDC);
				pListCtrl->EventDrawItemBkgrnd( pDC, _item, 0, _r);

				_r.x = _r.x + _r.w;
				_r.w = r.w - _r.x;
				OnDrawItemBkgrnd( pListCtrl, _item, 1, _r, pDC);
				pListCtrl->EventDrawItemBkgrnd( pDC, _item, 1, _r);

				_item++;
				y += _item_height;
				if ( y >= r.h)			break;
			}

			// Draw items
			y = 0;
			for ( size_t i = _scroll;  i < _size;  i++)
			{
				MRECT _r( _hmargin, y + _vmargin, r.w - (_hmargin << 1), _item_height - (_vmargin << 1));

				if ( pListCtrl->EventPreDrawItem( pDC, i, 0, _r) == false)
				{
					if ( pListCtrl->GetItemImage( i) != NULL)
						OnDrawImage( pListCtrl, i, MRECT( _r.x, _r.y + ((_r.h > 0) ? ((_r.h - m_SmallImageSize.h) >> 1) : 0), m_SmallImageSize.w, m_SmallImageSize.h), pDC);

					OnDrawItemText( pListCtrl, i, 0, MRECT( _r.x + m_SmallImageSize.w + 5, _r.y, _r.w - m_SmallImageSize.w, _r.h), GetTextAlign(), pDC);

					pListCtrl->EventDrawItem( pDC, i, 0, _r);
				}

				if ( pListCtrl->GetSel( i) == true)		OnDrawSelectbar( pListCtrl, i, MRECT( 0, y, r.w, _item_height), pDC);
				if ( pListCtrl->GetCaretPos() == i)		OnDrawCaret( pListCtrl, i, MRECT( 0, y, r.w, _item_height), pDC);


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
			size_t _size = pListCtrl->GetItemCount();
			size_t _scroll = pListCtrl->GetScrollBar()->GetScrollValue();
			int y = 0;

			// Draw column header
			if ( pListCtrl->IsShowColumnHeader() == true)
				OnDrawColumnHeader( pListCtrl, MRECT( 0, y, r.w, pListCtrl->GetColumnHeaderHeight()), pDC);

			// Draw item background
			int _item = _scroll;
			y = ( pListCtrl->IsShowColumnHeader() == true) ? pListCtrl->GetColumnHeaderHeight() + 2 : 0;
			while ( 1)
			{
				MRECT _r( _hmargin - m_nItemBkgrndRectExpand, y + _vmargin - m_nItemBkgrndRectExpand, m_SmallImageSize.w + (m_nItemBkgrndRectExpand << 1), m_SmallImageSize.h + (m_nItemBkgrndRectExpand << 1));
				OnDrawItemBkgrnd( pListCtrl, _item, 0, _r, pDC);

				_r.x = _r.x + _r.w + (_hmargin << 1);
				_r.w = r.w - _r.x;
				OnDrawItemBkgrnd( pListCtrl, _item, 1, _r, pDC);

				_item++;
				y += _item_height;
				if ( y >= r.h)			break;
			}

			// Draw items
			y = ( pListCtrl->IsShowColumnHeader() == true) ? pListCtrl->GetColumnHeaderHeight() + 2 : 0;
			size_t _column_count = pListCtrl->GetColumnCount();
			for ( size_t i = _scroll;  i < _size;  i++)
			{
				int x = 0;
				for ( size_t j = 0;  j < _column_count;  j++)
				{
					int _w = pListCtrl->GetColumn( j)->GetWidth();
					MRECT _r( x + _hmargin, y + _vmargin, _w - (_hmargin << 1), _item_height - (_vmargin << 1));

					if ( j > 0)
					{
						_r.h = _r.h >> 1;
						_r.y += _r.h - 1;
					}

					pListCtrl->EventDrawItemBkgrnd( pDC, i, j, _r);

					if ( pListCtrl->EventPreDrawItem( pDC, i, j, _r) == false)
					{
						if ( j == 0)
						{
							if ( pListCtrl->GetItemImage( i) != NULL)
								OnDrawImage( pListCtrl, i, MRECT( _r.x, _r.y + ((_r.h > 0) ? ((_r.h - m_SmallImageSize.h) >> 1) : 0), m_SmallImageSize.w, m_SmallImageSize.h), pDC);

							OnDrawItemText( pListCtrl, i, j, MRECT( _r.x + m_SmallImageSize.w + 5, _r.y + 1, _r.w - m_SmallImageSize.w, _r.h >> 1), GetTextAlign(), pDC);
						}
						else
							OnDrawItemText( pListCtrl, i, j, _r, pListCtrl->GetColumn( j)->GetAlign(), pDC);

						pListCtrl->EventDrawItem( pDC, i, j, _r);
					}


					if ( j == 0)	x = m_SmallImageSize.w + 5;
					else			x += _w;
				}

				if ( pListCtrl->GetSel( i) == true)
					OnDrawSelectbar( pListCtrl, i, MRECT( 0, y, r.w, _item_height), pDC);

				if ( pListCtrl->GetCaretPos() == i)
					OnDrawCaret( pListCtrl, i, MRECT( 0, y, r.w, _item_height), pDC);


				y += _item_height;
				if ( y >= r.h)			break;
			}

			break;
		}
	}
}

void XListCtrlCustomLook::OnDrawItemText( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, int align, MDrawContext* pDC)
{
	MCOLOR _color;
	bool bHasColor = pListCtrl->GetItemColor( index, subitem, _color);

	bool bEnableItem = pListCtrl->GetItemEnable( index);
	if ( bEnableItem == false)
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

#define ICONSIZE		18
#define COLOR_GOLD		MCOLOR( 230, 180, 20)
#define COLOR_SILVER	MCOLOR( 200, 200, 200)
#define COLOR_COPPER	MCOLOR( 180, 100, 50)

	string strText = pListCtrl->GetItemText( index, subitem);
	if ( strText.length() > 2  &&  strText.at( 0) == '$'  &&  strText.at( 1) == '$')
	{
		MBitmap* pIcon = MBitmapManager::Get( "iconCoin");
		if ( pIcon != NULL)
		{
			float fOpacity = pDC->GetOpacity();

			MFont* pFontValue = ( subitem > 0) ? GetSmallFont() : GetFont();
			MFont* pFontUnit = MFontManager::Get( "fntSmall");

			pDC->SetBitmap( pIcon);

			if ( bEnableItem == false)
			{
				pDC->SetOpacity( fOpacity * 0.3f);
				pDC->Draw( r.x, r.y + ((r.h - ICONSIZE) >> 1), ICONSIZE, ICONSIZE);
			}
			else
				pDC->Draw( r.x, r.y + ((r.h - ICONSIZE) >> 1), ICONSIZE, ICONSIZE);

			int x = r.x + ICONSIZE;
			int y = r.y + ((r.h - pDC->GetFont()->GetHeight()) >> 1);
			size_t _pos = 2;
			while ( true)
			{
				size_t _find = strText.find( " ", _pos);
				string _str = ( _find == string::npos)  ?  strText.substr( _pos, strText.length() - _pos)  :  strText.substr( _pos, _find - _pos);

				if ( _str.empty() == false)
				{
					string _val = _str.substr( 0, _str.length() - 2);
					string _unit = _str.substr( _str.length() - 2);

					MCOLOR _col;
					switch ( _unit.at( 0))
					{
					case 'G' :		_col = COLOR_GOLD;		break;
					case 'S' :		_col = COLOR_SILVER;	break;
					case 'C' :		_col = COLOR_COPPER;	break;
					}

					int _h = pFontValue->GetHeight();
					pDC->SetColor( _col);
					pDC->SetFont( pFontValue);
					pDC->Text( x, y, 100, _h, _val.c_str(), MAM_LEFT | MAM_BOTTOM);
					x += pFontValue->GetWidth( _val.c_str()) + 2;

					pDC->SetFont( pFontUnit);
					pDC->Text( x, y, 100, _h, _unit.c_str(), MAM_LEFT | MAM_BOTTOM);
					x += pFontUnit->GetWidth( _unit.c_str()) + 5;
				}

				if ( _find == string::npos)		break;
				_pos = _find + 1;
			}


			pDC->SetOpacity( fOpacity);
			return;
		}
	}


	pDC->SetFont( ( subitem > 0) ? GetSmallFont() : GetFont());
	pDC->Text( r, pListCtrl->GetItemText( index, subitem), align);
}

void XListCtrlCustomLook::OnDrawItemBkgrnd( MListCtrl* pListCtrl, size_t index, size_t subitem, MRECT& r, MDrawContext* pDC)
{
	Draw( pDC, r, MWP_ITEMBKGRND, pListCtrl->GetState());
}
















// XCategoryCtrlCustomLook
XCategoryCtrlCustomLook::XCategoryCtrlCustomLook()
{
	m_pSmallFont = NULL;
	m_nItemBkgrndRectExpand = 1;
}

void XCategoryCtrlCustomLook::OnDrawItems( MCategoryCtrl* pCategory, MDrawContext* pDC)
{
	int _hmargin = 1, _vmargin = 3;
	int _indent = GetIndent();

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
				OnDrawCategoryBkgrnd( pCategory, _cat, MRECT( x, y, r.w, _h_category), pDC);

				// Draw select bar
				if ( pCategory->GetSel( _cat, _item) == true)
					OnDrawSelectbar( pCategory, _cat, _item, MRECT( x, y, r.w, _h_category), pDC);

				// Draw category
				MRECT _r( x, y, r.w, _h_category);
				if ( pCategory->GetCategoryExpand( _cat) == true)
					OnDrawExpandIcon( pCategory, _cat, MRECT( x + 5, y + ((_h_category - m_ExpandIconSize.h) >> 1), m_ExpandIconSize.w, m_ExpandIconSize.h), pDC);
				else
					OnDrawCollapsedIcon( pCategory, _cat, MRECT( x + 5, y + ((_h_category - m_ExpandIconSize.h) >> 1), m_ExpandIconSize.w, m_ExpandIconSize.h), pDC);

				OnDrawCategory( pCategory, _cat, MRECT( x + 7 + m_ExpandIconSize.w, y, r.w - 5 - m_ExpandIconSize.w, _h_category), pDC);

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
				// Draw item background
				{
					MRECT _r( x + _indent + _hmargin - m_nItemBkgrndRectExpand, y + _vmargin - m_nItemBkgrndRectExpand, m_ImageSize.w + (m_nItemBkgrndRectExpand << 1), m_ImageSize.h + (m_nItemBkgrndRectExpand << 1));
					OnDrawItemBkgrnd( pCategory, _cat, _item, 0, _r, pDC);

					_r.x = _r.x + _r.w + (_hmargin << 1);
					_r.w = r.w - _r.x;
					OnDrawItemBkgrnd( pCategory, _cat, _item, 0, _r, pDC);
				}


				// Draw select bar
				if ( pCategory->GetSel( _cat, _item) == true)
					OnDrawSelectbar( pCategory, _cat, _item, MRECT( x, y, r.w, _h_item), pDC);

				// Draw items
				MRECT _r( x + _hmargin + GetIndent(), 0, 0, ( _h_item - (_vmargin << 1)) >> 1);
				size_t _size_col = pCategory->GetColumnCount();
				for ( size_t _sub = 0;  _sub < _size_col;  _sub++)
				{
					int _align = pCategory->GetColumn( _sub)->GetAlign();

					if ( _sub == 0)
					{
						_r.y = y + _vmargin;
						_r.w = r.w - m_ImageSize.w;
					}
					else
					{
						_r.y = y + _vmargin + _r.h - 1;
						_r.w = pCategory->GetColumn( _sub)->GetWidth();
					}

					if ( _sub == 0  &&  pCategory->GetItemImage( _cat, _item) != NULL)
					{
						OnDrawImage( pCategory, _cat, _item, MRECT( _r.x, _r.y, m_ImageSize.w, m_ImageSize.h), pDC);

						_r.x += m_ImageSize.w + 5;
						OnDrawItemText( pCategory, _cat, _item, _sub, MRECT( _r.x, _r.y + 1, _r.w - m_ImageSize.w, _r.h), _align, pDC);
					}
					else
					{
						OnDrawItemText( pCategory, _cat, _item, _sub, _r, _align, pDC);

						_r.x += _r.w;
					}
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

void XCategoryCtrlCustomLook::OnDrawItemText( MCategoryCtrl* pCatrgory, size_t category, size_t item, size_t subitem, MRECT& r, int align, MDrawContext* pDC)
{
	MCOLOR _color;
	bool bHasColor = pCatrgory->GetItemColor( category, item, subitem, _color);

	bool bEnableItem = pCatrgory->GetItemEnable( category, item);
	if ( bEnableItem == false)
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

#define ICONSIZE		18
#define COLOR_GOLD		MCOLOR( 230, 180, 20)
#define COLOR_SILVER	MCOLOR( 200, 200, 200)
#define COLOR_COPPER	MCOLOR( 180, 100, 50)

	string strText = pCatrgory->GetItemText(category, item, subitem);
	if ( strText.length() > 2  &&  strText.at( 0) == '$'  &&  strText.at( 1) == '$')
	{
		MBitmap* pIcon = MBitmapManager::Get( "iconCoin");
		if ( pIcon != NULL)
		{
			float fOpacity = pDC->GetOpacity();

			MFont* pFontValue = ( subitem > 0) ? GetSmallFont() : GetFont();
			MFont* pFontUnit = MFontManager::Get( "fntSmall");

			pDC->SetBitmap( pIcon);
			pDC->SetFont( ( subitem > 0) ? GetSmallFont() : GetFont());

			if ( bEnableItem == false)
			{
				pDC->SetOpacity( fOpacity * 0.3f);
				pDC->Draw( r.x, r.y + ((r.h - ICONSIZE) >> 1), ICONSIZE, ICONSIZE);
			}
			else
				pDC->Draw( r.x, r.y + ((r.h - ICONSIZE) >> 1), ICONSIZE, ICONSIZE);

			int x = r.x + ICONSIZE;
			int y = r.y + ((r.h - pDC->GetFont()->GetHeight()) >> 1);
			size_t _pos = 2;
			while ( true)
			{
				size_t _find = strText.find( " ", _pos);
				string _str = ( _find == string::npos)  ?  strText.substr( _pos, strText.length() - _pos)  :  strText.substr( _pos, _find - _pos);

				if ( _str.empty() == false)
				{
					string _val = _str.substr( 0, _str.length() - 2);
					string _unit = _str.substr( _str.length() - 2);

					MCOLOR _col;
					switch ( _unit.at( 0))
					{
					case 'G' :		_col = COLOR_GOLD;		break;
					case 'S' :		_col = COLOR_SILVER;	break;
					case 'C' :		_col = COLOR_COPPER;	break;
					}

					pDC->SetColor( _col);
					pDC->SetFont( pFontValue);
					pDC->Text( x, y, _val.c_str());
					x += pFontValue->GetWidth( _val.c_str()) + 2;

					pDC->SetFont( pFontUnit);
					pDC->Text( x, y + pFontValue->GetHeight() - pFontUnit->GetHeight(), _unit.c_str());
					x += pFontUnit->GetWidth( _unit.c_str()) + 5;
				}

				if ( _find == string::npos)		break;
				_pos = _find + 1;
			}


			pDC->SetOpacity( fOpacity);
			return;
		}
	}

	pDC->SetFont( ( subitem > 0) ? GetSmallFont() : GetFont());
	pDC->Text( r, pCatrgory->GetItemText( category, item, subitem), align);
}

void XCategoryCtrlCustomLook::OnDrawCategoryBkgrnd( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, "category", pCategory->GetState()) == false)
	{
		pDC->SetColor( MColorTable::SELECTBAR[ pCategory->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}

void XCategoryCtrlCustomLook::OnDrawCategory( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
{
	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( "category", pCategory->GetState(), MColorTable::TEXT_SELECTED));
	pDC->Text( r, pCategory->GetCategoryText( category), MAM_LEFT | MAM_VCENTER);
}

void XCategoryCtrlCustomLook::OnDrawItemBkgrnd( MCategoryCtrl* pCategory, size_t category, size_t item, size_t subitem, MRECT& r, MDrawContext* pDC)
{
	Draw( pDC, r, MWP_ITEMBKGRND, pCategory->GetState());
}

void XCategoryCtrlCustomLook::OnDrawCaret( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_CARET, pCategory->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::BORDER[ pCategory->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void XCategoryCtrlCustomLook::OnDrawSelectbar( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_SELECTBAR, pCategory->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::SELECTBAR[ pCategory->GetStateNum()]);
		pDC->FillRectangle( r);
	}
}

void XCategoryCtrlCustomLook::OnDrawExpandIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
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

void XCategoryCtrlCustomLook::OnDrawCollapsedIcon( MCategoryCtrl* pCategory, size_t category, MRECT& r, MDrawContext* pDC)
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

void XCategoryCtrlCustomLook::OnDrawImage( MCategoryCtrl* pCategory, size_t category, size_t item, MRECT& r, MDrawContext* pDC)
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














// XResizableFrameCustomLook
XResizableFrameCustomLook::XResizableFrameCustomLook()
{
}


void XResizableFrameCustomLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	if ( (pWidget->IsCapture() == true  ||  pWidget->IsMouseOver() == true)  &&
		( pWidget->IsMovable() == true  ||  pWidget->IsResizable() == true))
		MFrameLook::OnDrawBorder( pWidget, pDC);
}


} // namespace mint3


