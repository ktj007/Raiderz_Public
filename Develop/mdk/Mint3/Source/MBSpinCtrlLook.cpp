/************************************************************************
MBSpinCtrlLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : SpinCtrl 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBSpinCtrlLook.h"


// namespace : mint3
namespace mint3
{

/*	MBSpinCtrlLook
	생성자
*/
MBSpinCtrlLook::MBSpinCtrlLook()
{
	// 커스텀 Look 사용 안함
	m_bCustomLook = false;


	// 비트맵을 늘려서 찍음
	m_BitmapStyle = DRAWBITMAP_STRETCH;


	// 비트맵 이미지를 타일형으로 찍지 않음
	m_bTiled = false;

}


/*	~MBSpinCtrlLook
	소멸자
*/
MBSpinCtrlLook::~MBSpinCtrlLook()
{
}


/*	SetBitmap
	프레임에 사용될 이미지를 설정하는 함수
*/
void MBSpinCtrlLook::SetBitmap( const string strFileName)
{
	for ( int i = 0;  i < MSPR_MAX;  i++)
	{
		m_Bitmaps[ i][ 0].strBitmapName = strFileName;
		m_Bitmaps[ i][ 1].strBitmapName = strFileName;
		m_Bitmaps[ i][ 2].strBitmapName = strFileName;
	}
}


/*	SetAssignRect
	프레임에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBSpinCtrlLook::SetAssignRect( int type, int state, int index, float x1, float y1, float x2, float y2)
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


/*	OnDraw
	프레임을 그리는 함수
*/
void MBSpinCtrlLook::OnDraw( MSpinCtrl* pSpinCtrl, MDrawContext* pDC)
{
	// 스핀컨트롤의 상태를 구합니다
	int state;
	bool bEnable = pSpinCtrl->IsEnable();


	if ( pSpinCtrl->GetType() == MSPT_VERTICAL)
	{
		// 윗 버튼을 그립니다
		if ( !bEnable)	state = 2;
		else if ( pSpinCtrl->GetPushedRgn() == MSPR_UPARROW)	state = 1;
		else			state = 0;
		DrawBitmapByStyle( pDC, pSpinCtrl->GetUpArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSPR_UPARROW][ state].strBitmapName.c_str()),
			m_Bitmaps[ MSPR_UPARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);

		// 아래 버튼을 그립니다
		if ( !bEnable)	state = 2;
		else if ( pSpinCtrl->GetPushedRgn() == MSPR_DOWNARROW)		state = 1;
		else			state = 0;
		DrawBitmapByStyle( pDC, pSpinCtrl->GetDownArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSPR_DOWNARROW][ state].strBitmapName.c_str()),
			m_Bitmaps[ MSPR_DOWNARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);
	}

	else
	{
		// 왼쪽 버튼을 그립니다
		if ( !bEnable)	state = 2;
		else if ( pSpinCtrl->GetPushedRgn() == MSPR_LEFTARROW)		state = 1;
		else			state = 0;
		DrawBitmapByStyle( pDC, pSpinCtrl->GetLeftArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSPR_LEFTARROW][ state].strBitmapName.c_str()),
			m_Bitmaps[ MSPR_LEFTARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);

		// 오른쪽 버튼을 그립니다
		if ( !bEnable)	state = 2;
		else if ( pSpinCtrl->GetPushedRgn() == MSPR_RIGHTARROW)	state = 1;
		else			state = 0;
		DrawBitmapByStyle( pDC, pSpinCtrl->GetRightArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSPR_RIGHTARROW][ state].strBitmapName.c_str()),
			m_Bitmaps[ MSPR_RIGHTARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);
	}
}


/*	SetStretchBlt
비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBSpinCtrlLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


} // namespace mint3
