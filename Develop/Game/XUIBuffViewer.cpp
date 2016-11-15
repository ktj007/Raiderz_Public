#include "stdafx.h"
#include "XUIBuffViewer.h"
#include "MColorTable.h"
#include "XPost_Comm.h"
#include "XStrings.h"

#define SIZE_COOLTIME_EXPAND		2


// GetBuffPos
MPOINT GetBuffPos( XBuffViewerItem* pItem, int _buffcount)
{
	MPOINT _pos;
	MRECT _r = pItem->GetParent()->GetClientRect();
	XBuffViewerLook* pLook = (XBuffViewerLook*)pItem->GetLookAndFill();
	MSIZE _icon_size = pLook->GetIconSize();
	MSIZE _item_size = MSIZE( _icon_size.w + 10, _icon_size.h + SIZE_COOLTIME_EXPAND + pLook->GetSmallFont()->GetHeight());

	XBuffInfo* pBuffInfo = info.buff->Get( pItem->GetID());
	if ( pBuffInfo->IsDebuff())
	{
		_pos.x = _r.w - _item_size.w - _icon_size.w - (_buffcount * (_item_size.w + 5));
		_pos.y = _icon_size.h + (_item_size.h + 5) + 5;
	}
	else
	{
		_pos.x = _r.w - _item_size.w - _icon_size.w - (_buffcount * (_item_size.w + 5));
		_pos.y = _icon_size.h;
	}

	return _pos;
}






// XBuffViewerItem
XBuffViewerItem::XBuffViewerItem( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_nID = 0;
	m_bDelete = false;
	m_bEnableFocus = true;

	m_tFade.SetArrivalTime( 300);
	m_tMove.SetArrivalTime( 300);
}


// ~XBuffViewerItem
XBuffViewerItem::~XBuffViewerItem()
{
}


// Create
void XBuffViewerItem::Create( int nID)
{
	XBuffInfo* pBuffInfo = info.buff->Get( nID);
	if ( pBuffInfo == NULL)		return;


	m_nID = nID;

	MRECT _r = m_pParent->GetClientRect();
	MSIZE _icon_size = ((XBuffViewerLook*)m_pLookAndFill)->GetIconSize();
	MSIZE _item_size = MSIZE( _icon_size.w + 10, _icon_size.h + SIZE_COOLTIME_EXPAND + ((XBuffViewerLook*)m_pLookAndFill)->GetSmallFont()->GetHeight());
	MPOINT _pos = GetBuffPos( this, 0);
	m_Rect.x = _pos.x;
	m_Rect.y = _pos.y;
	m_Rect.w = _item_size.w;
	m_Rect.h = _item_size.h;

	m_StartPos = m_EndPos = MPOINT( _pos.x, _pos.y);
	

	wstring strToolTip;
	strToolTip += L"{BITMAP name=\"bmpItemSlot\" w=42 h=42}{CR h=1}{SPACE w=1 h=1}{BITMAP name=\"";
	strToolTip += pBuffInfo->m_szIcon;
	strToolTip += L"\" w=40 h=40}{CR h=0}{INDENT dent=45}{COLOR r=230 g=210 b=80}{FONT name=\"fntBoldOutline\"}";
	strToolTip += pBuffInfo->GetName();
	strToolTip += L"{/FONT}{/COLOR}{CR}{CR h=3}";
	strToolTip += pBuffInfo->GetDesc();
	SetToolTipText( MLocale::ConvUTF16ToAnsi(strToolTip.c_str()).c_str());

	m_tFade.Start();
}


// Delete
void XBuffViewerItem::Delete()
{
	if ( m_tFade.IsReversed() == true)		return;

	m_tFade.SetReverse( true);
	m_tFade.Start();
}


// MoveTo
void XBuffViewerItem::MoveTo( MPOINT& pos)
{
	if ( m_tMove.IsArrival() == true)
	{
		m_StartPos = m_EndPos;
		m_EndPos = pos;

		m_tMove.Start();
	}
	else
		m_EndPos = pos;
}


