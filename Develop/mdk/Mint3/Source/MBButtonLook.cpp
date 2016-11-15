/************************************************************************
MBButtonLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Button 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MBButtonLook.h"


// Define fade factor
const float FADE_FACTOR = 0.0045f;						// 값이 클수록 페이드 시간이 짧아진다



// namespace : mint3
namespace mint3
{

/*	MBButtonLook
	생성자
*/
MBButtonLook::MBButtonLook()
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
	m_colorFont[ MBS_NORMAL]	= MCOLOR( 200, 200, 200);
	m_colorFont[ MBS_MOUSEOVER]	= MCOLOR( 255, 255, 255);
	m_colorFont[ MBS_PRESSED]	= MCOLOR( 255, 255, 255);
	m_colorFont[ MBS_DISABLED]	= MCOLOR( 150, 150, 150);


	// 텍스트 그림자 사용
	m_bTextShadow = false;


	// 눌린 버튼의 텍스트가 이동하는 오프셋 설정
	m_ptTextOffset.x = 2;
	m_ptTextOffset.y = 2;


	// 페이드 롤오버 초기화
	m_dwFadeTimer = timeGetTime();
	m_bFadeRollover = false;
}


/*	~MBButtonLook
	소멸자
*/
MBButtonLook::~MBButtonLook()
{
	// Clear map list
	if ( !m_FadeMap.empty())
		m_FadeMap.clear();
}


/*	SetBitmap
	버튼에 사용될 이미지를 설정하는 함수
*/
void MBButtonLook::SetBitmap( const string strFileName)
{
	for ( int state = 0;  state < MBS_MAX;  state++)
		m_Bitmaps[ state].strBitmapName = strFileName;
}


/*	SetAssignRect
	버튼에 사용될 비트맵의 영역을 설정하는 함수
*/
void MBButtonLook::SetAssignRect( int state, int index, float x1, float y1, float x2, float y2)
{
	// Check valid range
	if ( (state >= MBS_MAX) || (index >= 9))
		return;


	// Set bitmap assignment
	m_Bitmaps[ state].m_rCoord[ index].x = (int)x1;
	m_Bitmaps[ state].m_rCoord[ index].y = (int)y1;
	m_Bitmaps[ state].m_rCoord[ index].w = (int)x2;
	m_Bitmaps[ state].m_rCoord[ index].h = (int)y2;
}


/*	OnUpDraw
	일반 상태의 버튼을 그리는 함수
*/
void MBButtonLook::OnUpDraw( MButton* pButton, MDrawContext* pDC)
{
	// 페이드 롤오버 모드를 사용하지 않으면 해당 버튼을 그리고 바로 리턴함
	if ( !m_bFadeRollover)
	{
		DrawButton( pButton, pDC, MBS_NORMAL);
		DrawText( pButton, pDC, MBS_NORMAL);

		return;
	}


	// 페이드 롤오버 방식을 사용해서 버튼을 그린다


	// 버튼의 페이드 값을 가져온다
	float fFade = GetFadeValue( pButton, MBS_NORMAL);


	// 페이드를 하지 않아도 되는 상황이면 버튼을 그리고 바로 리턴함
	if ( fFade < 0.0001f)
	{
		// 버튼을 그린다
		DrawButton( pButton, pDC, MBS_NORMAL);
		DrawText( pButton, pDC, MBS_NORMAL);

		return;
	}


	// 현재 투명도를 저장한다
	float fOpacity = pDC->GetOpacity();


	// 마우스가 올라온 상태의 버튼을 그린다
	DrawButton( pButton, pDC, MBS_MOUSEOVER);


	// 일반 상태의 버튼을 그린다
	pDC->SetOpacity( fOpacity * (1.0f - fFade));
	DrawButton( pButton, pDC, MBS_NORMAL);


	// 투명도를 이전으로 되돌린다
	pDC->SetOpacity( fOpacity);


	// 텍스트를 그린다
	DrawText( pButton, pDC, MBS_NORMAL);
}


