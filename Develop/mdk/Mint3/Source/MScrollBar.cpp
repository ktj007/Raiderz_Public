#include "stdafx.h"
#include "MScrollBar.h"
#include "MColorTable.h"


#define MSCROLLBAR_NAME				"Scroll Bar"



namespace mint3
{

IMPLEMENT_LOOK(MScrollBar, MScrollBarLook)



/**	OnDraw
	드로우 함수
*/
void MScrollBarLook::OnDraw( MScrollBar* pScrollBar, MDrawContext* pDC)
{
	// 배경을 그린다
	pDC->SetColor( 80, 80, 80);
	pDC->FillRectangle( pScrollBar->GetClientRect());


	// 양 끝에 화살표 버튼을 그린다
	if ( pScrollBar->IsShowArrow())
	{
		if ( pScrollBar->GetType() == MSBT_VERTICAL)
		{
			// 윗 화살표 버튼을 그린다
			pDC->SetColor( 160, 160, 160);
			pDC->FillRectangle( pScrollBar->GetUpArrowRgn());


			// 아래 화살표 버튼을 그린다
			pDC->SetColor( 160, 160, 160);
			pDC->FillRectangle( pScrollBar->GetDownArrowRgn());
		}
		else
		{
			// 왼쪽 화살표 버튼을 그린다
			pDC->SetColor( 160, 160, 160);
			pDC->FillRectangle( pScrollBar->GetLeftArrowRgn());


			// 오른쪽 화살표 버튼을 그린다
			pDC->SetColor( 160, 160, 160);
			pDC->FillRectangle( pScrollBar->GetRightArrowRgn());
		}
	}


	// Thumb를 그린다
	pDC->SetColor( 255, 255, 255);
	pDC->FillRectangle( pScrollBar->GetThumbRgn());
}


/**	GetClientRect
	클라이언트 영역을 구하는 함수
*/
MRECT MScrollBarLook::GetClientRect( MScrollBar* pScrollBar, MRECT& r)
{
	return r;
}








/**	MScrollBar
	생성자
*/
MScrollBar::MScrollBar( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
		
	m_nType			= MSBT_HORIZONTAL;
	m_bShowArrow	= true;
	m_nValue		= 0;
	m_nRangeMin		= 0;
	m_nRangeMax		= 100;
	m_nWidth		= 15;
	m_nPushedRgn	= MSBR_NONE;
}


/**	MScrollBar
	생성자
*/
MScrollBar::MScrollBar( MWidget* pParent, MListener* pListener, MScrollBarTypes type)
: MWidget( MSCROLLBAR_NAME, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()

	m_nType			= type;
	m_bShowArrow	= true;
	m_nValue		= 0;
	m_nRangeMin		= 0;
	m_nRangeMax		= 100;
	m_nWidth		= 15;
	m_nPushedRgn	= MSBR_NONE;
}


/**	~MScrollBar
	소멸자
*/
MScrollBar::~MScrollBar()
{
}


/**	OnEvent
	OnEvent 메시지 핸들러
*/
bool MScrollBar::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;


	// 부모 위젯이 안보이거나 사용할 수 없으면 동작 안함
	if ( m_pParent && ( !m_pParent->IsEnable() || !m_pParent->IsVisible()) || !IsEnable())
	{
		m_nPushedRgn = MSBR_NONE;
		return false;
	}


	// 메시지 검사
	switch ( pEvent->nMessage)
	{
		case MWM_LBUTTONDOWN:
		{
			MRECT rect = GetInitialClientRect();

			m_nPushedRgn = MSBR_NONE;


			// 화살표 버튼을 보이는지 확인
			if ( m_bShowArrow)
			{
				// Vertical
				if ( m_nType == MSBT_VERTICAL)
				{
					// 위쪽 화살표 영역인지 확인
					if ( GetUpArrowRgn().InPoint( pEvent->Pos) == true)
						m_nPushedRgn = MSBR_UPARROW;

					// 아래쪽 화살표 영역인지 확인
					else if ( GetDownArrowRgn().InPoint( pEvent->Pos) == true)
						m_nPushedRgn = MSBR_DOWNARROW;
				}

				// Horizon
				else
				{
					// 왼쪽 화살표 영역인지 확인
					if ( GetLeftArrowRgn().InPoint( pEvent->Pos) == true)
						m_nPushedRgn = MSBR_LEFTARROW;

					// 오른쪽 화살표 영역인지 확인
					else if ( GetRightArrowRgn().InPoint( pEvent->Pos) == true)
						m_nPushedRgn = MSBR_RIGHTARROW;
				}
			}
			
			
			if ( m_nPushedRgn == MSBR_NONE)
			{
				// Thumb 영역인지 확인
				if ( GetThumbRgn().InPoint( pEvent->Pos) == true)
					m_nPushedRgn = MSBR_THUMB;

				// 배경 영역인지 확인
				else if ( rect.InPoint( pEvent->Pos) == true)
					m_nPushedRgn = MSBR_BKGRND;
			}


			// 눌린 위치 저장
			m_ptCursorPos = pEvent->Pos;


			// 시간 재설정
			m_dwTimer = timeGetTime() - 1000;

			// 메시지가 처리되었다고 리턴
			if(rect.InPoint(pEvent->Pos) )
				return true;

			break;
		}


		case MWM_LBUTTONUP:
			m_nPushedRgn = MSBR_NONE;
			break;


		case MWM_MOUSEMOVE:
			m_ptCursorPos = pEvent->Pos;


		default :
			break;
	}