// OnRun
void XBuffViewerItem::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( m_bDelete == false  &&  m_tFade.IsReversed()  &&  m_tFade.IsArrival())
		m_bDelete = true;


	float fRatio = m_tMove.GetRatio();
	m_Rect.x = m_StartPos.x + (m_EndPos.x - m_StartPos.x) * fRatio;
	m_Rect.y = m_StartPos.y + (m_EndPos.y - m_StartPos.y) * fRatio;
}


// OnRClick
void XBuffViewerItem::OnRClick( MEvent* pEvent)
{
	MWidget::OnRClick( pEvent);

	XPostRequestBuffLost( m_nID);
}














// XUIBuffViewer
XUIBuffViewer::XUIBuffViewer( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( XBuffViewerLook);
	
	m_bStatic = true;
	m_bShowBorder = false;

	m_bRearrange = false;
}


// ~XUIBuffViewer
XUIBuffViewer::~XUIBuffViewer()
{
	Clear();
}


// Add
bool XUIBuffViewer::Add( int nID)
{
	XBuffInfo* pBuffInfo = info.buff->Get( nID);
	if ( pBuffInfo == NULL)		return false;


	// Add
	XBuffViewerItem* pItem = new XBuffViewerItem( NULL, this, this);
	pItem->SetLookAndFill( m_pLookAndFill);
	pItem->Create( nID);


	m_BuffList.push_back( pItem);

	m_bRearrange = true;

	return true;
}


// Remove
bool XUIBuffViewer::Remove( int nID)
{
	for ( list< XBuffViewerItem*>::iterator itr = m_BuffList.begin();  itr != m_BuffList.end();  itr++)
	{
		if ( (*itr)->GetID() == nID)
			(*itr)->Delete();
	}

	return true;
}


// Clear
void XUIBuffViewer::Clear()
{
	for ( list< XBuffViewerItem*>::iterator itr = m_BuffList.begin();  itr != m_BuffList.end();  itr++)
		delete (*itr);

	m_BuffList.clear();
}


// OnRun
void XUIBuffViewer::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Delete item
	for ( list< XBuffViewerItem*>::iterator itr = m_BuffList.begin();  itr != m_BuffList.end();  itr++)
	{
		XBuffViewerItem* pItem = (*itr);
		if ( pItem->IsDelete() == false)		continue;

		m_BuffList.erase( itr);
		delete pItem;

		m_bRearrange = true;

		break;
	}


	// Rearrange item
	if ( m_bRearrange == true)
	{
		MRECT r = GetClientRect();
		int nBuffCount = 0;
		int nDebuffCount = 0;

		for ( list< XBuffViewerItem*>::reverse_iterator itr = m_BuffList.rbegin();  itr != m_BuffList.rend();  itr++)
		{
			XBuffViewerItem* pItem = (*itr);
			if ( pItem->IsDelete() == true)		continue;


			XBuffInfo* pBuffInfo = info.buff->Get( pItem->GetID());
			switch ( pBuffInfo->m_nType)
			{
			case BUFT_BUFF :
			case BUFT_AURA :
			case BUFT_AURA_EFFECT_BUFF :
				{
					MPOINT pos = GetBuffPos( pItem, nBuffCount);
					pItem->MoveTo( pos);
					nBuffCount++;
				}
				break;

			case BUFT_DEBUFF :
			case BUFT_AURA_EFFECT_DEBUFF :
				{
					MPOINT pos = GetBuffPos( pItem, nDebuffCount);
					pItem->MoveTo( pos);
					nDebuffCount++;
				}
				break;
			}
		}

		m_bRearrange = false;
	}
}












XBuffViewerLook::XBuffViewerLook()
{
	m_pSmallFont = NULL;
	m_IconSize = MSIZE( 32, 32);
	m_pCooltimeImage = NULL;
	m_nCooltimeImgCellCount = 1;
	m_nCooltimeImgCellCountRow = 0;
	m_nCooltimeImgCellCountCol = 0;
	m_CooltimeImgSize = MSIZE( 0, 0);
}


