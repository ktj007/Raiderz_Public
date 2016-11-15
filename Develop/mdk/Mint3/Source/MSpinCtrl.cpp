#include "stdafx.h"
#include "MSpinCtrl.h"
#include "MColorTable.h"


#define MSPINCTRL_NAME				"Spin Ctrl"



namespace mint3
{

IMPLEMENT_LOOK(MSpinCtrl, MSpinCtrlLook)



/**	OnDraw
	드로우 함수
*/
void MSpinCtrlLook::OnDraw( MSpinCtrl* pSpinCtrl, MDrawContext* pDC)
{
	// 배경을 그린다
	pDC->SetColor( 80, 80, 80);
	pDC->FillRectangle( pSpinCtrl->GetClientRect());


	// 양 끝에 화살표 버튼을 그린다
	if ( pSpinCtrl->GetType() == MSPT_VERTICAL)
	{
		// 윗 화살표 버튼을 그린다
		pDC->SetColor( 160, 160, 160);
		pDC->FillRectangle( pSpinCtrl->GetUpArrowRgn());


		// 아래 화살표 버튼을 그린다
		pDC->SetColor( 160, 160, 160);
		pDC->FillRectangle( pSpinCtrl->GetDownArrowRgn());
	}
	else
	{
		// 왼쪽 화살표 버튼을 그린다
		pDC->SetColor( 160, 160, 160);
		pDC->FillRectangle( pSpinCtrl->GetLeftArrowRgn());


		// 오른쪽 화살표 버튼을 그린다
		pDC->SetColor( 160, 160, 160);
		pDC->FillRectangle( pSpinCtrl->GetRightArrowRgn());
	}
}


/**	GetClientRect
	클라이언트 영역을 구하는 함수
*/
MRECT MSpinCtrlLook::GetClientRect( MSpinCtrl* pSpinCtrl, MRECT& r)
{
	return r;
}








/**	MSpinCtrl
	생성자
*/
MSpinCtrl::MSpinCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
		
	m_nType			= MSPT_VERTICAL;
	m_nPushedRgn	= MSPR_NONE;
	m_pBuddy		= NULL;
}


/**	~MSpinCtrl
	소멸자
*/
MSpinCtrl::~MSpinCtrl()
{
}