/*	OnOverDraw
	마우스가 올라온 버튼을 그리는 함수
*/
void MBButtonLook::OnOverDraw( MButton* pButton, MDrawContext* pDC)
{
	// 페이드 롤오버 모드를 사용하지 않으면 해당 버튼을 그리고 바로 리턴함
	if ( !m_bFadeRollover)
	{
		// 버튼을 그린다
		DrawButton( pButton, pDC, MBS_MOUSEOVER);
		DrawText( pButton, pDC, MBS_MOUSEOVER);

		return;
	}

	
	// 페이드 롤오버 방식을 사용해서 버튼을 그린다


	// 버튼의 페이드 값을 가져온다
	float fFade = GetFadeValue( pButton, MBS_MOUSEOVER);


	// 페이드를 하지 않아도 되는 상황이면 버튼을 그리고 바로 리턴함
	if ( fFade > 0.999f)
	{
		DrawButton( pButton, pDC, MBS_MOUSEOVER);
		DrawText( pButton, pDC, MBS_MOUSEOVER);

		return;
	}


	// 현재 투명도를 저장한다
	float fOpacity = pDC->GetOpacity();


	// 일반 상태의 버튼을 그린다
	DrawButton( pButton, pDC, MBS_NORMAL);


	// 마우스가 올라온 상태의 버튼을 그린다
	pDC->SetOpacity( fOpacity * fFade);
	DrawButton( pButton, pDC, MBS_MOUSEOVER);


	// 투명도를 이전으로 되돌린다
	pDC->SetOpacity( fOpacity);


	// 텍스트를 그린다
	DrawText( pButton, pDC, MBS_MOUSEOVER);
}


/*	OnDownDraw
	눌린 버튼을 그리는 함수
*/
void MBButtonLook::OnDownDraw( MButton* pButton, MDrawContext* pDC)
{
	// 버튼의 페이드 값을 설정한다
	GetFadeValue( pButton, MBS_PRESSED);


	// 버튼을 그린다
	DrawButton( pButton, pDC, MBS_PRESSED);


	// 텍스트를 그린다
	DrawText( pButton, pDC, MBS_PRESSED);
}


/*	OnDisableDraw
	사용할 수 없는 버튼을 그리는 함수
*/
void MBButtonLook::OnDisableDraw( MButton* pButton, MDrawContext* pDC)
{
	// 버튼의 페이드 값을 설정한다
	GetFadeValue( pButton, MBS_DISABLED);


	// 버튼을 그린다
	DrawButton( pButton, pDC, MBS_DISABLED);


	// 텍스트를 그린다
	DrawText( pButton, pDC, MBS_DISABLED);
}


/*	DrawButton
	실제로 버튼을 그리는 함수
*/
void MBButtonLook::DrawButton( MButton* pButton, MDrawContext* pDC, MBS_STATE state)
{
	// 버튼을 그릴 영역을 구합니다
	MRECT rect = pButton->GetClientRect();


	// 체크 버튼의 경우엔 영역을 수정합니다
	if ( pButton->GetType() == MBT_CHECKBTN)
	{
//		// 체크박스의 크기는 폰트의 높이로 정합니다
//		int height = m_pFont->GetHeight();

		int height = rect.h;


		// 영역을 다시 정의합니다
		rect.x = 0;
		rect.y = (rect.h - height) / 2;
		rect.w = height;
		rect.h = height;
	}


	// 버튼을 그립니다
	DrawBitmapByStyle( pDC, rect, MBitmapManager::Get( m_Bitmaps[ state].strBitmapName.c_str()), m_Bitmaps[ state].m_rCoord, m_BitmapStyle, m_bTiled);
}


