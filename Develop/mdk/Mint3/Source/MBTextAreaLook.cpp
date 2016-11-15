/************************************************************************
MBTextAreaLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : TextArea 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBTextAreaLook.h"


// namespace : mint3
namespace mint3
{

/*	MBTextAreaLook
	생성자
*/
MBTextAreaLook::MBTextAreaLook()
{
	// 커스텀 Look 사용 안함
	m_bCustomLook = false;


	// 비트맵을 늘려서 찍음
	m_BitmapStyle = DRAWBITMAP_STRETCH;


	// 비트맵 이미지를 타일형으로 찍지 않음
	m_bTiled = false;


	// 폰트 초기화
	m_pFont = NULL;
}


/*	~MBTextAreaLook
	소멸자
*/
MBTextAreaLook::~MBTextAreaLook()
{
}


/*	SetBitmap
	프레임에 사용될 이미지를 설정하는 함수
*/
void MBTextAreaLook::SetBitmap( const string strFileName)
{
	m_Bitmaps[ 0].strBitmapName = strFileName;
	m_Bitmaps[ 1].strBitmapName = strFileName;
	m_Bitmaps[ 2].strBitmapName = strFileName;
}


/*	SetAssignRect
	프레임에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBTextAreaLook::SetAssignRect( int state, int index, float x1, float y1, float x2, float y2)
{
	// Check valid range
	if ( index >= 9)
		return;


	// Set bitmap assignment
	m_Bitmaps[ state].m_rCoord[ index].x = (int)x1;
	m_Bitmaps[ state].m_rCoord[ index].y = (int)y1;
	m_Bitmaps[ state].m_rCoord[ index].w = (int)x2;
	m_Bitmaps[ state].m_rCoord[ index].h = (int)y2;
}


/*	OnFrameDraw
	프레임을 그리는 함수
*/
void MBTextAreaLook::OnFrameDraw( MTextArea* pTextArea, MDrawContext* pDC)
{
	// 실제 클라이언트의 영역을 구합니다
	MRECT rect = pTextArea->GetRect();
	rect.x = rect.y = 0;


	// 리스트박스의 상태를 구합니다
	int state;
	if ( !pTextArea->IsEnable())		state = 2;
	else if ( pTextArea->IsFocus())		state = 1;
	else								state = 0;


	// 프레임을 그립니다
	DrawBitmapByStyle( pDC, rect, MBitmapManager::Get( m_Bitmaps[ state].strBitmapName.c_str()),
		m_Bitmaps[ state].m_rCoord, m_BitmapStyle, m_bTiled);
}


/*	SetStretchBlt
	비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBTextAreaLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


/*	GetClientRect
	클라이언트의 영역을 구하는 함수(텍스트를 입력하는 공간)
*/
MRECT MBTextAreaLook::GetClientRect( MTextArea* pTextArea, MRECT& r)
{
	if ( m_bCustomLook)
		return MRECT( r.x + 1,  r.y + 1,  r.w - 2,  r.h - 2);


	MRECT rect = r;
	rect.x += m_Bitmaps[ 0].m_rCoord[ 1].w;
	rect.y += m_Bitmaps[ 0].m_rCoord[ 1].h;
	rect.w -= m_Bitmaps[ 0].m_rCoord[ 1].w + m_Bitmaps[ 0].m_rCoord[ 3].w;
	rect.h -= m_Bitmaps[ 0].m_rCoord[ 1].h + m_Bitmaps[ 0].m_rCoord[ 6].h;


	return rect;
}


} // namespace mint3








/********************************************************************
여기서부터 이전 코드입니다

#include "stdafx.h"
#include "MBTextAreaLook.h"
#include "MBitmapDrawer.h"

namespace mint3 {

void MBTextAreaLook::OnFrameDraw(MTextArea* pTextArea, MDrawContext* pDC)
{
//	MRECT r = pTextArea->GetInitialClientRect();
//	DrawBitmapFrame9(pDC, r, m_pFrameBitmaps);

	MRECT r = pTextArea->GetInitialClientRect();
	pDC->SetColor(19,19,19,255);
	pDC->FillRectangle(r);
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);

}

MBTextAreaLook::MBTextAreaLook(void)
{
	m_pFont = NULL;
	for(int i=0; i<9; i++){
		m_pFrameBitmaps[i] = NULL;
	}
}

MRECT MBTextAreaLook::GetClientRect(MTextArea* pTextArea, MRECT& r)
{
	int al = GETWIDTH(m_pFrameBitmaps[3]);
	int au = GETHEIGHT(m_pFrameBitmaps[7]);
	int ar = GETWIDTH(m_pFrameBitmaps[5]);
	int ab = GETHEIGHT(m_pFrameBitmaps[1]);

	int nWidth = r.w-(al+ar);
	if(pTextArea->IsScrollBarVisible()) 
		nWidth-=pTextArea->GetScrollBarWidth();

	const int nMargin = 2;

	return MRECT(r.x+al + nMargin, r.y+au + nMargin, nWidth -nMargin*2, r.h-(au+ab) -nMargin*2);
}

} // namespace mint3
*/