/**	OnEvent
	OnEvent 메시지 핸들러
*/
bool MSpinCtrl::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;


	// 부모 위젯이 안보이거나 사용할 수 없으면 동작 안함
	if ( m_pParent && ( !m_pParent->IsEnable() || !m_pParent->IsVisible()) || !IsEnable())
	{
		m_nPushedRgn = MSPR_NONE;
		return false;
	}


	// 메시지 검사
	switch ( pEvent->nMessage)
	{
		case MWM_LBUTTONDOWN:
		{
			MRECT rect = GetInitialClientRect();

			m_nPushedRgn = MSPR_NONE;


			// Vertical
			if ( m_nType == MSPT_VERTICAL)
			{
				// 위쪽 화살표 영역인지 확인
				if ( GetUpArrowRgn().InPoint( pEvent->Pos) == true)
					m_nPushedRgn = MSPR_UPARROW;

				// 아래쪽 화살표 영역인지 확인
				else if ( GetDownArrowRgn().InPoint( pEvent->Pos) == true)
					m_nPushedRgn = MSPR_DOWNARROW;
			}

			// Horizon
			else
			{
				// 왼쪽 화살표 영역인지 확인
				if ( GetLeftArrowRgn().InPoint( pEvent->Pos) == true)
					m_nPushedRgn = MSPR_LEFTARROW;


				// 오른쪽 화살표 영역인지 확인
				else if ( GetRightArrowRgn().InPoint( pEvent->Pos) == true)
					m_nPushedRgn = MSPR_RIGHTARROW;
			}



			// 값 증가
			if ( (m_nPushedRgn == MSPR_UPARROW) || (m_nPushedRgn == MSPR_RIGHTARROW))
			{
				if ( m_pBuddy)
				{
					int num = atoi( m_pBuddy->GetText());
					num++;

					char szTmp[ 256];
					_itoa( num, szTmp, 10);
					m_pBuddy->SetText( szTmp);
				}
			}

			// 값 감소
			else if ( (m_nPushedRgn == MSPR_DOWNARROW) || (m_nPushedRgn == MSPR_LEFTARROW))
			{
				if ( m_pBuddy)
				{
					int num = atoi( m_pBuddy->GetText());
					num--;

					if ( num < 0)
						num = 0;

					char szTmp[ 256];
					_itoa( num, szTmp, 10);
					m_pBuddy->SetText( szTmp);
				}
			}
			
			
			// 눌린 위치 저장
			m_ptCursorPos = pEvent->Pos;


			// 시간 재설정
			m_dwTimer = timeGetTime();


			break;
		}


		case MWM_LBUTTONUP:
			m_nPushedRgn = MSPR_NONE;
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
void MSpinCtrl::OnSize( int w, int h)
{
}


/**	OnRun
	OnRun 메시지 핸들러
*/
void MSpinCtrl::OnRun(void)
{
	// 250ms마다 수행
	DWORD curr = timeGetTime();
	DWORD diff = curr - m_dwTimer;

	if ( diff < 250)
		return;

	m_dwTimer = curr;


	// 리스너를 구함
	MListener* pListener = GetListener();



	// 선택된 영역을 확인함
	switch ( m_nPushedRgn)
	{
		// 선택영역 없음
		case MSPR_NONE :
			break;


		// 윗 화살표 영역
		case MSPR_UPARROW :
		// 왼쪽 화살표 영역
		case MSPR_RIGHTARROW:
		{
			// 메시지 전달
			if ( pListener)
				pListener->OnCommand( this, MEVENT_LIST_VALUE_CHANGED);


			// 값 증가
			if ( m_pBuddy)
			{
				int num = atoi( m_pBuddy->GetText());
				num++;

				char szTmp[ 256];
				_itoa( num, szTmp, 10);
				m_pBuddy->SetText( szTmp);
			}


			break;
		}


		// 아래 화살표 영역
		case MSPR_DOWNARROW :
		// 오른쪽 화살표 영역
		case MSPR_LEFTARROW:
		{
			// 메시지 전달
			if ( pListener)
				pListener->OnCommand( this, MEVENT_LIST_VALUE_CHANGED);


			// 값 감소
			if ( m_pBuddy)
			{
				int num = atoi( m_pBuddy->GetText());
				num--;

				if ( num < 0)
					num = 0;

				char szTmp[ 256];
				_itoa( num, szTmp, 10);
				m_pBuddy->SetText( szTmp);
			}


			break;
		}
	}


	// 부모 위젯의 크기에 맞춰서 사이즈를 변경한다. 안좋은 위치...
	if ( m_pBuddy)
	{
		MRECT r = m_pBuddy->GetRect();

		int width = GetClientRect().w;
		SetBounds( MRECT( r.x + r.w, r.y, width, r.h));
	}
}


/**	GetUpArrowRgn
	위 화살표 버튼의 영역을 구한다
*/
MRECT MSpinCtrl::GetUpArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.h /= 2;

	return rect;
}


/**	GetDownArrowRgn
	아래 화살표 버튼의 영역을 구한다
*/
MRECT MSpinCtrl::GetDownArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.y += rect.h / 2;
	rect.h = rect.h - (rect.h / 2);

	return rect;
}


/**	GetLeftArrowRgn
	왼쪽 화살표 버튼의 영역을 구한다
*/
MRECT MSpinCtrl::GetLeftArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.w /= 2;

	return rect;
}


/**	GetRightArrowRgn
	오른쪽 화살표 버튼의 영역을 구한다
*/
MRECT MSpinCtrl::GetRightArrowRgn()
{
	MRECT rect = GetClientRect();
	rect.x += rect.w / 2;
	rect.w = rect.w - (rect.w / 2);

	return rect;
}


/*	IsEnable
	사용 가능한 상태인지 여부를 구하는 함수
*/
bool MSpinCtrl::IsEnable()	
{
	if ( m_pParent && GetListener())
		return m_pParent->IsEnable();


	return MWidget::IsEnable();
}



/*	IsFocus
	포커스를 가진 상태인지 여부를 구하는 함수
*/
bool MSpinCtrl::IsFocus()
{
	if ( m_pParent && GetListener())
		return m_pParent->IsFocus();


	return MWidget::IsFocus();
}


} // namespace mint3
