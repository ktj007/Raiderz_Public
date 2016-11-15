#include "stdafx.h"
#include "MProgressCtrl.h"
#include "MColorTable.h"


namespace mint3
{



// MProgressCtrl
MProgressCtrl::MProgressCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MProgressCtrlLook);

	m_bStatic = true;

	m_nLower = 0;
	m_nUpper = 100;
	m_nBeginPos = 0;
	m_nEndPos = 0;
	m_bEnableSliding = false;

	m_tSliding.SetArrivalTime( 300);
}


// SetRange
void MProgressCtrl::SetRange( int nLower, int nUpper)
{
	if ( nLower > nUpper)								return;
	if ( nLower == m_nLower  &&  nUpper == m_nUpper)	return;

	m_nLower = nLower;
	m_nUpper = nUpper;

	if ( m_nEndPos < nLower)			m_nBeginPos = m_nEndPos = nLower;
	else if ( m_nEndPos > nUpper)		m_nBeginPos = m_nEndPos = nUpper;
}


// GetRange
void MProgressCtrl::GetRange( int& nLower, int& nUpper) const
{
	nLower = m_nLower;
	nUpper = m_nUpper;
}


// SetPos
int MProgressCtrl::SetPos( int nPos)
{
	nPos = max( m_nLower, nPos);
	nPos = min( m_nUpper, nPos);

	if ( nPos == m_nEndPos)		return nPos;

	m_nBeginPos = GetPos();
	m_nEndPos = nPos;

	m_tSliding.Start();

	return m_nEndPos;
}


// GetPos
int MProgressCtrl::GetPos() const
{
	int nPos = 0;
	
	if ( m_bEnableSliding == false)		nPos = m_nEndPos;
	else								nPos = m_nBeginPos + (int)( (double)(m_nEndPos - m_nBeginPos) * m_tSliding.GetRatio());

	return nPos;
}


// EnableSliding
void MProgressCtrl::EnableSliding( bool bEnable)
{
	m_bEnableSliding = bEnable;

	m_tSliding.SetEnable( bEnable);
}












// MProgressCtrlLook
MProgressCtrlLook::MProgressCtrlLook()
{
	m_nTextViewStyle = MPCTS_NONE;

	SetTextAlign( MAM_HCENTER | MAM_VCENTER);
}

void MProgressCtrlLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MProgressCtrl* pProgressCtrl = (MProgressCtrl*)pWidget;
	MRECT r = pProgressCtrl->GetClientRect();

	OnDrawGauge( pProgressCtrl, r, pDC);
	OnDrawText( pProgressCtrl, r, pDC);
}

void MProgressCtrlLook::OnDrawGauge( MProgressCtrl* pProgressCtrl, MRECT& r, MDrawContext* pDC)
{
	string strPart = MWP_PROGRESSBAR;
	string strState = pProgressCtrl->GetState();
	bool bValid = false;

	if ( IsValidAssign( strPart, strState) == true)
		bValid = true;

	else if ( strState != string( MWS_NORMAL))
	{
		if ( IsValidAssign( strPart, MWS_NORMAL) == true)
			bValid = true;
	}

	
	MRECT* pRect = GetAssignRect( strPart, strState);
	int nLower, nUpper, nPos;
	pProgressCtrl->GetRange( nLower, nUpper);
	nPos = pProgressCtrl->GetPos();
	int nLength = (int)( (float)r.w * ( (float)(nPos - nLower) / (float)(nUpper - nLower)) + 0.5f);


	if ( bValid == false)
	{
		pDC->SetColor( MColorTable::SELECTBAR[ MWSN_NORMAL]);
		pDC->FillRectangle( r.x, r.y, nLength + 1, r.h + 1);
		return;
	}


	pDC->SetBitmap( GetBitmap());

	
	// Draw head
	int x = r.x, y = r.y;
	int w = min( nLength, pRect[ 4].w);
	if ( pRect[ 1].h > 0)		pDC->Draw( x, y, w, pRect[ 1].h, pRect[ 1].x, pRect[ 1].y, w, pRect[ 1].h);
	if ( pRect[ 6].h > 0)		pDC->Draw( x, y + r.h - pRect[ 6].h, w, pRect[ 6].h, pRect[ 6].x, pRect[ 6].y, w, pRect[ 6].h);
	pDC->Draw( x, y + pRect[ 1].h, w, r.h - pRect[ 1].h - pRect[ 6].h, pRect[ 4].x, pRect[ 4].y, w, pRect[ 4].h);

	if ( nLength <= pRect[ 4].w)			return;


	// Draw body
 	x += pRect[ 4].w;
 	w = min( nLength, r.w - pRect[ 5].w) - pRect[ 4].w;
 	if ( pRect[ 2].h > 0)		pDC->Draw( x, y, w, pRect[ 2].h, pRect[ 2].x, pRect[ 2].y, pRect[ 2].w, pRect[ 2].h);
 	if ( pRect[ 7].h > 0)		pDC->Draw( x, y + r.h - pRect[ 7].h, w, pRect[ 7].h, pRect[ 7].x, pRect[ 7].y, pRect[ 7].w, pRect[ 7].h);
 	pDC->Draw( x, y + pRect[ 2].h, w, r.h - pRect[ 2].h - pRect[ 7].h, pRect[ 0].x, pRect[ 0].y, pRect[ 0].w, pRect[ 0].h);

	if ( nLength <= (r.w - pRect[ 5].w))	return;


	// Draw tail
	x += r.w - pRect[ 4].w - pRect[ 5].w;
	w = min( nLength, r.w) - (r.w - pRect[ 5].w);
	if ( pRect[ 3].h > 0)		pDC->Draw( x, y, w, pRect[ 3].h, pRect[ 3].x, pRect[ 3].y, w, pRect[ 3].h);
	if ( pRect[ 8].h > 0)		pDC->Draw( x, y + r.h - pRect[ 8].h, w, pRect[ 8].h, pRect[ 8].x, pRect[ 8].y, w, pRect[ 8].h);
	pDC->Draw( x, y + pRect[ 3].h, w, r.h - pRect[ 3].h - pRect[ 8].h, pRect[ 5].x, pRect[ 5].y, w, pRect[ 5].h);
}

void MProgressCtrlLook::OnDrawText( MProgressCtrl* pProgressCtrl, MRECT& r, MDrawContext* pDC)
{
	int nLower, nUpper, nPos;
	pProgressCtrl->GetRange( nLower, nUpper);
	nPos = pProgressCtrl->GetPos();

	char buff[ 128];
	switch ( m_nTextViewStyle)
	{
	case MPCTS_PERCENT :
		sprintf_s( buff, "%.1f%%", (float)(nPos - nLower) / (float)(nUpper - nLower) * 100.0f);
		break;

	case MPCTS_PROGRESS :
		sprintf_s( buff, "%d / %d", nPos, nUpper);
		break;

	default :
		return;
	}

	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( MWP_BORDER, pProgressCtrl->GetState(), MColorTable::TEXT[ MWSN_NORMAL]));
	pDC->Text( r, buff, GetTextAlign());
}


} // namespace mint3