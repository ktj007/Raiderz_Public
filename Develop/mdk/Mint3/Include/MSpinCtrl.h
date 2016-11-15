#pragma once

#include "MWidget.h"
#include "MLookNFeel.h"


// namespace : mint3
namespace mint3
{


// 클래스 선언
class MListBox;



// SpinCtrl 타입
enum MSpinCtrlTypes
{
	MSPT_VERTICAL		= 0,		// 세로형
	MSPT_HORIZONTAL,				// 가로형
};



// 스핀컨트롤 영역
enum MSPINCTRL_REGION
{
	MSPR_NONE			= -1,
	MSPR_UPARROW		= 0,		// 윗 화살표 영역
	MSPR_DOWNARROW,					// 아래 화살표 영역
	MSPR_LEFTARROW,					// 왼쪽 화살표 영역
	MSPR_RIGHTARROW,				// 오른쪽 화살표 영역
	MSPR_MAX
};



class MSpinCtrl;


// 스핀컨트롤 커스텀 Look
class MSpinCtrlLook
{
public:
	virtual void OnDraw( MSpinCtrl* pSpinCtrl, MDrawContext* pDC);
	virtual MRECT GetClientRect( MSpinCtrl* pSpinCtrl, MRECT& r);
};



// 스핀컨트롤 클래스 선언
class MSpinCtrl : public MWidget
{
// friend 선언
friend MListBox;


// Member variables
protected:
	// 스핀컨트롤 타입
	MSpinCtrlTypes		m_nType;

	// 버디 설정
	MWidget*			m_pBuddy;

	// 마우스가 눌린 위치
	MSPINCTRL_REGION	m_nPushedRgn;

	// 커서 좌표
	MPOINT				m_ptCursorPos;

	// 타이머
	DWORD				m_dwTimer;



// Member functions
public:
	// 생성자
	MSpinCtrl( const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);

	// 소멸자
	virtual ~MSpinCtrl();


	// 타입을 설정하거나 구하는 함수
	void SetType( MSpinCtrlTypes type)			{ m_nType = type;		}
	MSpinCtrlTypes GetType()					{ return m_nType;		}


	// 버디를 설정하는 함수
	void SetBuddy( MWidget* pBuddy)				{ m_pBuddy = pBuddy;	}

	// 마우스가 눌린 영역을 구하는 함수
	MSPINCTRL_REGION GetPushedRgn()				{ return m_nPushedRgn;	}


	// 위 화살표 버튼의 영역을 구한다
	MRECT GetUpArrowRgn();

	// 아래 화살표 버튼의 영역을 구한다
	MRECT GetDownArrowRgn();

	// 왼쪽 화살표 버튼의 영역을 구한다
	MRECT GetLeftArrowRgn();

	// 오른쪽 화살표 버튼의 영역을 구한다
	MRECT GetRightArrowRgn();


	// 사용 가능한 상태인지 여부를 구하는 함수
	bool IsEnable();	


	// 포커스를 가진 상태인지 여부를 구하는 함수
	bool IsFocus();


	// Look을 설정한다
	DECLARE_LOOK( MSpinCtrlLook)
	DECLARE_LOOK_CLIENT()



// 메시지 핸들러
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual void OnRun( void);
	virtual void OnSize( int w, int h);


#define MINT_SPINCTRL	"SpinCtrl"
	virtual const char* GetClassName(void)		{ return MINT_SPINCTRL; }
};



} // namespace mint3