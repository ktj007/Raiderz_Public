/************************************************************************
MBButtonLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Button 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#pragma once

#include "Mint.h"
#include "MButton.h"
#include "MBitmapDrawer.h"
#include <map>


// namespace : mint3
namespace mint3
{

/*	MBS_STATE
	버튼의 상태를 정의합니다
*/
enum MBS_STATE
{
	MBS_NORMAL = 0,					// 일반 상태
	MBS_MOUSEOVER,					// 마우스가 올라와 있는 상태
	MBS_PRESSED,						// 눌린 상태
	MBS_DISABLED,					// 사용 불가 상태
	MBS_MAX							// 최대 상태 개수
};



/*	MBButtonLook
	버튼의 Look을 정의하는 클래스
*/
class MBButtonLook : public MButtonLook
{
// Member variables
private:
	// 버튼의 상태에 따른 비트맵을 저장하는 변수 선언
	MBitmapRep			m_Bitmaps[ MBS_MAX];

	// 비트맵 그리기 스타일 변수
	MDRAWBITMAPSTYLE	m_BitmapStyle;

	// 비트맵 이미지를 타일형으로 찍는지 여부를 저장하는 변수
	bool				m_bTiled;

	// 커스텀 Look을 사용하는지 여부를 설정하는 변수
	bool				m_bCustomLook;

	// 폰트를 지정하는 변수
	MFont*				m_pFont;

	// 텍스트 색상을 저장하는 변수
	MCOLOR				m_colorFont[ MBS_MAX];

	// 텍스트의 그림자를 사용하는지 여부를 설정하는 변수
	bool				m_bTextShadow;

	// 눌린 버튼의 텍스트가 이동하는 오프셋 값을 저장하는 변수
	MPOINT				m_ptTextOffset;

	// 페이드 롤오버(마우스를 갖다대면 서서히 변하는 효과)를 사용하는지 여부를 설정하는 변수
	bool				m_bFadeRollover;

	// 페이드 롤오버용 타이머
	DWORD				m_dwFadeTimer;

	// 페이드 값을 저장하는 변수
	map<MButton*,float>		m_FadeMap;



// Member functions
public:
	// 생성자
	MBButtonLook();

	// 소멸자
	virtual ~MBButtonLook();



	// 커스텀 Look을 사용하는지 여부를 설정하는 함수
	void SetCustomLook( bool bCustom =true)		{ m_bCustomLook = bCustom;			}

	// 커스텀 Look을 사용하는지 여부를 구하는 함수
	bool IsCustomLook() const						{ return m_bCustomLook;				}



	/* Interface functions */

	// 버튼에 사용될 이미지를 설정하는 함수
	void SetBitmap( const string strFileName);

	// 버튼에 사용될 비트맵의 영역을 설정하는 함수
	void SetAssignRect( int state, int index, float x1, float y1, float x2, float y2);

	// 일반 상태의 버튼을 그리는 함수
	virtual void OnUpDraw( MButton* pButton, MDrawContext* pDC);

	// 마우스가 올라온 버튼을 그리는 함수
	virtual void OnOverDraw( MButton* pButton, MDrawContext* pDC);

	// 눌린 버튼을 그리는 함수
	virtual void OnDownDraw( MButton* pButton, MDrawContext* pDC);

	// 사용할 수 없는 버튼을 그리는 함수
	virtual void OnDisableDraw( MButton* pButton, MDrawContext* pDC);

	// 비트맵을 늘려 찍을 수 있게 설정하는 함수
	void SetStretchBlt( bool bStretch =true);

	// 타일 비트맵을 사용하는지 여부를 설정하는 함수
	void SetTiled( bool bTiled)					{ m_bTiled = bTiled;				}

	// 텍스트 폰트를 설정하는 함수
	void SetFont( MFont* pFont)						{ m_pFont = pFont;					}

	// 텍스트 색상을 설정하는 함수
	void SetFontColor( int state, MCOLOR& color)	{ m_colorFont[ state] = color;		}

	// 텍스트 그림자를 사용하도록 설정하는 함수
	void SetTextShadow( bool bShadow =true)		{ m_bTextShadow = bShadow;			}

	// 텍스트 위치 오프셋 값을 설정하는 함수
	void SetTextOffset( int x, int y)				{ m_ptTextOffset.x = x;  m_ptTextOffset.y = y;	}

	// 페이드 롤오버 효과를 사용하도록 설정하는 함수
	void SetFadeRollover( bool bFade =true);


protected:
	// 실제로 버튼을 그리는 함수
	void DrawButton( MButton* pButton, MDrawContext* pDC, MBS_STATE state);

	// 버튼의 텍스트를 그리는 함수
	void DrawText( MButton* pButton, MDrawContext* pDC, MBS_STATE state);

	// 버튼의 페이드 값을 구하는 함수
	float GetFadeValue( MButton* pButton, MBS_STATE state);


	WIDGET_UI_LOOK_TYPE( MWIDGET_MBBUTTONLOOK);
};


} // namespace mint3








/**************************************************************************************************

   여기서부터는 이전 코드입니다.



	void SetAssignRect(int stat, int index, float x1, float y1, float x2, float y2)
	{
		if( stat >= BS_MAX) return;
		if( index >= 9) return;

		m_StateBitmaps[stat].m_rCoord[index].x = (int)x1;
		m_StateBitmaps[stat].m_rCoord[index].y = (int)y1;
		m_StateBitmaps[stat].m_rCoord[index].w = (int)x2;
		m_StateBitmaps[stat].m_rCoord[index].h = (int)y2;
	}

	MBitmap*	m_pUpBitmaps[9];
	MBitmap*	m_pDownBitmaps[9];
	MBitmap*	m_pOverBitmaps[9];
	MBitmap*	m_pFocusBitmaps[4];

	MFont*		m_pFont;
	MCOLOR		m_FontColor;
	MCOLOR		m_FontDisableColor;
	MCOLOR		m_FontDownColor;
	MCOLOR		m_FontHighlightColor;
	bool		m_bStretch;
	bool		m_bCustomLook;
	MPOINT		m_FontDownOffset;

	bool		m_bEnableFade;
	float		m_fFade;
	
protected:
	virtual void DrawText(MButton* pButton, MDrawContext* pDC, MRECT& r, const char* szText, MAlignmentMode a);
	virtual void DrawFocus(MDrawContext* pDC, MRECT& r);
	virtual void OnDownDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnUpDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnOverDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnDisableDraw(MButton* pButton, MDrawContext* pDC);
	virtual void OnCheckBoxDraw(MButton* pButton, MDrawContext* pDC, bool bPushed);
	virtual void OnDraw(	MButton* pButton, MDrawContext* pDC );

public:
	MBButtonLook(void);
	virtual ~MBButtonLook() {}

	void SetCustomLook(bool b) {
		m_bCustomLook = b;
	}
	bool GetCustomLook() const{
		return m_bCustomLook;
	}

	virtual MRECT GetClientRect(MButton* pButton, MRECT& r);

	void EnableFade( bool bEnable)		{ m_bEnableFade = bEnable;	}
*/
