/************************************************************************
MBEditLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Editbox 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBEditLook.h"


// namespace : mint3
namespace mint3
{

/*	MBEditLook
	생성자
*/
MBEditLook::MBEditLook()
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
	m_colorFont[ 2]	= MCOLOR( 150, 150, 150);		// Disabled
}


/*	~MBEditLook
	소멸자
*/
MBEditLook::~MBEditLook()
{
}


/*	SetBitmap
	프레임에 사용될 이미지를 설정하는 함수
*/
void MBEditLook::SetBitmap( const string strFileName)
{
	m_Bitmaps[ 0].strBitmapName = strFileName;
	m_Bitmaps[ 1].strBitmapName = strFileName;
	m_Bitmaps[ 2].strBitmapName = strFileName;
}


/*	SetAssignRect
	프레임에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBEditLook::SetAssignRect( int state, int index, float x1, float y1, float x2, float y2)
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
void MBEditLook::OnFrameDraw(MEdit* pEdit, MDrawContext* pDC)
{
	// 실제 클라이언트의 영역을 구합니다
	MRECT rect = pEdit->GetRect();
	rect.x = rect.y = 0;


	// 에디트박스의 상태를 구합니다
	int state;
	if ( !pEdit->IsEnable())		state = 2;
	else if ( pEdit->IsFocus())		state = 1;
	else							state = 0;


	// 프레임을 그립니다
	DrawBitmapByStyle( pDC, rect, MBitmapManager::Get( m_Bitmaps[ state].strBitmapName.c_str()), m_Bitmaps[ state].m_rCoord, m_BitmapStyle, m_bTiled);
}


/*	OnTextDraw
	텍스트를 그리는 함수
*/
void MBEditLook::OnTextDraw( MEdit* pEdit, MDrawContext* pDC, bool bShowLanguageTab)
{
#define BUFFERSIZE	1024

	char szBuffer[BUFFERSIZE];
	_ASSERT(sizeof(szBuffer)>pEdit->GetMaxLength()+2);
	if(pEdit->GetMaxLength()+2>BUFFERSIZE) return;

	if(pEdit->IsPasswordField()==false){
		wsprintf(szBuffer, "%s", pEdit->GetText());
	}
	else{
		memset(szBuffer, '*', strlen(pEdit->GetText()));
		szBuffer[strlen(pEdit->GetText())] = '\0';
	}

	// IME Composition String
	if(pEdit->IsFocus()==true && pEdit->GetCompositionString()[0]!=NULL){
		InsertString(szBuffer, pEdit->GetCompositionString(), pEdit->GetCarretPos());
	}

	// 텍스트 색상을 설정
	int state;
	if ( !pEdit->IsEnable())		state = 2;
	else if ( pEdit->IsFocus())		state = 1;
	else							state = 0;
	pDC->SetColor( m_colorFont[ state]);

	
	// 폰트 설정
	pDC->SetFont( m_pFont);


	MRECT r = pEdit->GetClientRect();
	r.x-=2;

	if( bShowLanguageTab )
		r.w-=m_pFont->GetHeight();
	//	r.w-=10;

	if(pEdit->GetCarretPos()<pEdit->GetStartPos())
		pEdit->SetStartPos(pEdit->GetCarretPos());

	_ASSERT(pEdit->GetCarretPos()>=pEdit->GetStartPos());

	int nCompositionStringLength = strlen(pEdit->GetCompositionString());


	// 텍스트의 시작 문자열 위치 지정
	char* szTemp = NULL;
	szTemp = szBuffer+pEdit->GetStartPos();
	// pEdit->GetCompositionString()의 길이도 포함해야 StartPos 세팅에 컴포지션을 고려된다.
	int nTestLen = pEdit->GetCarretPos()-pEdit->GetStartPos()+nCompositionStringLength;

	// Composition 문자열이 Edit 너비를 넘어설때는 StartPos이 Composition 문자열 안으로 들어와야 한다.
	// 이런 경우 ESC 등으로 StartPos이 Composition 문자열을 제외한 원래 문자열로 돌아오는 문제점등이 생기므로,
	// StartPos의 범위는 Composition 문자열을 제외한 문자열로 제한한다.
#define INDICATOR_WIDTH	10
	while(szTemp[0]!=NULL && r.w-INDICATOR_WIDTH < m_pFont->GetWidth(szTemp, nTestLen))
	{	// -INDICATOR_WIDTH는 IME Indicator 위치 보정
		int nStartPos = NextPos(pEdit->GetText(), pEdit->GetStartPos());
		if(pEdit->SetStartPos(nStartPos)==false) break;
		szTemp = szBuffer+pEdit->GetStartPos();
		nTestLen = pEdit->GetCarretPos()-pEdit->GetStartPos()+nCompositionStringLength;
	}


	char* szStartText = szBuffer+pEdit->GetStartPos();	// 현재 창에 표시되는 문자열의 시작

	if(pEdit->IsFocus()==true)
	{
		int nFontHeight = m_pFont->GetHeight();
		Mint* pMint = Mint::GetInstance();

		int nInsertPosInWidget = m_pFont->GetWidth(szStartText, pEdit->GetCarretPos()-pEdit->GetStartPos());
		int nCaretPosInWidget = m_pFont->GetWidth(szStartText, pEdit->GetCarretPos()+pMint->m_nCompositionCaretPosition-pEdit->GetStartPos());

		// 출력될 Candidate List 위치 지정하기
		MRECT r = pEdit->GetClientRect();
		//int nTextWidth = pFont->GetWidth(pEdit->GetText());
		MPOINT cp = MClientToScreen(pEdit, MPOINT(r.x+nInsertPosInWidget, r.y));
		pMint->SetCandidateListPosition(cp, r.h);


		// Caret
		int nSelStartPos=0;
		int nSelEndPos=0;
		// Caret
		long nCurrTime = timeGetTime();
		if((nCurrTime%(MEDIT_BLINK_TIME*2))>MEDIT_BLINK_TIME){
			//pDC->Text(r.x+nWidth-1, r.y, "|");
			//				r.x+=nCaretPosInWidget-(int)((float)pFont->GetHeight()*0.3f);
			r.x+=nCaretPosInWidget;//-(int)((float)pFont->GetHeight());

//			if (pDC->BeginFont())
			{
				pDC->Text(r, "|", MAM_LEFT);
//				pDC->EndFont();
			}
		}


		r = pEdit->GetClientRect();

		// Composition 언더바 출력
		MPOINT p;
		pDC->GetPositionOfAlignment(&p, r, szStartText, MAM_LEFT);
		p.x += nInsertPosInWidget;
		pMint->DrawCompositionAttributes(pDC, p, pEdit->GetCompositionString());
	}


	pDC->Text(r, szStartText, MAM_LEFT);


	// 인디케이터 표기, 지전분하게 텍스트와 겹쳐서 보이지 않게 앞쪽에 그려준다.
	if(pEdit->IsFocus()==true && 
		pEdit->GetVisibleIndicator() == true)	// 인디케이터 표시 하는가?
	{
		if(bShowLanguageTab){
			Mint* pMint = Mint::GetInstance();
			pMint->DrawIndicator(pDC, pEdit->GetClientRect());
		}
	}
}


