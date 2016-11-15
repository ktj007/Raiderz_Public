/************************************************************************
MBScrollBarLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : ScrollBar 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBScrollBarLook.h"


// namespace : mint3
namespace mint3
{

/*	MBScrollBarLook
	생성자
*/
MBScrollBarLook::MBScrollBarLook()
{
	// 커스텀 Look 사용 안함
	m_bCustomLook = false;


	// 비트맵을 늘려서 찍음
	m_BitmapStyle = DRAWBITMAP_STRETCH;


	// 비트맵 이미지를 타일형으로 찍지 않음
	m_bTiled = false;

}


/*	~MBScrollBarLook
	소멸자
*/
MBScrollBarLook::~MBScrollBarLook()
{
}


/*	SetBitmap
	프레임에 사용될 이미지를 설정하는 함수
*/
void MBScrollBarLook::SetBitmap( const string strFileName)
{
	for ( int i = 0;  i < MSBR_MAX;  i++)
	{
		m_Bitmaps[ i][ 0].strBitmapName = strFileName;
		m_Bitmaps[ i][ 1].strBitmapName = strFileName;
		m_Bitmaps[ i][ 2].strBitmapName = strFileName;
	}
}


/*	SetAssignRect
	프레임에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBScrollBarLook::SetAssignRect( int type, int state, int index, float x1, float y1, float x2, float y2)
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
void MBScrollBarLook::OnDraw( MScrollBar* pScrollBar, MDrawContext* pDC)
{
	// 스크롤바의 상태를 구합니다
	int state;
	bool bEnable = pScrollBar->IsEnable();


	// 배경을 그립니다
	if ( !bEnable)	state = 2;
	else if ( pScrollBar->GetPushedRgn() == MSBR_BKGRND)	state = 1;
	else			state = 0;
	DrawBitmapByStyle( pDC, pScrollBar->GetBackgroundRgn(), MBitmapManager::Get( m_Bitmaps[ MSBR_BKGRND][ state].strBitmapName.c_str()),
		m_Bitmaps[ MSBR_BKGRND][ state].m_rCoord, m_BitmapStyle, m_bTiled);


	if ( pScrollBar->IsShowArrow())
	{
		if ( pScrollBar->GetType() == MSBT_VERTICAL)
		{
			// 윗 버튼을 그립니다
			if ( !bEnable)	state = 2;
			else if ( pScrollBar->GetPushedRgn() == MSBR_UPARROW)	state = 1;
			else			state = 0;
			DrawBitmapByStyle( pDC, pScrollBar->GetUpArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSBR_UPARROW][ state].strBitmapName.c_str()),
				m_Bitmaps[ MSBR_UPARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);

			// 아래 버튼을 그립니다
			if ( !bEnable)	state = 2;
			else if ( pScrollBar->GetPushedRgn() == MSBR_DOWNARROW)		state = 1;
			else			state = 0;
			DrawBitmapByStyle( pDC, pScrollBar->GetDownArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSBR_DOWNARROW][ state].strBitmapName.c_str()),
				m_Bitmaps[ MSBR_DOWNARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);
		}

		else
		{
			// 왼쪽 버튼을 그립니다
			if ( !bEnable)	state = 2;
			else if ( pScrollBar->GetPushedRgn() == MSBR_LEFTARROW)		state = 1;
			else			state = 0;
			DrawBitmapByStyle( pDC, pScrollBar->GetLeftArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSBR_LEFTARROW][ state].strBitmapName.c_str()),
				m_Bitmaps[ MSBR_LEFTARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);

			// 오른쪽 버튼을 그립니다
			if ( !bEnable)	state = 2;
			else if ( pScrollBar->GetPushedRgn() == MSBR_RIGHTARROW)	state = 1;
			else			state = 0;
			DrawBitmapByStyle( pDC, pScrollBar->GetRightArrowRgn(), MBitmapManager::Get( m_Bitmaps[ MSBR_RIGHTARROW][ state].strBitmapName.c_str()),
				m_Bitmaps[ MSBR_RIGHTARROW][ state].m_rCoord, m_BitmapStyle, m_bTiled);
		}

	}


	// Thumb 버튼을 그립니다
	if ( !bEnable)	state = 2;
	else if ( pScrollBar->GetPushedRgn() == MSBR_THUMB)		state = 1;
	else			state = 0;
	DrawBitmapByStyle( pDC, pScrollBar->GetThumbRgn(), MBitmapManager::Get( m_Bitmaps[ MSBR_THUMB][ state].strBitmapName.c_str()),
		m_Bitmaps[ MSBR_THUMB][ state].m_rCoord, m_BitmapStyle, m_bTiled);
}


/*	SetStretchBlt
비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBScrollBarLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


} // namespace mint3
