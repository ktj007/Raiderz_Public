#pragma once

#include "MWidget.h"
#include "MLookNFeel.h"


// namespace : mint3
namespace mint3
{


// 클래스 선언
class MListBox;



// ScrollBar 타입
enum MScrollBarTypes
{
	MSBT_VERTICAL		= 0,		// 우측 세로면에 부착
	MSBT_HORIZONTAL		= 1			// 하단 가로면에 부착
};



// 스크롤바 영역
enum MSCROLLBAR_REGION
{
	MSBR_NONE			= -1,		// 없음
	MSBR_UPARROW		= 0,		// 윗 화살표 영역
	MSBR_DOWNARROW,					// 아래 화살표 영역
	MSBR_LEFTARROW,					// 왼쪽 화살표 영역
	MSBR_RIGHTARROW,				// 오른쪽 화살표 영역
	MSBR_THUMB,						// Thumb 영역
	MSBR_BKGRND,					// 배경 영역
	MSBR_MAX
};



class MScrollBar;


// 스크롤바 커스텀 Look
class MScrollBarLook
{
public:
	virtual void OnDraw( MScrollBar* pScrollBar, MDrawContext* pDC);
	virtual MRECT GetClientRect( MScrollBar* pScrollBar, MRECT& r);
};



// 스크롤바 클래스 선언
class MScrollBar : public MWidget
{
// friend 선언
friend MListBox;


// Member variables
protected:
	// 스크롤바 타입
	MScrollBarTypes		m_nType;

	// 화살표 버튼을 보일지 여부
	bool				m_bShowArrow;

	// 스크롤바의 현재 설정된 값
	int					m_nValue;

	// 스크롤바 최대/최소 범위
	int					m_nRangeMin;
	int					m_nRangeMax;

	// 스크롤바 두께
	int					m_nWidth;

	// 마우스가 눌린 위치
	MSCROLLBAR_REGION	m_nPushedRgn;

	// 커서 좌표
	MPOINT				m_ptCursorPos;

	// 타이머
	DWORD				m_dwTimer;



// Member functions
public:
	// 생성자
	MScrollBar( const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
	MScrollBar( MWidget* pParent=NULL, MListener* pListener=NULL, MScrollBarTypes type=MSBT_VERTICAL);

	// 소멸자
	virtual ~MScrollBar();


	// 타입을 설정하거나 구하는 함수
	void SetType( MScrollBarTypes type)			{ m_nType = type;							}
	MScrollBarTypes GetType()					{ return m_nType;							}


	// 화살표 버튼을 보일지 여부를 구하는 함수
	bool IsShowArrow()							{ return m_bShowArrow;						}


	// 스크롤바의 최대/최소 범위를 설정하거나 구하는 함수
	void SetRange( int min, int max);
	int GetMin()								{ return m_nRangeMin;						}
	int GetMax()								{ return m_nRangeMax;						}


	// 값을 설정하거나 구하는 함수
	void SetValue( int nValue);
	void SetValue( MPOINT point);
	int GetValue()								{ return m_nValue;							}


	// 스크롤바 두께를 설정하거나 구하는 함수
	void SetWidth( int nWidth)					{ m_nWidth = nWidth;						}
	int GetWidth()								{ return m_nWidth;							}


	// 마우스가 눌린 영역을 구하는 함수
	MSCROLLBAR_REGION GetPushedRgn()			{ return m_nPushedRgn;	}


	// 위 화살표 버튼의 영역을 구한다
	MRECT GetUpArrowRgn();

	// 아래 화살표 버튼의 영역을 구한다
	MRECT GetDownArrowRgn();

	// 왼쪽 화살표 버튼의 영역을 구한다
	MRECT GetLeftArrowRgn();

	// 오른쪽 화살표 버튼의 영역을 구한다
	MRECT GetRightArrowRgn();

	// Thumb 버튼의 영역을 구한다
	MRECT GetThumbRgn();

	// 배경 영역을 구한다
	MRECT GetBackgroundRgn();


	// 사용 가능한 상태인지 여부를 구하는 함수
	bool IsEnable();	


	// 포커스를 가진 상태인지 여부를 구하는 함수
	bool IsFocus();


	// Look을 설정한다
	DECLARE_LOOK( MScrollBarLook)
	DECLARE_LOOK_CLIENT()

	// Glue 함수
	int glueGetValue() { return GetValue(); }

// 메시지 핸들러
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual void OnRun( void);
	virtual void OnSize( int w, int h);

public:
	// 임시
	int GetDefaultBreadth(void) { return 0; }
	void MoveStartLine() {}
	void MoveEndLine();

#define MINT_SCROLLBAR	"ScrollBar"
	virtual const char* GetClassName(void)		{ return MINT_SCROLLBAR; }
};



} // namespace mint3