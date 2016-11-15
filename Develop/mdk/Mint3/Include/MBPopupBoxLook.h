

#pragma once

#include "Mint.h"
#include "MPopupBox.h"
#include "MBitmapDrawer.h"


// namespace : mint3
namespace mint3
{


// class : MBPopupBoxLook
class MBPopupBoxLook : public MPopupBoxLook
{
// Member variables
private:
	// 프레임의 비트맵을 저장하는 변수 선언
	MBitmapRep			m_Bitmaps;

	// 비트맵 그리기 스타일 변수
	MDRAWBITMAPSTYLE	m_BitmapStyle;

	// 비트맵 이미지를 타일형으로 찍는지 여부를 저장하는 변수
	bool				m_bTiled;

	// 커스텀 Look을 사용하는지 여부를 설정하는 변수
	bool				m_bCustomLook;



// Member functions
public:
	MBPopupBoxLook();
	virtual ~MBPopupBoxLook();


	// 커스텀 Look을 사용하는지 여부를 설정하는 함수
	void SetCustomLook( bool bCustom =true)			{ m_bCustomLook = bCustom;			}

	// 커스텀 Look을 사용하는지 여부를 구하는 함수
	bool IsCustomLook() const						{ return m_bCustomLook;				}



	/* Interface functions */

	// 프레임에 사용될 이미지를 설정하는 함수
	void SetBitmap( const string strFileName);

	// 프레임에 사용될 비트맵의 영역을 설정하는 함수
	void SetAssignRect( int index, float x1, float y1, float x2, float y2);

	// 비트맵을 늘려 찍을 수 있게 설정하는 함수
	void SetStretchBlt( bool bStretch =true);

	// 타일 비트맵을 사용하는지 여부를 설정하는 함수
	void SetTiled( bool bTiled)						{ m_bTiled = bTiled;				}


protected:
	// 그리는 함수
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC);
};



} // namespace mint3
















/************************************************************************************
여기서부터 이전코드입니다


#pragma once

#include "MGroup.h"

namespace mint3 {

#define FRAME_BITMAP_COUNT	9
class MBPopupBoxLook : public MGroupLook{
public:
	MBitmap*	m_pFrameBitmaps[FRAME_BITMAP_COUNT];
	MFont*		m_pFont;
	MCOLOR		m_FontColor;
	MPOINT		m_TitlePosition;
	bool		m_bStretch;

protected:
	virtual void OnDraw(MGroup* pGroup, MDrawContext* pDC);

public:
	MBPopupBoxLook(void);

	virtual MRECT GetClientRect(MGroup* pGroup, MRECT& r);
};

} // namespace mint3
*/
