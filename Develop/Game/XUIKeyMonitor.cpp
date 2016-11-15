#include "stdafx.h"
#include "XUIKeyMonitor.h"
#include "XGame.h"
#include "XController.h"


void DrawKeys( MDrawContext* pDC, XKeyBuffer key, unsigned int time, float gain, MRECT rect, bool bTrashBuff =false);
void DrawKeysBackgrnd( MDrawContext* pDC, XKeyBuffer key, unsigned int time, float sq_time, float gain, MRECT rect, bool bTrashBuff =false);


#define MAX_KEY_BUFF	7
XVirtualKey KeyBuff[ MAX_KEY_BUFF] = {	VKEY_LEFT, VKEY_RIGHT, VKEY_FORWARD, VKEY_BACKWARD, VKEY_ACTION, VKEY_ACTION2, VKEY_JUMP};
const wchar_t szKeyName[ MAX_KEY_BUFF][32] = { L"Left", L"Right", L"Forward", L"Backward", L"Action", L"Alt.Action", L"Jump"};
bool g_PressedKeys[ MAX_KEY_BUFF];


int GetKeyNum( unsigned int nKeyCode)
{
	switch ( nKeyCode)
	{
	case VKEY_LEFT :			return 0;
	case VKEY_RIGHT :			return 1;
	case VKEY_FORWARD :			return 2;
	case VKEY_BACKWARD :		return 3;
	case VKEY_ACTION :			return 4;
	case VKEY_ACTION2 :			return 5;
	case VKEY_JUMP :			return 6;
	}

	return -1;
}




XUIKeyMonitor::XUIKeyMonitor( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	m_nTime = 0;
	m_nViewSecond = 5;
}

XUIKeyMonitor::~XUIKeyMonitor()
{
}

void XUIKeyMonitor::OnDraw( MDrawContext* pDC)
{
	if ( global.ui->IsMouseVisible() == false)
		m_nTime = XGetNowTime();



	MRECT rect = GetClientRect();
	float gain = (float)rect.w / (float)( m_nViewSecond * 1000);

	int _h = (rect.h - pDC->GetFont()->GetHeight() - 5);
	int dh = _h / MAX_KEY_BUFF;
	int sh = dh - 2;


	pDC->SetColor( 255, 255, 255, 62);
	for ( int i = 0;  i < MAX_KEY_BUFF;  i++)
		pDC->FillRectangle( 0, dh * i, rect.w, dh - 2);

	float dx = 1000.0f * gain;
	pDC->SetColor( 255, 0, 0, 128);
	for ( int i = 0;  i < 1000;  i++)
	{
		int x = rect.w - (int)( dx * (float)i + 0.5f);
		if ( x < 0)
			break;
		pDC->Line( x, 0, x, _h);
	}

	memset( g_PressedKeys, false, MAX_KEY_BUFF);


	DrawKeysBackgrnd( pDC, gg.controller->GetKeyBuffer(), m_nTime, 0.4f, gain, rect);
	DrawKeys( pDC, gg.controller->GetKeyBuffer(), m_nTime, gain, rect);


	for ( int i = 0;  i < MAX_KEY_BUFF;  i++)
	{
		if ( g_PressedKeys[ i] == true)		pDC->SetColor( 255, 255, 255);
		else								pDC->SetColor( 255, 255, 255, 64);

		pDC->Text( MRECT( 0, dh * i, rect.w - 2, dh - 2), MLocale::ConvUTF16ToAnsi(szKeyName[ i]).c_str(), MAM_RIGHT | MAM_VCENTER);
	}


	int x = 0;
	int y = rect.h - 14;

	pDC->SetColor( 0, 255, 0);
	pDC->FillRectangle( x, y, 2, 12);
	pDC->Line( x, y, x + 4, y);
	pDC->Line( x, y + 12, x + 4, y + 12);
	x += 5;
	pDC->SetColor( 255, 255, 255);
	pDC->Text( x, y, ":Down");
	x += pDC->GetFont()->GetWidth( ":Down") + 20;

	pDC->SetColor( 0, 255, 0);
	pDC->FillRectangle( x + 2, y, 2, 12);
	pDC->Line( x, y, x + 4, y);
	pDC->Line( x, y + 12, x + 4, y + 12);
	x += 5;
	pDC->SetColor( 255, 255, 255);
	pDC->Text( x, y, ":Up");
	x += pDC->GetFont()->GetWidth( ":Up") + 20;
	
	pDC->SetColor( 0, 255, 0);
	pDC->FillRectangle( x, y + (12 >> 1) - 1 , 2, 2);
	x += 5;
	pDC->SetColor( 255, 255, 255);
	pDC->Text( x, y, ":Press");
	x += pDC->GetFont()->GetWidth( ":Press") + 20;


	x = rect.w - 80;
	pDC->SetColor( 128, 128, 128);
	pDC->FillRectangle( x, y, 12, 12);
	pDC->SetColor( 0, 0, 0);
	pDC->FillRectangle( x + 2, y + 5, 8, 2);
	pDC->FillRectangle( x + 5, y + 2, 2, 8);
	x += 17;
	pDC->SetColor( 128, 128, 128);
	pDC->FillRectangle( x, y, 12, 12);
	pDC->SetColor( 0, 0, 0);
	pDC->FillRectangle( x + 2, y + 5, 8, 2);

	if ( (timeGetTime() % 300) > 100)
	{
		x += 25;
		if ( global.ui->IsMouseVisible() == true)
		{
			pDC->SetColor( 255, 0, 0);
			pDC->FillRectangle( x, y, 40, 12);
			pDC->SetColor( 0, 0, 0);
			pDC->Text( MRECT(x, y, 40, 12), "Stop", MAM_HCENTER | MAM_VCENTER);
		}
		else
		{
			pDC->SetColor( 0, 255, 0);
			pDC->FillRectangle( x, y, 40, 12);
			pDC->SetColor( 0, 0, 0);
			pDC->Text( MRECT(x, y, 40, 12), "Play", MAM_HCENTER | MAM_VCENTER);
		}
	}
}

