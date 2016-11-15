#include "stdafx.h"
#include "MSelector.h"
#include "MColorTable.h"
#include "Mint.h"



namespace mint3
{



// MSelector
MSelector::MSelector( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MSelectorLook);

	SetEnableFocus( true);

	m_nStyle = MSES_HORZ;
	m_nCurSel = -1;
	m_nMouseOver = -1;
}

MSelector::~MSelector()
{
	ResetContent();
}


// AddItem
int MSelector::AddItem( const char* szString, double fVal)
{
	MSelectorItem* pItem = new MSelectorItem;
	pItem->strString = szString;
	pItem->fValue = fVal;

	m_ItemList.push_back( pItem);

	return (int)(m_ItemList.size() - 1);
}


// InsertItem
int MSelector::InsertItem( size_t nIndex, const char* szString, double fVal)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= _size)		return AddItem( szString, fVal);


	vector< MSelectorItem*>::iterator itr = m_ItemList.begin();
	for ( size_t i = 0;  i < _size;  i++, itr++) ;


	MSelectorItem* pItem = new MSelectorItem;
	pItem->strString = szString;
	pItem->fValue = fVal;

	m_ItemList.insert( itr, pItem);

	return nIndex;
}


// DeleteItem
int MSelector::DeleteItem( size_t nIndex)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= _size)	return -1;


	vector< MSelectorItem*>::iterator itr = m_ItemList.begin();
	for ( size_t i = 0;  i != nIndex;  i++, itr++) ;


	delete m_ItemList[ nIndex];
	m_ItemList.erase( itr);

	return (int)nIndex;
}


// ResetContent
void MSelector::ResetContent()
{
	if ( m_ItemList.empty() == false)
	{
		size_t _size = m_ItemList.size();
		for ( size_t i = 0;  i < _size;  i++)	delete m_ItemList[ i];
		m_ItemList.clear();
	}

	m_nCurSel = -1;
}


// SetCurSel
int MSelector::SetCurSel( int nIndex)
{
	size_t _size = m_ItemList.size();
	if ( nIndex >= (int)_size)	nIndex = -1;


	if ( nIndex < 0)			m_nCurSel = -1;
	else						m_nCurSel = nIndex;

	return m_nCurSel;
}


// GetValueItem
int MSelector::GetItemFromValue( double fVal)
{
	size_t _size = m_ItemList.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		double _min = m_ItemList[ i]->fValue - 0.0001;
		double _max = m_ItemList[ i]->fValue + 0.0001;

		if ( fVal > _min  &&  fVal < _max)
			return i;
	}

	return -1;
}


// SetItemString
void MSelector::SetItemString( size_t nIndex, const char* szString)
{
	m_ItemList[ nIndex]->strString = szString;
}


// GetItemString
const char* MSelector::GetItemString( size_t nIndex)
{
	return m_ItemList[ nIndex]->strString.c_str();
}


// SetItemValue
void MSelector::SetItemValue( size_t nIndex, double fVal)
{
	m_ItemList[ nIndex]->fValue = fVal;
}


// GetItemValue
double MSelector::GetItemValue( size_t nIndex) const
{
	return m_ItemList[ nIndex]->fValue;
}


// ItemFromPoint
int MSelector::ItemFromPoint( const MPOINT& pt)
{
	MRECT r = GetClientRect();
	size_t _size = m_ItemList.size();
	if ( _size == 0)			return -1;

	if ( m_nStyle == MSES_HORZ)
	{
		if ( pt.y < r.y  ||  pt.y > (r.y + r.h)  ||  pt.x < r.x)	return -1;

		int _len;
		if ( _size == 1)	_len = r.w;
		else				_len = (int)( (float)(r.w - r.h) / (float)(_size - 1) );

		if ( (pt.x % _len) <= r.h)		return (int)( (float)(pt.x - r.x) / (float)_len);
	}
	else
	{
		if ( pt.x < r.x  ||  pt.x > (r.x + r.w)  ||  pt.y < r.y)	return -1;

		int _len;
		if ( _size == 1)	_len = r.h;
		else				_len = (int)( (float)(r.h - r.w) / (float)(_size - 1) );

		if ( (pt.y % _len) <= r.w)		return (int)( (float)(pt.y - r.y) / (float)_len);
	}

	return -1;
}