/*	SetStretchBlt
	비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBEditLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


/*	GetClientRect
	클라이언트의 영역을 구하는 함수(텍스트를 입력하는 공간)
*/
MRECT MBEditLook::GetClientRect(MEdit* pEdit, MRECT& r)
{
	if ( m_bCustomLook )
		return MRECT( r.x + 1,  r.y + 1,  r.w - 2,  r.h - 2);


	MRECT rect = r;
	rect.x += m_Bitmaps[ 0].m_rCoord[ 1].w;
	rect.y += m_Bitmaps[ 0].m_rCoord[ 1].h;
	rect.w -= m_Bitmaps[ 0].m_rCoord[ 1].w + m_Bitmaps[ 0].m_rCoord[ 3].w;
	rect.h -= m_Bitmaps[ 0].m_rCoord[ 1].h + m_Bitmaps[ 0].m_rCoord[ 6].h;


	return rect;
}


} // namespace mint3








/*****************************************************************

여기서부터는 이전 코드입니다


#include "stdafx.h"
#include "MBEditLook.h"
#include "MBitmapDrawer.h"

namespace mint3 {

void MBEditLook::OnFrameDraw(MEdit* pEdit, MDrawContext* pDC)
{
	MRECT r = pEdit->GetInitialClientRect();
 	if(GetCustomLook())
	{
		pDC->SetColor(MCOLOR(200,200,200,255));
		pDC->Rectangle(r);
		HLineBitmap( pDC, r.x+1, r.y+1, r.w-2, m_pFrameBitmaps[4], false );
		return;
	}
	DrawBitmapFrame9(pDC, r, m_pFrameBitmaps);
}

MBEditLook::MBEditLook(void)
{
	for(int i=0; i<9; i++){
		m_pFrameBitmaps[i] = NULL;
	}
	m_pFont=NULL;
}

MRECT MBEditLook::GetClientRect(MEdit* pEdit, MRECT& r)
{
	if( m_bCustomLook )
	{
		return MRECT(r.x+1, r.y+1, r.w-2, r.h-2);
	}
	int al = GETWIDTH(m_pFrameBitmaps[3]);
	int au = GETHEIGHT(m_pFrameBitmaps[7]);
	int ar = GETWIDTH(m_pFrameBitmaps[5]);
	int ab = GETHEIGHT(m_pFrameBitmaps[1]);
	return MRECT(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
}

void MBEditLook::OnDraw(MEdit* pEdit, MDrawContext* pDC) 
{
	if(m_pFont!=NULL) pDC->SetFont(m_pFont);

    MEditLook::OnDraw(pEdit,pDC);
}

} // namespace mint3
*/