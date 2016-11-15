/************************************************************************
MBListBoxLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : ListBox 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBListBoxLook.h"


// namespace : mint3
namespace mint3
{

/*	MBListBoxLook
	생성자
*/
MBListBoxLook::MBListBoxLook()
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
	for ( int i = 0;  i < MLBR_MAX;  i++)
	{
		m_colorFont[ i][ 0]	= MCOLOR( 200, 200, 200);		// Normal
		m_colorFont[ i][ 1]	= MCOLOR( 255, 255, 255);		// Focused
		m_colorFont[ i][ 2]	= MCOLOR( 150, 150, 150);		// Disabled
	}
}


/*	~MBListBoxLook
	소멸자
*/
MBListBoxLook::~MBListBoxLook()
{
}


/*	SetBitmap
	프레임에 사용될 이미지를 설정하는 함수
*/
void MBListBoxLook::SetBitmap( const string strFileName)
{
	for ( int i = 0;  i < MLBR_MAX;  i++)
	{
		m_Bitmaps[ i][ 0].strBitmapName = strFileName;
		m_Bitmaps[ i][ 1].strBitmapName = strFileName;
		m_Bitmaps[ i][ 2].strBitmapName = strFileName;
	}
}


/*	SetAssignRect
	프레임에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBListBoxLook::SetAssignRect( int type, int state, int index, float x1, float y1, float x2, float y2)
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


/*	OnHeaderDraw
	리스트 박스의 헤더를 그리는 함수
*/
void MBListBoxLook::OnHeaderDraw( MListBox* pListBox, MDrawContext* pDC, MRECT& r, const char* szText)
{
	// 텍스트가 없으면 그냥 리턴
	if ( szText == NULL)
		return;


	// 리스트 박스의 상태를 구한다
	int state;
	if ( !pListBox->IsEnable())			state = 2;
	else if ( pListBox->IsFocus())		state = 1;
	else								state = 0;

	// 헤더 뒷 배경을 그린다
	DrawBitmapByStyle( pDC, r, MBitmapManager::Get( m_Bitmaps[ MLBR_HEADER][ state].strBitmapName.c_str()),
					m_Bitmaps[ MLBR_HEADER][ state].m_rCoord, DRAWBITMAP_STRETCH);


	// 텍스트를 출력한다
	pDC->SetFont( m_pFont);
	pDC->SetColor( m_colorFont[ MLBR_HEADER][ state]);
	pDC->Text( r, szText);
}


/*	OnItemDraw
	리스트 박스의 선택 막대를 그리는 함수
*/
void MBListBoxLook::OnDrawSelectBar( MListBox* pListBox, MDrawContext* pDC, MRECT& r, bool bSelected, bool bEnabled, bool bFocus)
{
	// 리스트 박스의 상태를 구한다
	int state;
	if ( bEnabled == false)				state = 2;
	else if ( !pListBox->IsEnable())	state = 2;
	else if ( pListBox->IsFocus())		state = 1;
	else								state = 0;


	// 선택된 아이템일 경우엔 선택막대를 그린다
	if ( bSelected && bEnabled)
		DrawBitmapByStyle( pDC, r, MBitmapManager::Get( m_Bitmaps[ MLBR_SELECTBAR][ state].strBitmapName.c_str()),
		m_Bitmaps[ MLBR_SELECTBAR][ state].m_rCoord, DRAWBITMAP_STRETCH);
}

							  
/*	OnItemDraw
	리스트 박스의 아이템을 그리는 함수
*/
int MBListBoxLook::OnItemDraw( MListBox* pListBox, MDrawContext* pDC, MRECT& r, const char* szText, MCOLOR color, bool bSelected, bool bEnabled, bool bFocus, int nAdjustWidth)
{
	// 텍스트가 없으면 바로 종료
	if ( szText == NULL)
		return 0;


	// 리스트 박스의 상태를 구한다
	int state;
	if ( bEnabled == false)				state = 2;
	else if ( !pListBox->IsEnable())	state = 2;
	else if ( pListBox->IsFocus())		state = 1;
	else								state = 0;


	// 아이템 이미지를 그린다
	if( m_pItemSlotBitmap != NULL )
	{
		pDC->SetBitmap( m_pItemSlotBitmap );

		if ( bEnabled == false)
		{
			pDC->SetBitmapColor( 128, 128, 128);
			pDC->Draw(r.x, r.y, r.w, r.h );
			pDC->SetBitmapColor( 255, 255, 255);
		}
		else
			pDC->Draw(r.x, r.y, r.w, r.h );
	}


	// 텍스트 출력
	else if ( bSelected && bEnabled)
		pDC->SetColor( m_colorFont[ MLBR_SELECTBAR][ state]);
	else
		pDC->SetColor( m_colorFont[ MLBR_LISTBOX][ state]);

	pDC->SetFont( m_pFont);
	pDC->Text( r, szText, MAM_LEFT | MAM_VCENTER);


	return 1;
}