// OnEvent
bool MSelector::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent(pEvent, pListener) == true)	return true;

	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONUP:
		{
			int nSel = ItemFromPoint( pEvent->Pos);
			if ( nSel >= 0  &&  nSel != m_nCurSel)
			{
				m_nCurSel = nSel;

				OnSelChanged();
			}
			break;
		}

	case MWM_MOUSEMOVE :
		{
			m_nMouseOver = ItemFromPoint( pEvent->Pos);
			break;
		}
	}

	return false;
}


// OnLeave
void MSelector::OnLeave()
{
	if ( m_nMouseOver >= 0)		MGetMint()->GetToolTipManager()->Reset();
	m_nMouseOver = -1;

	MWidget::OnLeave();
}


// OnSetCursor
bool MSelector::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	if ( IsEnabled() == true  &&  m_bStatic == false  &&  m_nMouseOver >= 0)
		*pCursor = MCURSOR_HAND;

	return MWidget::OnSetCursor( pEvent, pCursor);
}








// MSelectorLook
MSelectorLook::MSelectorLook()
{
	SetTextAlign( MAM_RIGHT | MAM_BOTTOM);
}


void MSelectorLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MSelector* pSelector = (MSelector*)pWidget;

	MFont* pFont = GetFont();
	int nAlign = GetTextAlign();
	int nStyle = pSelector->GetStyle();
	MRECT r = pSelector->GetClientRect();

	MRECT _br( r.x, r.y, 0, 0);
	MRECT _tr( r.x, r.y, 100, 100);
	size_t _size = pSelector->GetItemCount();
	int _gap = 0;

	if ( nStyle == MSES_HORZ)
	{
		_br.w = r.h;
		_br.h = r.h;
		_tr.x = (_br.w - _tr.w) >> 1;
		_tr.y = (nAlign & MAM_TOP)  ?  (r.y - _tr.h)  :  (r.y + r.h);

		if ( _size == 0)		_gap = r.w;
		else if ( _size == 1)	_gap = r.w - _br.w;
		else					_gap = (int)( (float)(r.w - (_br.w * _size)) / (float)(_size - 1) );
	}
	else
	{
		_br.w = r.w;
		_br.h = r.w;
		_tr.x = (nAlign & MAM_LEFT)  ?  (r.y - _tr.x)  :  (r.x + r.w);
		_tr.h = r.w;

		if ( _size == 0)		_gap = r.h;
		else if ( _size == 1)	_gap = r.h - _br.h;
		else					_gap = (int)( (float)(r.h - (_br.h * _size)) / (float)(_size - 1) );
	}


	// Draw track
	if ( _size == 0)
	{
		OnDrawTrack( pSelector, r, pDC);
	}
	else
	{
		for ( size_t i = 0;  i < _size;  i++)
		{
			if ( nStyle == MSES_HORZ)
			{
				int _x = r.x + _br.w + (_br.w + _gap) * i;
				if ( _x <= (r.w - _br.w))		OnDrawTrack( pSelector, MRECT( _x, _br.y, _gap, _br.h), pDC);
			}
			else
			{
				int _y = r.y + _br.h + (_br.h + _gap) * i;
				if ( _y <= (r.h - _br.h))		OnDrawTrack( pSelector, MRECT( _br.x, _y, _br.w, _gap), pDC);
			}
		}
	}


	// Draw items
	pDC->SetFont( pFont);

	for ( size_t i = 0;  i < _size;  i++)
	{
		OnDrawItem( pSelector, i, _br, pDC);
		OnDrawItemText( pSelector, i, _tr, pDC);

		if ( nStyle == MSES_HORZ)
		{
			_br.x += _br.w + _gap;
			_tr.x += _br.w + _gap;
		}
		else
		{
			_br.y += _br.h + _gap;
			_tr.y += _br.h + _gap;
		}
	}
}


void MSelectorLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
}


void MSelectorLook::OnDrawItem( MSelector* pSelector, size_t index, MRECT& r, MDrawContext* pDC)
{
	// Draw border
	string strPart;
	if ( index == 0)										strPart = "border_begin";
	else if ( index == (pSelector->GetItemCount() - 1))		strPart = "border_end";
	else													strPart = MWP_BORDER;

	if ( Draw( pDC, r, strPart, pSelector->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::BKGRND);
		pDC->FillRectangle( r);
		pDC->SetColor( MColorTable::BORDER[ pSelector->GetStateNum()]);
		pDC->Rectangle( r);
	}


	// Draw caret
	if ( index == pSelector->GetCurSel())
	{
		if ( Draw( pDC, r, MWP_CARET, pSelector->GetState()) == false)
		{
			// Draw default
			pDC->SetColor( MColorTable::SELECTBAR[ pSelector->GetStateNum()]);
			pDC->FillRectangle( r.x + (r.w >> 2), r.y + (r.h >> 2), (r.w >> 1) + 1, (r.h >> 1) + 1);
		}
	}

	// Draw select bar
	else if ( index == pSelector->GetMouseOver())
	{
		if ( Draw( pDC, r, MWP_SELECTBAR, pSelector->GetState()) == false)
		{
			// Draw default
			MCOLOR _col = MColorTable::SELECTBAR[ pSelector->GetStateNum()];
			pDC->SetColor( _col.r, _col.g, _col.b, _col.r >> 1);
			pDC->FillRectangle( r.x + (r.w >> 2), r.y + (r.h >> 2), (r.w >> 1) + 1, (r.h >> 1) + 1);
		}
	}
}


void MSelectorLook::OnDrawItemText( MSelector* pSelector, size_t index, MRECT& r, MDrawContext* pDC)
{
	const char* szString = pSelector->GetItemString( index);

	const char* szState = MWS_NORMAL;
	int nStateNum = MWSN_NORMAL;
	if ( index == pSelector->GetCurSel()  ||  index == pSelector->GetMouseOver())
	{
		szState = MWS_FOCUS;
		nStateNum = MWSN_FOCUS;
	}
	pDC->SetColor( GetFontColor( MWP_BORDER, szState, MColorTable::TEXT[ nStateNum]));

	int nAlign = GetTextAlign();
	if ( pSelector->GetStyle() == MSES_HORZ)	nAlign = (nAlign & MAM_TOP)  ?  MAM_BOTTOM | MAM_HCENTER  :  MAM_TOP | MAM_HCENTER;
	else										nAlign = (nAlign & MAM_LEFT)  ?  MAM_RIGHT | MAM_VCENTER  :  MAM_LEFT | MAM_VCENTER;

	pDC->Text( r, szString, nAlign);
}


void MSelectorLook::OnDrawTrack( MSelector* pSelector, MRECT& r, MDrawContext* pDC)
{
	int nStyle = pSelector->GetStyle();

	if ( Draw( pDC, r, (nStyle == MSES_HORZ  ? MWP_HORZTRACK : MWP_VERTTRACK), pSelector->GetState()) == false)
	{
		// Draw default
		MRECT _r = r;
		if ( nStyle == MSES_HORZ)
		{
			_r.y = r.y + (r.h >> 2);
			_r.h = r.h >> 1;
		}
		else
		{
			_r.x = r.x + (r.w >> 2);
			_r.w = r.w >> 1;
		}

		pDC->SetColor( MColorTable::BKGRND);
		pDC->FillRectangle( _r);
		pDC->SetColor( MColorTable::BORDER[ pSelector->GetStateNum()]);
		pDC->Rectangle( _r);
	}
}


} // namespace mint3
