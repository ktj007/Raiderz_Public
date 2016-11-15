/************************************************************************
MBGroupLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Group 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBGroupLook.h"


// namespace : mint3
namespace mint3
{

/*	MBGroupLook
	생성자
*/
MBGroupLook::MBGroupLook()
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
	m_colorFont[ 0]	= MCOLOR( 200, 200, 200);		// Normal
	m_colorFont[ 1]	= MCOLOR( 255, 255, 255);		// Focused


	// 텍스트 위치 오프셋 설정
	m_ptTextOffset.x = 0;
	m_ptTextOffset.y = 0;
}


/*	~MBGroupLook
	소멸자
*/
MBGroupLook::~MBGroupLook()
{
}


/*	SetBitmap
	프레임에 사용될 이미지를 설정하는 함수
*/
void MBGroupLook::SetBitmap( const string strFileName)
{
	m_Bitmaps[ 0].strBitmapName = strFileName;
	m_Bitmaps[ 1].strBitmapName = strFileName;
}


/*	SetAssignRect
	프레임에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBGroupLook::SetAssignRect( int state, int index, float x1, float y1, float x2, float y2)
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


/*	OnDraw
	프레임을 그리는 함수
*/
void MBGroupLook::OnDraw( MGroup* pGroup, MDrawContext* pDC)
{
	// 프레임의 상태를 구합니다
	int state = pGroup->IsEnable()  ?  0 : 1;


	// 프레임을 그립니다
	MRECT rect = pGroup->GetClientRect();
	rect.y += m_ptTextOffset.y;
	rect.h -= m_ptTextOffset.y;
	DrawBitmapByStyle( pDC, rect, MBitmapManager::Get( m_Bitmaps[ state].strBitmapName.c_str()), m_Bitmaps[ state].m_rCoord, m_BitmapStyle, m_bTiled);


	// 텍스트 출력

	// 폰트 적용
	pDC->SetFont( m_pFont);


	// 텍스트를 그립니다
//	if ( pDC->BeginFont())
	{
		// 텍스트를 출력합니다
		pDC->SetColor( m_colorFont[ state]);
		pDC->Text( m_ptTextOffset.x, 0, pGroup->GetText());

//		pDC->EndFont();
	}
}


/*	SetStretchBlt
	비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBGroupLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


} // namespace mint3




















/****************************************************************************
여기서부터는 이전코드 입니다

#include "stdafx.h"
#include "MBGroupLook.h"
#include "MBitmapDrawer.h"

namespace mint3 {

void MBGroupLook::OnDraw(MGroup* pGroup, MDrawContext* pDC)
{
	MRECT r = pGroup->GetInitialClientRect();
	
	//DrawBitmapFrame9(pDC, r, m_pFrameBitmaps, false, m_bStretch);

	DrawBitmapFrameCustom1( pDC, r, m_pFrameBitmaps, m_bStretch );

	if(m_pFont!=NULL) pDC->SetFont(m_pFont);
	pDC->SetColor(m_FontColor);
	pDC->Text(r.x+m_TitlePosition.x, r.y+m_TitlePosition.y, pGroup->m_szText);
}

MBGroupLook::MBGroupLook(void)
{
	for(int i=0; i<FRAME_BITMAP_COUNT; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
	m_bStretch = true;
	m_TitlePosition = MPOINT(8, 4);
}

MRECT MBGroupLook::GetClientRect(MGroup* pGroup, MRECT& r)
{
//	int al = GETWIDTH(m_pFrameBitmaps[3]);
//	int au = GETHEIGHT(m_pFrameBitmaps[6]);
//	int ar = GETWIDTH(m_pFrameBitmaps[5]);
//	int ab = GETHEIGHT(m_pFrameBitmaps[0]);
//	return MRECT(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
}

} // namespace mint3
*/