	return false;
}


/**	OnSize
	OnSize 메시지 핸들러
*/
void MScrollBar::OnSize( int w, int h)
{
}


/**	OnRun
	OnRun 메시지 핸들러
*/
void MScrollBar::OnRun(void)
{
	if ( !IsVisible())
		return;


	// 20ms마다 수행
	DWORD curr = timeGetTime();
	DWORD diff = curr - m_dwTimer;

	if ( diff < 20)
		return;

	m_dwTimer = curr;


	// 리스너를 구함
	MListener* pListener = GetListener();



	// 선택된 영역을 확인함
	switch ( m_nPushedRgn)
	{
		// 선택영역 없음
		case MSBR_NONE :
			break;


		// 윗 화살표 영역
		case MSBR_UPARROW :
		// 왼쪽 화살표 영역
		case MSBR_LEFTARROW :
		{
			// 값 설정
			m_nValue--;
			if ( m_nValue < m_nRangeMin)
			{
				m_nValue = m_nRangeMin;
				break;
			}


			// 메시지 전달
			if ( pListener)
				pListener->OnCommand( this, MEVENT_LIST_VALUE_CHANGED);

			break;
		}


		// 아래 화살표 영역
		case MSBR_DOWNARROW :
		// 오른쪽 화살표 영역
		case MSBR_RIGHTARROW :
		{
			// 값 설정
			m_nValue++;
			if ( m_nValue > m_nRangeMax)
			{
				m_nValue = m_nRangeMax;
				break;
			}


			// 메시지 전달
			if ( pListener)
				pListener->OnCommand( this, MEVENT_LIST_VALUE_CHANGED);

			break;
		}
		

		// Thumb 영역
		case MSBR_THUMB :
		// 배경 영역
		case MSBR_BKGRND :
		{
			// 커서 좌표로부터 값을 설정
			SetValue( m_ptCursorPos);


			// 메시지 전달
			if ( pListener)
				pListener->OnCommand( this, MEVENT_LIST_VALUE_CHANGED);

			break;
		}
	}


	// 부모 리스너가 없으면 리턴
	if ( pListener == NULL)
	{
		MRECT rect = GetClientRect();

	
		if ( m_nType == MSBT_VERTICAL)
			m_nWidth = rect.w;
		else
			m_nWidth = rect.h;

		return;
	}


	// 부모 위젯의 크기에 맞춰서 사이즈를 변경한다. 안좋은 위치...
	if ( m_pParent)
	{
		MRECT r = m_pParent->GetClientRect();
		if ( m_nType == MSBT_VERTICAL)
			SetBounds( MRECT(r.x + r.w - m_nWidth, r.y, m_nWidth, r.h));
		else
			SetBounds( MRECT(r.x, r.y + r.h - m_nWidth, r.w, m_nWidth));
	}
}


/**	SetRange
	스크롤바의 최대/최소 범위를 설정하거나 구하는 함수
*/
void MScrollBar::SetRange( int min, int max)
{
	if ( min > max)
		return;


	m_nRangeMin = min;
	m_nRangeMax = max;


	if ( m_nValue < min)			m_nValue = m_nRangeMin;
	else if ( m_nValue > max)		m_nValue = m_nRangeMax;
}


/**	SetValue
	값을 설정하는 함수
*/
void MScrollBar::SetValue( int nValue)
{
	if ( nValue < m_nRangeMin)			nValue = m_nRangeMin;
	else if ( nValue > m_nRangeMax)		nValue = m_nRangeMax;

	m_nValue = nValue;
}


