/************************************************************************
MBScrollBarLook

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Scrollbar 컨트롤의 커스텀룩(Custom look) 클래스 입니다.
*************************************************************************/


#pragma once

#include "Mint.h"
#include "MScrollBar.h"
#include "MBitmapDrawer.h"


// namespace : mint3
namespace mint3
{


/*	MBScrollBarLook
	스크롤바의 Look을 정의하는 클래스
*/
class MBScrollBarLook : public MScrollBarLook
{
	// Member variables
private:
	// 비트맵을 저장하는 변수 선언
	MBitmapRep			m_Bitmaps[ MSBR_MAX][ 3];	// [type] [normal, mouse down, disabled]

	// 비트맵 그리기 스타일 변수
	MDRAWBITMAPSTYLE	m_BitmapStyle;

	// 비트맵 이미지를 타일형으로 찍는지 여부를 저장하는 변수
	bool				m_bTiled;

	// 커스텀 Look을 사용하는지 여부를 설정하는 변수
	bool				m_bCustomLook;



// Member functions
public:
	// 생성자
	MBScrollBarLook();

	// 소멸자
	virtual ~MBScrollBarLook();



	// 커스텀 Look을 사용하는지 여부를 설정하는 함수
	void SetCustomLook( bool bCustom =true)		{ m_bCustomLook = bCustom;			}

	// 커스텀 Look을 사용하는지 여부를 구하는 함수
	bool IsCustomLook() const						{ return m_bCustomLook;				}



	/* Interface functions */

	// 프레임에 사용될 이미지를 설정하는 함수
	void SetBitmap( const string strFileName);

	// 프레임에 사용될 비트맵의 영역을 설정하는 함수
	void SetAssignRect( int type, int state, int index, float x1, float y1, float x2, float y2);

	// 비트맵을 늘려 찍을 수 있게 설정하는 함수
	void SetStretchBlt( bool bStretch =true);

	// 타일 비트맵을 사용하는지 여부를 설정하는 함수
	void SetTiled( bool bTiled)					{ m_bTiled = bTiled;				}



protected:
	// 스크롤바를 그리는 함수
	virtual void OnDraw( MScrollBar* pScrollBar, MDrawContext* pDC);
};

} // namespace mint3