/*	DrawText
	버튼의 텍스트를 그리는 함수
*/
void MBButtonLook::DrawText( MButton* pButton, MDrawContext* pDC, MBS_STATE state)
{
	// 텍스트를 출력할 영역을 구합니다
	MRECT rect = pButton->GetClientRect();


	// client edge
#ifdef _DEBUG
	// 텍스트가 "show rect"이면 윈도우 영역을 표시한다
	if ( strcmp( pButton->GetText(), "show rect") == 0)
	{
		pDC->SetColor( 255, 255, 255, (int)(25.0 * sin( (double)timeGetTime() * 0.005) + 25.0));
		pDC->Rectangle( rect);
	}
#endif


	// 폰트 적용
	pDC->SetFont( m_pFont);


	// 텍스트를 그립니다
//	if ( pDC->BeginFont())
	{
		// 콤보 박스 버튼일 경우엔 가로 넓이에서 높이 만큼(드롭 버튼 영역)을 빼줍니다
		if ( pButton->IsComboButton())
			rect.w -= rect.h;



		// 텍스트 정렬값을 구합니다
		MAlignmentMode am = pButton->GetAlignment();

		// 텍스트 색생값을 구합니다
		MCOLOR color = m_colorFont[ state];



		// 버튼 상태가 체크 버튼일 경우엔...
		if ( pButton->GetType() == MBT_CHECKBTN)
		{
//			// 텍스트의 위치를 체크박스 크기만큼 오른쪽으로 이동시킵니다
//			rect.x += m_pFont->GetHeight() + 3;
			rect.x += rect.h + 2;


			// 텍스트 색상 설정
			color = (pButton->IsMouseOver()) ? m_colorFont[ MBS_MOUSEOVER] : m_colorFont[ MBS_NORMAL];


			// 텍스트를 왼쪽 정렬시킵니다
			am = MAM_LEFT;
		}

		// 일반 버튼일 경우에 오프셋을 적용합니다
		else if ( state == MBS_PRESSED)
		{
			rect.x += m_ptTextOffset.x;
			rect.y += m_ptTextOffset.y;
		}


		// 폰트에 외곽선이 있으면 외곽선 두께만큼 빼줍니다
		MFont* pFont = pDC->GetFont();
		if ( pFont)
		{
			if ( pFont->IsOutline())
			{
				rect.x -= 1;
				rect.y -= 1;
			}
		}


		// 그림자 텍스트를 출력합니다
		if ( m_bTextShadow)
		{
			rect.x += 1;
			rect.y += 1;

			pDC->SetColor( MCOLOR( 0, 0, 0, 128));
			pDC->Text( rect, pButton->GetText(), am);

			rect.x -= 1;
			rect.y -= 1;
		}


		// 텍스트를 출력합니다
		pDC->SetColor( color);
		pDC->Text( rect, pButton->GetText(), am);
//		pDC->EndFont();
	}
}


/*	SetStretchBlt
	비트맵을 늘려 찍을 수 있게 설정하는 함수
*/
void MBButtonLook::SetStretchBlt( bool bStretch /*=true*/)
{
	// Set stretch mode
	m_BitmapStyle = bStretch ? DRAWBITMAP_STRETCH : DRAWBITMAP_NORMAL;
}


/*	SetFadeRollover
	페이드 롤오버 효과를 사용하도록 설정하는 함수
*/
void MBButtonLook::SetFadeRollover( bool bFade /*=true*/)
{
	m_bFadeRollover = bFade;


	// 페이드 롤오버를 사용하지 않으면 맵 초기화
	if ( bFade == false)
	{
		if ( !m_FadeMap.empty())
			m_FadeMap.clear();
	}

	// 사용하면 타이머 초기화
	else
		m_dwFadeTimer = timeGetTime();
}


/*	GetFadeValue
	버튼의 페이드 값을 구하는 함수
*/
float MBButtonLook::GetFadeValue( MButton* pButton, MBS_STATE state)
{
	// Find button info
	map<MButton*,float>::iterator find = m_FadeMap.find( pButton);

	// If not exist
	if ( find == m_FadeMap.end())
	{
		// Register a new button node
		m_FadeMap.insert( map<MButton*,float>::value_type( pButton, 1.0f));

		find = m_FadeMap.find( pButton);
	}


	// 페이드 값을 일정 범위로 유지함
	switch ( state)
	{
		// 음수면 양수로 변경함.  1.0 ~ 2.0을 유지...
		case MBS_NORMAL :
			if ( (*find).second < 0.0f)
				(*find).second *= -1.0f;
			break;

		// 양수면 음수로 변경함.  -2.0 ~ -1.0을 유지...
		case MBS_MOUSEOVER :
			if ( (*find).second > 0.0f)
				(*find).second *= -1.0f;
			break;

		case MBS_PRESSED :
			(*find).second = -2.0f;
			break;

		case MBS_DISABLED :
			(*find).second = 1.0f;
			break;

		default:
			break;
	}


	// Set new fade value
	if ( m_bFadeRollover)
	{
		DWORD curr = timeGetTime();
		DWORD diff = curr - m_dwFadeTimer;

		if ( diff > 10)
		{
			// Get face value
			float fFade = (float)diff * FADE_FACTOR;


			// Set fade to all button
			for ( map<MButton*,float>::iterator itr = m_FadeMap.begin();  itr != m_FadeMap.end();  itr++)
			{
				// 양수일 경우엔 Normal상태이므로.... (diff가 너무 커져서 계산 후 음수가
				// 될 수 있으므로 비교 구문을 둘로 나눈다)
				if ( (*itr).second > 0.0f)
				{
					// fade factor
					(*itr).second -= fFade;

					// Limited range
					if ( (*itr).second < 1.0f)
						(*itr).second = 1.0f;
				}

				// 음수일 경우엔 Mouse over 상태이므로....
				else
				{
					// fade factor
					(*itr).second -= fFade;

					// Limited range
					if ( (*itr).second < -2.0f)
						(*itr).second = -2.0f;
				}
			}


			// Set timer
			m_dwFadeTimer = curr;
		}
	}


	// Get fade value
	float fFade = 0.0f;
	switch ( state)
	{
		case MBS_NORMAL :
			fFade = (*find).second - 1.0f;
			break;

		case MBS_MOUSEOVER :
			fFade = -(*find).second - 1.0f;
			break;

		case MBS_PRESSED :
			fFade = -2.0f;
			break;

		case MBS_DISABLED :
			fFade = 1.0f;
			break;

		default:
			break;
	}


	return fFade;
}


} // namespace mint3