int MBListBoxLook::OnItemDraw( MListBox* pListBox, MDrawContext* pDC, MRECT& r, MBitmap* pBitmap, bool bSelected, bool bEnabled, bool bFocus, int nAdjustWidth)
{
	// 리스트 박스의 상태를 구한다
	int state;
	if ( bEnabled == false)				state = 2;
	else if ( !pListBox->IsEnable())	state = 2;
	else if ( pListBox->IsFocus())		state = 1;
	else								state = 0;


	pDC->SetBitmap( pBitmap);

	if ( bEnabled == false)
	{
		pDC->SetBitmapColor( 128, 128, 128);
		pDC->Draw(r.x, r.y, r.w, r.h );
		pDC->SetBitmapColor( 255, 255, 255);
	}
	else
		pDC->Draw(r.x, r.y, r.w, r.h );


	return 1;
}


/*	OnFrameDraw
	프레임을 그리는 함수
*/
void MBListBoxLook::OnFrameDraw( MListBox* pListBox, MDrawContext* pDC)
{
	// 실제 클라이언트의 영역을 구합니다
	MRECT rect = pListBox->GetRect();
	rect.x = rect.y = 0;


	// 리스트박스의 상태를 구합니다
	int state;
	if ( !pListBox->IsEnable())			state = 2;
	else if ( pListBox->IsFocus())		state = 1;
	else								state = 0;


	// 프레임을 그립니다
	DrawBitmapByStyle( pDC, rect, MBitmapManager::Get( m_Bitmaps[ MLBR_LISTBOX][ state].strBitmapName.c_str()),
					m_Bitmaps[ MLBR_LISTBOX][ state].m_rCoord, m_BitmapStyle, m_bTiled);
}


/*	SetStretchBlt
	비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBListBoxLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


/*	GetClientRect
	클라이언트의 영역을 구하는 함수(텍스트를 입력하는 공간)
*/
MRECT MBListBoxLook::GetClientRect( MListBox* pListBox, MRECT& r)
{
	if ( m_bCustomLook)
		return MRECT( r.x + 1,  r.y + 1,  r.w - 2,  r.h - 2);


	MRECT rect = r;
	rect.x += m_Bitmaps[ MLBR_LISTBOX][ 0].m_rCoord[ 1].w;
	rect.y += m_Bitmaps[ MLBR_LISTBOX][ 0].m_rCoord[ 1].h;
	rect.w -= m_Bitmaps[ MLBR_LISTBOX][ 0].m_rCoord[ 1].w + m_Bitmaps[ MLBR_LISTBOX][ 0].m_rCoord[ 3].w;
	rect.h -= m_Bitmaps[ MLBR_LISTBOX][ 0].m_rCoord[ 1].h + m_Bitmaps[ MLBR_LISTBOX][ 0].m_rCoord[ 6].h;


	return rect;
}


} // namespace mint3








/************************************************************************
여기서부터는 이전 코드입니다


#include "stdafx.h"
#include "MBListBoxLook.h"
#include "MBitmapDrawer.h"

namespace mint3 {

void MBListBoxLook::OnFrameDraw(MListBox* pListBox, MDrawContext* pDC)
{
	MRECT r = pListBox->GetInitialClientRect();
	//DrawBitmapFrame9(pDC, r, m_pFrameBitmaps);
	pDC->SetColor(19,19,19,255);
	pDC->FillRectangle(r);
	pDC->SetColor(128,128,128,255);
	pDC->Rectangle(r);
}

MBListBoxLook::MBListBoxLook(void)
{
	for(int i=0; i<9; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
}

MRECT MBListBoxLook::GetClientRect(MListBox* pListBox, MRECT& r)
{
	//int lw = GETWIDTH(m_pFrameBitmaps[3]);
	//int rw = GETWIDTH(m_pFrameBitmaps[5]);
	//int uh = GETHEIGHT(m_pFrameBitmaps[7]);
	//int bh = GETHEIGHT(m_pFrameBitmaps[1]);
	//return MRECT(r.x+lw, r.y+uh, r.w-(lw+rw)-((pListBox->GetScrollBar()->IsVisible()==true)?(pListBox->GetScrollBar()->GetClientRect().w):0), r.h-(uh+bh));
	return MRECT(r.x+3, r.y+3, r.w-(6)-((pListBox->GetScrollBar()->IsVisible()==true)?(pListBox->GetScrollBar()->GetClientRect().w):0), r.h-(6));
}

void MBListBoxLook::OnDraw(MListBox* pListBox, MDrawContext* pDC) 
{
	if(m_pFont!=NULL) pDC->SetFont(m_pFont);

	MListBoxLook::OnDraw(pListBox,pDC);
}

} // namespace mint3
*/