void XBuffViewerLook::SliceCellCooltimeImg( int row, int col)
{
	if ( m_pCooltimeImage == NULL)		return;
	row = max( 0, row);
	col = max( 0, col);

	m_nCooltimeImgCellCountRow = row;
	m_nCooltimeImgCellCountCol = col;
	m_nCooltimeImgCellCount = row * col;

	m_CooltimeImgSize.w = m_pCooltimeImage->GetWidth() / row;
	m_CooltimeImgSize.h = m_pCooltimeImage->GetHeight() / col;
}


void XBuffViewerLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	if ( strcmp( pWidget->GetClassName(), MINT_BUFFVIEWER) == 0)		return;
	XBuffViewerItem* pBuffItem = (XBuffViewerItem*)pWidget;


	int nID = pBuffItem->GetID();
	XBuffInfo* pBuffInfo = info.buff->Get( nID);
	MRECT _r = pBuffItem->GetClientRect();
	MRECT _ir = MRECT( (_r.w - m_IconSize.w) >> 1, 0, m_IconSize.w, m_IconSize.h);


	// Draw cooltime image
	MTimer tFade = pBuffItem->GetFadeTimer();
	bool bActivate = ( tFade.IsReversed() == false  &&  tFade.IsArrival() == true);
	if ( bActivate == true)
	{
		MRECT _cr( _ir.x - SIZE_COOLTIME_EXPAND, _ir.y - SIZE_COOLTIME_EXPAND, _ir.w + (SIZE_COOLTIME_EXPAND << 1), _ir.h + (SIZE_COOLTIME_EXPAND << 1));


		// Black background
		pDC->SetColor( 0, 0, 0, 25);
		pDC->FillRectangle( _cr.x - 1, _cr.y - 1, _cr.w + 2, _cr.h + 2);


		// Colored background
		MCOLOR _col;
		if ( pBuffInfo->m_nType == BUFT_DEBUFF)		_col = GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 255, 255, 255));
		else										_col = GetFontColor( MWP_BORDER, MWS_DISABLE, MCOLOR( 255, 255, 255));

		uint32 nRemainedTime = gvar.MyInfo.BuffList.GetRemainedTime( nID);
		double _sin = sin( (double)MGetMint()->GetUpdateTime() * ( nRemainedTime < 3000 ? 0.02 : 0.005));
		_col.r = min( 255, _col.r + (int)(_sin * 40.0));
		_col.g = min( 255, _col.g + (int)(_sin * 40.0));
		_col.b = min( 255, _col.b + (int)(_sin * 40.0));

		pDC->SetColor( _col);
		pDC->FillRectangle( _cr);


		// Cooltime mask image
		float fRemainedRatio = gvar.MyInfo.BuffList.GetRemainedRatio( nID);
		int nIndex = (int)( fRemainedRatio * (float)m_nCooltimeImgCellCount);
		MRECT sr( (nIndex % m_nCooltimeImgCellCountRow) * m_CooltimeImgSize.w, (nIndex / m_nCooltimeImgCellCountRow) * m_CooltimeImgSize.h, m_CooltimeImgSize.w, m_CooltimeImgSize.h);

		float _old_opacity = pDC->SetOpacity( 0.8f);
		pDC->SetBitmap( GetCooltimeImage());
		pDC->Draw( _cr.x, _cr.y, _cr.w, _cr.h, sr.x, sr.y, sr.w, sr.h);
		pDC->SetOpacity( _old_opacity);
	}


	// Draw icon
	float fRatio = tFade.GetRatio();
	int nExpand = (int)( m_IconSize.w * (1.0f - fRatio));
	_ir.x -= nExpand;
	_ir.y -= nExpand;
	_ir.w += nExpand << 1;
	_ir.h += nExpand << 1;

	
	MBitmap* pIcon = global.ui->GetIcon(pBuffInfo->m_szIcon);
	float _old_opacity = pDC->SetOpacity( fRatio);
	
	if ( pIcon != NULL)
	{
		pDC->SetBitmap( pIcon);
		pDC->Draw( _ir);
	}
	else
	{
		pDC->SetColor( 0, 0, 255);
		pDC->FillRectangle( _ir);

		pDC->SetColor( 128, 128, 128);
		pDC->Rectangle( _ir.x, _ir.y, _ir.w - 1, _ir.h - 1);
	}

	pDC->SetOpacity( _old_opacity);


	if ( bActivate == true)
	{
		// Remained time
		long nRemainedTime = gvar.MyInfo.BuffList.GetRemainedTime( nID);
		if ( nRemainedTime >= 0)
		{
			char szBuff[ 16];

			if ( nRemainedTime > 3600000 /*1 hour*/)
			{
				string _unit = MLocale::ConvUTF16ToAnsi(XGetStr( L"UNIT_HOUR"));
				sprintf_s( szBuff, "%d%s", nRemainedTime / 3600000, _unit.c_str());
			}
			else if ( nRemainedTime > 60000 /*1 minute*/)
			{
				string _unit = MLocale::ConvUTF16ToAnsi(XGetStr( L"UNIT_MINUTE"));
				sprintf_s( szBuff, "%d%s", nRemainedTime / 60000, _unit.c_str());
			}
			else
			{
				string _unit = MLocale::ConvUTF16ToAnsi(XGetStr( L"UNIT_SECOND"));
				sprintf_s( szBuff, "%d%s", nRemainedTime / 1000, _unit.c_str());
			}


			MFont* pFont = GetFont();
			pDC->SetFont( pFont);
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255)));
			pDC->Text( MRECT( _ir.x - 4, _ir.y - 4, _ir.w + 4, _ir.h + 4), szBuff, MAM_LEFT | MAM_TOP);
		}

		// Stack Count
		int nStackCount = gvar.MyInfo.BuffList.GetItemCount(nID);
		if(nStackCount > 1)
		{
			char szBuff[16] = {0,};
			sprintf_s(szBuff, "%d", nStackCount);

			MFont* pFont = GetFont();
			pDC->SetFont( pFont);
			pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 255, 255, 255)));
			pDC->Text( MRECT( _ir.x - 4, _ir.y - 4, _ir.w + 4, _ir.h + 4), szBuff, MAM_RIGHT | MAM_BOTTOM);
		}
	}
}


void XBuffViewerLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	if ( strcmp( pWidget->GetClassName(), MINT_BUFFVIEWER) == 0)		return;
	XBuffViewerItem* pBuffItem = (XBuffViewerItem*)pWidget;

	
	MTimer tFade = pBuffItem->GetFadeTimer();
	if ( tFade.IsReversed() == true  ||  tFade.IsArrival() == false)	return;


	// Draw text
	XBuffInfo* pBuffInfo = info.buff->Get( pBuffItem->GetID());
	MRECT _r = pBuffItem->GetClientRect();
	int _x = 0;
	int _y = m_IconSize.h + SIZE_COOLTIME_EXPAND;


	MFont* pFont = GetSmallFont();
	pDC->SetFont( pFont);
	pDC->SetColor( GetFontColor( "title", MWS_NORMAL, MCOLOR( 255, 255, 255)));


	// 짧은 이름일 경우엔 가운데 출력
	int _w = pFont->GetWidth( MLocale::ConvTCHARToAnsi(pBuffInfo->GetName()).c_str());
	if ( _w <= _r.w)
	{
		_x = ( _r.w - _w) >> 1;
		pDC->Text( _x, _y, MLocale::ConvTCHARToAnsi(pBuffInfo->GetName()).c_str());
	}

	// 긴 이름일 경우엔 스크롤
	else
	{
		wstring strName;
		strName += pBuffInfo->GetName();

		_w = pFont->GetWidth( MLocale::ConvUTF16ToAnsi(strName.c_str()).c_str()) + 10;
		_x = -(int)((double)timeGetTime() * 0.025) % _w;

		pDC->Text( _x, _y, MLocale::ConvUTF16ToAnsi(strName.c_str()).c_str());
		pDC->Text( _x + _w, _y, MLocale::ConvUTF16ToAnsi(strName.c_str()).c_str());
	}
}

void XBuffViewerLook::SetSmallFont(const char* szFont)
{
	m_pSmallFont = MFontManager::Get( szFont);
}

void XBuffViewerLook::SetCooltimeImage(const char* szImage)
{
	m_pCooltimeImage = MBitmapManager::Get( szImage);
}