/**************************************************************************************************

   여기서부터는 이전 코드입니다.



MBButtonLook::MBButtonLook(void) : MScalableLook()
{
	for(int i=0; i<9; i++){
		m_pUpBitmaps[i] = NULL;
		m_pDownBitmaps[i] = NULL;
		m_pOverBitmaps[i] = NULL;
	}
	for(int i=0; i<4; i++){
		m_pFocusBitmaps[i] = NULL;
	}

	m_pFont = NULL;
	m_FontColor = MColorTable::MBUTTON_TEXT;
	m_FontDownColor = MCOLOR(DEFCOLOR_LIGHT);
	m_FontDisableColor = MCOLOR(MColorTable::DISABLE);
	m_FontHighlightColor = MCOLOR(MColorTable::PEN_HIGHLIGHT);
	m_bStretch = true;
	m_bCustomLook = false;

	m_FontDownOffset = MPOINT(0,0);
}

void MBButtonLook::DrawText(MButton* pButton, MDrawContext* pDC, MRECT& r, const char* szText, MAlignmentMode a)
{
	if(pButton->m_pIcon!=NULL){
		pDC->SetBitmap(pButton->m_pIcon);
		pDC->Draw(r.x, r.y+(r.h-pButton->m_pIcon->GetHeight())/2);

		r.x+=pButton->m_pIcon->GetWidth();
		r.w-=pButton->m_pIcon->GetWidth();
	}

	if(pButton->IsButtonDown())
	{
		r.x+=(int)(m_FontDownOffset.x * GetScale());
		r.y+=(int)(m_FontDownOffset.y * GetScale());
	}


//	if(m_pFont!=NULL)
//		pDC->SetFont(m_pFont);
//
//	if(pButton->IsEnable()==true){
//		if(pButton->IsButtonDown()==true) pDC->SetColor(m_FontDownColor);
//		else if(pButton->IsMouseOver()) pDC->SetColor(m_FontDownColor);
//		else if(pButton->GetType() == MBT_PUSHBTN && pButton->GetCheck()) pDC->SetColor(m_FontDownColor);
//		else pDC->SetColor(m_FontColor);
//	}
//	else pDC->SetColor(m_FontDisableColor);
//	if(pButton->m_bHighlight==true){
//		MCOLOR PrevHCol = pDC->SetHighlightColor(m_FontHighlightColor);
//		pDC->TextWithHighlight(r, szText, a);
//		pDC->SetHighlightColor(PrevHCol);
//	}
//	else{
//		pDC->Text(r, szText, a);
//	}


	// 임시로 이렇게 사용합니다
	if (pDC->BeginFont())
	{
		if(pButton->m_bHighlight==true){
			MCOLOR PrevHCol = pDC->SetHighlightColor(MColorTable::PEN_HIGHLIGHT);
			pDC->TextWithHighlight(r, pButton->m_szText, pButton->GetAlignment());
			pDC->SetHighlightColor(PrevHCol);
		}
		else{
			pDC->Text(r, pButton->m_szText, pButton->GetAlignment());
		}
		pDC->EndFont();
	}
}

void MBButtonLook::DrawFocus(MDrawContext* pDC, MRECT& r)
{
	if(m_pFocusBitmaps[0]==NULL) return;

	pDC->SetBitmap(m_pFocusBitmaps[0]);
	pDC->Draw(r.x-2, r.y-2);
	pDC->SetBitmap(m_pFocusBitmaps[1]);
	pDC->Draw(r.x+r.w-m_pFocusBitmaps[1]->GetWidth()+2, r.y-2);
	pDC->SetBitmap(m_pFocusBitmaps[2]);
	pDC->Draw(r.x-2, r.y+r.h-m_pFocusBitmaps[1]->GetHeight()+2);
	pDC->SetBitmap(m_pFocusBitmaps[3]);
	pDC->Draw(r.x+r.w-m_pFocusBitmaps[1]->GetWidth()+2, r.y+r.h-m_pFocusBitmaps[1]->GetHeight()+2);
}

void MBButtonLook::OnDownDraw(MButton* pButton, MDrawContext* pDC)
{
	MRECT r = pButton->GetInitialClientRect();
	if( GetCustomLook() )
	{
//		DrawBitmapButtonCustom1( pDC, r, m_pDownBitmaps, true, m_bStretch );
//		r = pButton->GetClientRect();
//		r.x += 1;
//		r.y += 1;
//		r.w += 1;
//		r.h += 1;

		// madduck [4/5/2006]
		int stat = CONTROL_STATE_PRESSED;
		MDRAWBITMAPSTYLE eDrawStyle = DRAWBITMAP_NORMAL;
		if(m_bStretch)
			eDrawStyle = DRAWBITMAP_STRETCH;
		DrawBitmapByStyle(pDC, pButton->m_Rect, MBitmapManager::Get(m_StateBitmaps[stat].strBitmapName.c_str()),
			m_StateBitmaps[stat].m_rCoord, eDrawStyle);
	}
	else
	{
		DrawBitmapFrame9(pDC, r, m_pDownBitmaps, m_bStretch, GetScale() );
		r = pButton->GetClientRect();
	}
	if(pButton->IsFocus()==true) DrawFocus(pDC, r);
    
	DrawText(pButton, pDC, r, pButton->m_szText, pButton->GetAlignment());
}

void MBButtonLook::OnUpDraw(MButton* pButton, MDrawContext* pDC)
{
	if( pButton->GetType() == MBT_PUSHBTN  && pButton->GetCheck() ) 
	{
		OnDownDraw( pButton, pDC );
		return;
	}

	MRECT r = pButton->GetInitialClientRect();
	if( GetCustomLook() )
	{
//		DrawBitmapButtonCustom1( pDC, r, m_pUpBitmaps, false, m_bStretch );

		// madduck [4/5/2006]
		int stat = CONTROL_STATE_NORMAL;
		MDRAWBITMAPSTYLE eDrawStyle = DRAWBITMAP_NORMAL;
		if(m_bStretch)
			eDrawStyle = DRAWBITMAP_STRETCH;

		
		DrawBitmapByStyle(pDC, pButton->m_Rect, MBitmapManager::Get(m_StateBitmaps[stat].strBitmapName.c_str()),
			m_StateBitmaps[stat].m_rCoord, eDrawStyle);
	}
	else
		DrawBitmapFrame9(pDC, r, m_pUpBitmaps, m_bStretch, GetScale() );
	if(pButton->IsFocus()==true) DrawFocus(pDC, r);

	DrawText(pButton, pDC, pButton->GetClientRect(), pButton->m_szText, pButton->GetAlignment());
}

void MBButtonLook::OnOverDraw(MButton* pButton, MDrawContext* pDC)
{
	if( pButton->GetType() == MBT_PUSHBTN )
	{
		if( pButton->GetCheck() ) OnDownDraw( pButton, pDC );
		else OnUpDraw(pButton, pDC );
		return;
	}
	
	MRECT r = pButton->GetInitialClientRect();
	if( GetCustomLook() )
	{
//		DrawBitmapButtonCustom1( pDC, r, m_pOverBitmaps, false, m_bStretch );

		// madduck [4/5/2006]
		int stat = CONTROL_STATE_MOUSEOVER;
		MDRAWBITMAPSTYLE eDrawStyle = DRAWBITMAP_NORMAL;
		if(m_bStretch)
			eDrawStyle = DRAWBITMAP_STRETCH;

		DrawBitmapByStyle(pDC, pButton->m_Rect, MBitmapManager::Get(m_StateBitmaps[stat].strBitmapName.c_str()),
			m_StateBitmaps[stat].m_rCoord, eDrawStyle);
	}
	else
		DrawBitmapFrame9(pDC, r, m_pOverBitmaps, m_bStretch, GetScale());
	if(pButton->IsFocus()==true) DrawFocus(pDC, r);

	DrawText(pButton, pDC, pButton->GetClientRect(), pButton->m_szText, pButton->GetAlignment());
}

void MBButtonLook::OnDisableDraw(MButton* pButton, MDrawContext* pDC)
{
	OnUpDraw(pButton, pDC);
}

void MBButtonLook::OnDraw( MButton* pButton, MDrawContext* pDC )
{
	if(GetWireLook())		// 위젯의 오른쪽에 붙는 방식일 경우에...
	{
		MRECT rect = pButton->GetInitialClientRect();

		// 배경 그리고...
		pDC->SetColor(19,19,19,255);
		pDC->FillRectangle( rect);


		// 버튼이 그려질 크기를 구한다.
		MRECT rectButtonBmp;
		rectButtonBmp.x = rect.x + rect.w - rect.h + 1;		// 버튼이 그려질 시작 위치를 구한다.
        rectButtonBmp.y = rect.y + 1;						// 테두리 두께(1 pixel)때문에 1 pixel 아래로 내려서 그린다.
		rectButtonBmp.w = rect.h - 2;						// 위젯의 높이를 기준으로 폭과 높이를 맞춘다.
		rectButtonBmp.h = rect.h - 2;						// 위젯의 높이를 기준으로 폭과 높이를 맞춘다.
		float fScale = (float)(rect.h - 2) / (float)m_pDownBitmaps[1]->GetHeight();

		// 버튼의 비트맵을 그린다
		if( pButton->GetComboDropped() )
		{
			HLineBitmap( pDC, rectButtonBmp.x, rectButtonBmp.y, rectButtonBmp.w, m_pDownBitmaps[2], false, fScale);
		}
		else
		{
			if( pButton->IsFocus() )
				HLineBitmap( pDC, rectButtonBmp.x, rectButtonBmp.y, rectButtonBmp.w, m_pDownBitmaps[0], false, fScale);
			else
				HLineBitmap( pDC, rectButtonBmp.x, rectButtonBmp.y, rectButtonBmp.w, m_pDownBitmaps[1], false, fScale);
		}
		
		MRECT rectText = rect;
		rectText.w -= rect.h;
		pDC->SetColor(205,205,205,255);
	 	pDC->Text(rectText, pButton->m_szText, pButton->GetAlignment() );

		// 테두리 그리고...
		pDC->SetColor(205,205,205,255);
		pDC->Rectangle( rect);

	}
	else
		MButtonLook::OnDraw( pButton, pDC );
}

void MBButtonLook::OnCheckBoxDraw( MButton* pButton, MDrawContext* pDC, bool bPushed )
{
	// 체크박스 그리기
	MRECT r = pButton->GetInitialClientRect();
	int x = r.x + (int)CHECKBOX_SIZE;	// 약간의 여유분
	int y = r.y + (int)(r.h*0.5) -(int)(CHECKBOX_SIZE*0.5);
	pDC->SetColor( 128, 128, 128, 255 );
    pDC->Rectangle( x, y, (int)CHECKBOX_SIZE, (int)CHECKBOX_SIZE );
	//체크버튼 그리기
    if(bPushed)
	{
		pDC->SetBitmap(m_pUpBitmaps[0]);
		pDC->Draw(x-(int)(CHECKBOX_SIZE*0.5), y-(int)(CHECKBOX_SIZE*0.5));
	}
	// 글씨쓰기
	r = pButton->GetClientRect();
//	r.x += 2*CHECKBOX_SIZE;
	r.x += (int)CHECKBOX_SIZE;			// 동환이가 수정
	DrawText( pButton, pDC, r, pButton->m_szText, pButton->GetAlignment());
}

MRECT MBButtonLook::GetClientRect(MButton* pButton, MRECT& r)
{
	float fScale = GetScale();

	int al = (int)(fScale * GETWIDTH(m_pUpBitmaps[3]));
	int au = (int)(fScale * GETHEIGHT(m_pUpBitmaps[7]));
	int ar = (int)(fScale * GETWIDTH(m_pUpBitmaps[5]));
	int ab = (int)(fScale * GETHEIGHT(m_pUpBitmaps[1]));
	return MRECT(r.x+al, r.y+au, r.w-(al+ar), r.h-(au+ab));
}
*/