/**	SetValue
	좌표로부터 값을 설정하는 함수
*/
void MScrollBar::SetValue( MPOINT point)
{
	MRECT r = GetClientRect();


	// Thumb가 이동할 길이를 구한다
	int length = (m_nType == MSBT_VERTICAL) ? r.h : r.w;
	length -= m_nWidth;


	// 화살표 버튼 만큼 길이를 빼준다
	if ( m_bShowArrow)
		length -= m_nWidth + m_nWidth;


	// 값을 구한다
	int nValue;

	if ( m_nType == MSBT_VERTICAL)
		nValue = (int)( (float)((point.y - (m_nWidth / 2) - r.y - (m_bShowArrow ? m_nWidth : 0)) * (m_nRangeMax - m_nRangeMin)) / (float)length + 0.5f) + m_nRangeMin;
	else
		nValue = (int)( (float)((point.x - (m_nWidth / 2) - r.x - (m_bShowArrow ? m_nWidth : 0)) * (m_nRangeMax - m_nRangeMin)) / (float)length + 0.5f) + m_nRangeMin;


	// 값을 설정한다
	SetValue( nValue);
}


/**	GetUpArrowRgn
	위 화살표 버튼의 영역을 구한다
*/
MRECT MScrollBar::GetUpArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.h = m_nWidth;

	return rect;
}


/**	GetDownArrowRgn
	아래 화살표 버튼의 영역을 구한다
*/
MRECT MScrollBar::GetDownArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.y += rect.h - m_nWidth;
	rect.h = m_nWidth;

	return rect;
}


/**	GetLeftArrowRgn
	왼쪽 화살표 버튼의 영역을 구한다
*/
MRECT MScrollBar::GetLeftArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.w = m_nWidth;

	return rect;
}


/**	GetRightArrowRgn
	오른쪽 화살표 버튼의 영역을 구한다
*/
MRECT MScrollBar::GetRightArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.x += rect.w - m_nWidth;
	rect.w = m_nWidth;

	return rect;
}


/**	GetThumbRgn
	Thumb 버튼의 영역을 구한다
*/
MRECT MScrollBar::GetThumbRgn()
{
	MRECT r = GetClientRect();


	// Thumb가 이동할 길이를 구한다
	int length = (m_nType == MSBT_VERTICAL) ? r.h : r.w;
	length -= m_nWidth;


	// 화살표 버튼 만큼 길이를 빼준다
	if ( m_bShowArrow)
		length -= m_nWidth + m_nWidth;


	// 위치 상대값을 구한다
	float fRatio = (float)( m_nValue - m_nRangeMin) / (float)( m_nRangeMax - m_nRangeMin);


	// Thumb의 위치를 구한다
	MRECT rect;

	// 수직형이면...
	if ( m_nType == MSBT_VERTICAL)
	{
		rect.x = r.x;
		rect.y = r.y + (int)( (float)length * fRatio + 0.5f) + ( m_bShowArrow ? m_nWidth : 0);
		rect.w = r.w;
		rect.h = m_nWidth;
	}

	// 수평형이면...
	else			
	{
		rect.x = r.x + (int)( (float)length * fRatio + 0.5f) + ( m_bShowArrow ? m_nWidth : 0);
		rect.y = r.y;
		rect.w = m_nWidth;
		rect.h = r.h;
	}


	return rect;
}


/**	GetBackgroundRgn
	배경 영역을 구한다
*/
MRECT MScrollBar::GetBackgroundRgn()
{
	MRECT rect = GetClientRect();

	
	// 화살표를 안보이면 바로 리턴
	if ( !m_bShowArrow)
		return rect;


	// 수직형이면...
	if ( m_nType == MSBT_VERTICAL)
	{
		rect.y += m_nWidth;
		rect.h -= m_nWidth + m_nWidth;
	}

	// 수평형이면...
	else
	{
		rect.x += m_nWidth;
		rect.w -= m_nWidth + m_nWidth;
	}


	return rect;
}


/*	IsEnable
	사용 가능한 상태인지 여부를 구하는 함수
*/
bool MScrollBar::IsEnable()	
{
	if ( m_pParent && GetListener())
		return m_pParent->IsEnable();


	return MWidget::IsEnable();
}



/*	IsFocus
	포커스를 가진 상태인지 여부를 구하는 함수
*/
bool MScrollBar::IsFocus()
{
	if ( m_pParent)
		return m_pParent->IsFocus();


	return MWidget::IsFocus();
}

void MScrollBar::MoveEndLine()
{
	SetValue(m_nRangeMax);

	MListener* pListener = GetListener();
	if(pListener!=NULL) pListener->OnCommand(this, MEVENT_LIST_VALUE_CHANGED);
}

} // namespace mint3
