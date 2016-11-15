/************************************************************************
MBFrameLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Frame 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#pragma once

#include "Mint.h"
#include "MFrame.h"
#include "MBitmapDrawer.h"


// namespace : mint3
namespace mint3
{


/*	MBFrameLook
	프레임의 Look을 정의하는 클래스
*/
class MBFrameLook : public MFrameLook
{
// Member variables
private:
	// 프레임의 비트맵을 저장하는 변수 선언
	MBitmapRep			m_Bitmaps[ 3];

	// 비트맵 그리기 스타일 변수
	MDRAWBITMAPSTYLE	m_BitmapStyle;

	// 비트맵 이미지를 타일형으로 찍는지 여부를 저장하는 변수
	bool				m_bTiled;

	// 커스텀 Look을 사용하는지 여부를 설정하는 변수
	bool				m_bCustomLook;

	// 폰트를 지정하는 변수
	MFont*				m_pFont;

	// 텍스트 색상을 저장하는 변수
	MCOLOR				m_colorFont[ 3];

	// 타이틀의 그림자를 사용하는지 여부를 설정하는 변수
	bool				m_bTitleShadow;

	// 타이틀 위치 오프셋 값을 저장하는 변수
	MPOINT				m_ptTitleOffset;



// Member functions
public:
	// 생성자
	MBFrameLook();

	// 소멸자
	virtual ~MBFrameLook();



	// 커스텀 Look을 사용하는지 여부를 설정하는 함수
	void SetCustomLook( bool bCustom =true)		{ m_bCustomLook = bCustom;			}

	// 커스텀 Look을 사용하는지 여부를 구하는 함수
	bool IsCustomLook() const						{ return m_bCustomLook;				}



	/* Interface functions */

	// 프레임에 사용될 이미지를 설정하는 함수
	void SetBitmap( const string strFileName);

	// 프레임에 사용될 비트맵의 영역을 설정하는 함수
	void SetAssignRect( int state, int index, float x1, float y1, float x2, float y2);

	// 비트맵을 늘려 찍을 수 있게 설정하는 함수
	void SetStretchBlt( bool bStretch =true);

	// 타일 비트맵을 사용하는지 여부를 설정하는 함수
	void SetTiled( bool bTiled)						{ m_bTiled = bTiled;				}

	// 텍스트 폰트를 설정하는 함수
	void SetFont( MFont* pFont)						{ m_pFont = pFont;					}

	// 텍스트 색상을 설정하는 함수
	void SetFontColor( int state, MCOLOR& color)	{ m_colorFont[ state] = color;		}

	// 타이틀 그림자를 사용하도록 설정하는 함수
	void SetTitleShadow( bool bShadow =true)		{ m_bTitleShadow = bShadow;			}

	// 타이틀 위치 오프셋 값을 설정하는 함수
	void SetTitleOffset( int x, int y)				{ m_ptTitleOffset.x = x;  m_ptTitleOffset.y = y;	}


protected:
	// 프레임을 그리는 함수
	virtual void OnDraw( MFrame* pFrame, MDrawContext* pDC);

	// 프레임의 타이틀바에 텍스트를 그리는 함수
	void DrawText( MFrame* pFrame, MDrawContext* pDC);
};


} // namespace mint3









/**************************************************************

 여기서부터는 예전 코드입니다


#pragma once

#include "MFrame.h"
#include "MScalableLook.h"

namespace mint3 {

#define FRAME_BITMAP_COUNT			9
#define FRAME_BUTTON_BITMAP_COUNT	3

class MBFrameLook : public MFrameLook , public MScalableLook {
public:

	// madduck [4/5/2006]
	MBitmapRep	m_sBitmaps;
	void	SetAssignRect(int index, float x1, float y1, float x2, float y2)
	{
		if( index >= 9) return;

		m_sBitmaps.m_rCoord[index].x = (int)x1;
		m_sBitmaps.m_rCoord[index].y = (int)y1;
		m_sBitmaps.m_rCoord[index].w = (int)x2;
		m_sBitmaps.m_rCoord[index].h = (int)y2;
	}

	char		m_szDefaultTitle[256];
	MBitmap*	m_pFrameBitmaps[FRAME_BITMAP_COUNT];
	MFont*		m_pFont;
	MCOLOR		m_FontColor;
	MCOLOR		m_BGColor;
	MPOINT		m_TitlePosition;
	bool		m_bStretch;
	MBitmap*	m_pCloseButtonBitmaps[FRAME_BUTTON_BITMAP_COUNT];	// 0 - up, 1 - down, 2 - disable
	MBitmap*	m_pMinimizeButtonBitmaps[FRAME_BUTTON_BITMAP_COUNT];
protected:
	virtual void	OnDraw(MFrame* pFrame, MDrawContext* pDC);
	int			m_iCustomLook;

public:
	MBFrameLook(void);
	void SetCustomLook( int i) {
		m_iCustomLook = i;
	}
	int GetCustomLook() const{
		return m_iCustomLook;
	}

	virtual MRECT GetClientRect(MFrame* pFrame, MRECT& r);
};

} // namespace mint3
*/
