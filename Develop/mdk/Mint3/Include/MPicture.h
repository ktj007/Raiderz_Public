/************************************************************************
MPicture

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Picture 위젯 클래스 입니다.
*************************************************************************/

#pragma once

#include "MWidget.h"


// namespace : mint3
namespace mint3
{

/*	MPicture
	Picture 위젯 클래스
*/
class MPicture : public MWidget
{
// Member variables
protected:
	MBitmap*	m_pBitmap;			// 비트맵 이미지 포인터

	bool		m_bStretch;			// 비트맵을 늘려 찍는지 설정하는 함수

	DWORD		m_dwRotate;			// 비트맵 회전 상태(좌우 반전, 상하반전, 90도 회전, 90도 역회전)

	MCOLOR		m_Colorize;			// 비트맵 이미지의 colorize 색상

	bool		m_bTiled;			// 비트맵을 타일 형식으로 찍는지 설정하는 함수




// Member functions
public:
	// 생성자
	MPicture( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);


	// 비트맵 리소스를 설정하는 함수
	void SetBitmap( MBitmap* pBitmap);

	// 비트맵 리소스를 구하는 함수
	MBitmap* GetBitmap();


	// 비트맵을 늘려 찍는지 설정하는 함수
	void SetStretch( bool bStretch);


	// 비트맵을 좌우로 뒤집는 함수
	void SetFlipHorizontal();

	// 비트맵을 상하로 뒤집는 함수
	void SetFlipVertical();

	// 비트맵을 시계 방향으로 90도 회전시키는 함수
	void SetRotate90CW();

	// 비트맵을 반시계 방향으로 90도 회전시키는 함수
	void SetRotate90CCW();

	// 비트맵을 반시계 방향으로 180도 회전시키는 함수
	void SetRotate180();

	// 비트맵 회전을 리셋하는 함수
	void ResetRotate();

	// 비트맵을 타일 형식으로 찍는지 설정하는 함수
	void SetTiled( bool bTiled)			{ m_bTiled = bTiled;	}


	// 비트맵 이미지의 colorize를 설정하는 함수
	void SetColorize( MCOLOR color);	


// Event handler
	void glueSetBitmapName( const char* szBitmapName );

protected:
	// OnDraw 이벤트 핸들러
	virtual void OnDraw( MDrawContext* pDC);

	// OnEvent 이벤트 핸들러
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);


#define MINT_PICTURE	"Picture"
	virtual const char* GetClassName(void)		{ return MINT_PICTURE; }
};


} // namespace mint3














/*****************************************************************************
여기서부터 이전 코드입니다


#pragma once

#include "MWidget.h"

namespace mint3 {

/// Picture
class MPicture : public MWidget{
protected:
	MBitmap* m_pBitmap;
	int		m_iStretch;
	int		m_iAnimType;			// 0:좌->우, 1:우->좌
	float	m_fAnimTime;			// 애니메이션 되는 시간
	DWORD	m_dwCurrentTime;
	bool	m_bAnim;
	DWORD	m_DrawMode;

	MCOLOR	m_BitmapColor;
	MCOLOR	m_BitmapReseveColor;
	bool	m_bSwaped;

protected:
	/// Draw Handler
	virtual void OnDraw(MDrawContext* pDC);
	void OnAnimDraw(MDrawContext* pDC, int x, int y, int w, int h, int bx, int by, int bw, int bh );

public:
	MPicture(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	/// 비트맵 지정
	void SetBitmap(MBitmap* pBitmap);
	/// 비트맵 얻기
	MBitmap* GetBitmap(void);
	int		GetStretch() { return m_iStretch; }
	void SetStretch(int i) { m_iStretch = i; }

	void SetAnimation( int animType, float fAnimTime);

	void SetBitmapColor( MCOLOR color );	
	MCOLOR GetBitmapColor() const {
		return m_BitmapColor;
	}
	MCOLOR GetReservedBitmapColor() const{
		return m_BitmapReseveColor;	
	}

	void SetDrawMode( DWORD mode ) {
		m_DrawMode = mode;
	}
	DWORD GetDrawMode( ) const {
		return m_DrawMode;
	}

	bool IsAnim() const { return m_bAnim;}

#define MINT_PICTURE	"Picture"
	virtual const char* GetClassName(void){ return MINT_PICTURE; }
};


} // namespace mint3
*/