void DrawKeysBackgrnd( MDrawContext* pDC, XKeyBuffer key, unsigned int time, float sq_time, float gain, MRECT rect, bool bTrashBuff)
{
	int _h = (rect.h - pDC->GetFont()->GetHeight() - 5);
	int dh = _h / MAX_KEY_BUFF;
	int sh = dh - 2;


	for ( int i = 0;  i < key.GetSize();  i++)
	{
		KEYNODE node = key.Get( i);

		int nKey = GetKeyNum( node.nKeycode);
		if ( nKey < 0)
			continue;

		if ( nKey >= MAX_KEY_BUFF)
			continue;


		int x = rect.w - (int)( (float)( time - node.nTime) * gain + 0.5f);
		int y = dh * nKey;

		if ( x < -100)
			continue;

		if ( node.nState != KEYNODE::PRESSONCE)
			continue;


		int w = (int)( sq_time * 1000.0f * gain + 0.5f);
		pDC->SetColor( 0, 0, 255, 128);
		pDC->FillRectangle( x, y, w, sh);

		pDC->SetColor( 255, 255, 255, 32);
		pDC->Line( x, 0, x, _h);
	}
}

void DrawKeys( MDrawContext* pDC, XKeyBuffer key, unsigned int time, float gain, MRECT rect, bool bTrashBuff)
{
	int _h = (rect.h - pDC->GetFont()->GetHeight() - 5);
	int dh = _h / MAX_KEY_BUFF;
	int sh = dh - 2;


	for ( int i = 0;  i < key.GetSize();  i++)
	{
		KEYNODE node = key.Get( i);

		int nKey = GetKeyNum( node.nKeycode);
		if ( nKey < 0)
			continue;

		if ( nKey >= MAX_KEY_BUFF)
			continue;


		int x = rect.w - (int)( (float)( time - node.nTime) * gain + 0.5f);
		int y = dh * nKey;

		if ( x < -100)
			continue;

		pDC->SetColor( 0, 255, 0);
		if ( node.nState == KEYNODE::PRESSONCE)
		{
			pDC->FillRectangle( x, y, 2, sh);
			pDC->Line( x, y, x + 4, y);
			pDC->Line( x, y+sh-1, x + 4, y+sh-1);

			if ( (time - node.nTime) < 1000)
				g_PressedKeys[ nKey] = true;
		}

		else if ( node.nState == KEYNODE::PRESSED)
		{
			pDC->FillRectangle( x, y + (sh >> 1) - 1 , 2, 2);

			if ( (time - node.nTime) < 1000)
				g_PressedKeys[ nKey] = true;
		}

		else if ( node.nState == KEYNODE::RELEASED)
		{
			pDC->FillRectangle( x - 1, y, 2, sh);
			pDC->Line( x, y, x - 3, y);
			pDC->Line( x, y + sh - 1, x - 3, y + sh - 1);
		}
	}
}

bool XUIKeyMonitor::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;


	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		{
			MPOINT pt = pEvent->Pos;
			MRECT rectClient = GetClientRect();
			MRECT rectWindow = GetInitialClientRect();
			MRECT r;
			r.x = rectWindow.x + rectClient.w - 80;
			r.y = rectWindow.y + rectClient.h - 14;
			r.w = r.h = 12;

			if ( r.InPoint( pt) == true)
			{
				m_nViewSecond = max( 2, m_nViewSecond - 1);
				return true;
			}

			r.x += 17;
			if ( r.InPoint( pt) == true)
			{
				m_nViewSecond = min( 10, m_nViewSecond + 1);
				return true;
			}


			if ( rectClient.InPoint( pt) == true)
				return true;

			break;
		}
	}

	return false;
}
