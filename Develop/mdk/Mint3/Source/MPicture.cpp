/************************************************************************
MPicture

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Picture 위젯 클래스 입니다.
*************************************************************************/


#include "stdafx.h"
#include "MPicture.h"



// namespace : mint3
namespace mint3
{

/*	MPicture
	생성자
*/
MPicture::MPicture( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
: MWidget( szName, pParent, pListener)
{
	// 비트맵 포인터 초기화
	m_pBitmap = NULL;

	// 비트맵 늘려 찍기 사용 안함
	m_bStretch = false;	

	// 비트맵을 회전시키지 않음
	m_dwRotate = MBM_Normal;

	// Colorize 적용 안함
	m_Colorize = MCOLOR( 255, 255, 255);
}


/*	SetBitmap
	비트맵 리소스를 설정하는 함수
*/
void MPicture::SetBitmap( MBitmap* pBitmap)
{
	m_pBitmap = pBitmap;
}


/*	GetBitmap
	비트맵 리소스를 구하는 함수
*/
MBitmap* MPicture::GetBitmap()
{
	return m_pBitmap;
}



/*	SetStretch
	비트맵을 늘려 찍는지 설정하는 함수
*/
void MPicture::SetStretch( bool bStretch)
{
	m_bStretch = bStretch;
}


/*	SetFlipHorizontal
	비트맵을 좌우로 뒤집는 함수
*/
void MPicture::SetFlipHorizontal()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;

	unsigned int nBit = ((m_dwRotate & nBit90L) || (m_dwRotate & nBit90R))  ?  MBM_FlipUD : MBM_FlipLR;


	// 이미 좌우를 뒤집은 상태면...
	if ( m_dwRotate & nBit)		m_dwRotate &= ~nBit;
	else						m_dwRotate |= nBit;
}


/*	SetFlipVertical
	비트맵을 상하로 뒤집는 함수
*/
void MPicture::SetFlipVertical()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;

	unsigned int nBit = ((m_dwRotate & nBit90L) || (m_dwRotate & nBit90R))  ?  MBM_FlipLR : MBM_FlipUD;


	// 이미 상하를 뒤집은 상태면...
	if ( m_dwRotate & nBit)		m_dwRotate &= ~nBit;
	else						m_dwRotate |= nBit;
}


/*	SetRotate90CW	
	비트맵을 시계 방향으로 90도 회전시키는 함수
*/
void MPicture::SetRotate90CW()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;


	if ( m_dwRotate & nBit90L)
		m_dwRotate &= ~nBit90L;
	else if ( m_dwRotate & nBit90R)
	{
		SetRotate180();

		m_dwRotate &= ~nBit90R;
	}
	else
		m_dwRotate |= nBit90R;
}


/*	SetRotate90CCW
	비트맵을 반시계 방향으로 90도 회전시키는 함수
*/
void MPicture::SetRotate90CCW()
{
	unsigned int nBit90L = MBM_RotL90;
	unsigned int nBit90R = MBM_RotR90;


	if ( m_dwRotate & nBit90R)
		m_dwRotate &= ~nBit90R;
	else if ( m_dwRotate & nBit90L)
	{
		SetRotate180();

		m_dwRotate &= ~nBit90L;
	}
	else
		m_dwRotate |= nBit90L;
}


/*	SetRotate180
	비트맵을 반시계 방향으로 180도 회전시키는 함수
*/
void MPicture::SetRotate180()
{
	SetFlipHorizontal();
	SetFlipVertical();
}


/*	SetNormal
	비트맵 회전을 리셋하는 함수
*/
void MPicture::ResetRotate()
{
	m_dwRotate = MBM_Normal;
}


/*	SetColorize
	비트맵 이미지의 colorize를 설정하는 함수
*/
void MPicture::SetColorize( MCOLOR color)
{
	m_Colorize = color;
}


/*	OnEvent
	OnEvent 이벤트 핸들러
*/
bool MPicture::OnEvent( MEvent* pEvent, MListener* pListener)
{
	return false;
}


/*	OnDraw
	OnDraw 이벤트 핸들러
*/
void MPicture::OnDraw( MDrawContext* pDC)
{
	// 비트맵이 없으면 그냥 종료
	if ( m_pBitmap == NULL)
		return;


	// 회전 설정
	DWORD tmpDrawMode = m_pBitmap->GetDrawMode();
	m_pBitmap->SetDrawMode( m_dwRotate);

	
	// Colorize 색상 설정
	MCOLOR tmpColorize = pDC->GetBitmapColor();
	pDC->SetBitmapColor( m_Colorize);


	// 비트맵 설정
	pDC->SetBitmap( m_pBitmap);


	// 비트맵을 늘려찍는다면 원래 클라이언트 영역 전체에 그린다
	if ( m_bStretch)
	{
		MRECT rect = GetClientRect();
		pDC->Draw( rect.x, rect.y, rect.w, rect.h, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
	}
	else if ( m_bTiled)
	{
		MRECT rect = GetClientRect();
		pDC->DrawTiled( rect.x, rect.y, rect.w, rect.h, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
	}
	else
		pDC->Draw( 0, 0, 0, 0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());


	// Colorize 색상 복원
	pDC->SetBitmapColor( tmpColorize);


	// 그리기 모드 복원
	m_pBitmap->SetDrawMode( tmpDrawMode);
}

void MPicture::glueSetBitmapName( const char* szBitmapName )
{
	MBitmap* pBitmap = MBitmapManager::Get(szBitmapName);
	if (pBitmap == NULL) return;

	SetBitmap(pBitmap);
}

} // namespace mint3


















/**********************************************************************************
여기서부터 이전 코드입니다



#include "stdafx.h"
#include "MPicture.h"
#include "math.h"

namespace mint3 {

void MPicture::OnDraw(MDrawContext* pDC)
{
 	if(m_pBitmap==NULL) return;
	pDC->SetBitmap(m_pBitmap);

	MCOLOR tempColor = pDC->GetBitmapColor();
	pDC->SetBitmapColor( m_BitmapColor );
	DWORD tempDrawMode = m_pBitmap->GetDrawMode();
	m_pBitmap->SetDrawMode( m_dwRotate );

	int bw, bh;
	if( m_iStretch == 2 )	bw	= m_Rect.w;
	else		bw	= m_pBitmap->GetWidth();
	if( m_iStretch == 1 )	bh	= m_Rect.h; 
	else		bh	= m_pBitmap->GetHeight();
    
	int w, h;
	if (m_iStretch != 0)	{
		w = m_Rect.w;
		h = m_Rect.h;
	}
 	else	{
		w = bw;
		h = bh;
	}

 	if(m_bAnim)
		OnAnimDraw( pDC, 0,0, w, h, 0,0, bw, bh );
	else
 		pDC->Draw( 0, 0, w, h, 0, 0, bw, bh );

	pDC->SetBitmapColor( tempColor );
	m_pBitmap->SetDrawMode( tempDrawMode );
}

void MPicture::SetAnimation( int animType, float fAnimTime )
{
	m_bAnim = true;
	m_fAnimTime = fAnimTime;
	m_iAnimType = animType;
	m_dwCurrentTime = timeGetTime();
}

void MPicture::OnAnimDraw( MDrawContext* pDC, int x, int y, int w, int h, int bx, int by, int bw, int bh )
{
   	DWORD e_time = timeGetTime() - m_dwCurrentTime;
 	float ratio	= e_time / m_fAnimTime;
	
   	if( ratio >= 1.0f ) 
	{
		if(m_iAnimType == 2)
		{
 			m_iAnimType = 0;
			m_dwCurrentTime = timeGetTime();
			if(m_bSwaped)
			{
				MCOLOR c = m_BitmapColor;
				m_BitmapColor = m_BitmapReseveColor;
				m_BitmapReseveColor = c;
				m_bSwaped = false;
			}
			return;
		}
		else if(m_iAnimType == 3) 
		{
			m_iAnimType = 1;
			m_dwCurrentTime = timeGetTime();
			if(m_bSwaped)
			{
 				MCOLOR c = m_BitmapColor;
				m_BitmapColor = m_BitmapReseveColor;
				m_BitmapReseveColor = c;
				m_bSwaped = false;
			}
			return;
		}
		else
		{
			m_bAnim = false;
			pDC->Draw( 0, 0, w, h, 0, 0, bw, bh );
			return;
		}
	}

	int i_offset;
 	
  	if( m_iAnimType == 0 )
	{		
   		i_offset = (int)(bw*sqrtf(ratio));
		pDC->Draw( x,y,i_offset, h, bx+(bw-i_offset), by, i_offset, bh );
 	}
	else if( m_iAnimType == 1 )
	{
 		i_offset = (int)(bw*sqrtf(ratio));
		pDC->Draw( x+w-i_offset,y,i_offset, h, bx+(bw-i_offset), by, i_offset, bh );
	}
 	else if( m_iAnimType == 2 ) //<----
	{
//  		i_offset = w - w*ratio*ratio;	
		i_offset = (int)(bw - bw*sqrtf(ratio));	
		if(!m_bSwaped)
		{
			MCOLOR c = m_BitmapColor;
			m_BitmapColor = m_BitmapReseveColor;
			m_BitmapReseveColor = c;
			m_bSwaped = true;
			pDC->SetBitmapColor( m_BitmapColor );
		}
 		pDC->Draw( x,y,i_offset, h, bx+(bw-i_offset), by, i_offset, bh );
	}
	else if( m_iAnimType == 3 )
	{
		//i_offset = w - w*ratio*ratio;	
		i_offset = (int)(bw - bw*sqrtf(ratio));	
		if(!m_bSwaped)
		{
			MCOLOR c = m_BitmapColor;
			m_BitmapColor = m_BitmapReseveColor;
			m_BitmapReseveColor = c;
			m_bSwaped = true;
			pDC->SetBitmapColor( m_BitmapColor );
		}
		pDC->Draw( x+w-i_offset,y,i_offset, h, bx+(bw-i_offset), by, i_offset, bh );
	}
}

MPicture::MPicture(const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	m_pBitmap = NULL;
	m_bAnim = false;
	m_iStretch = 0;
	m_BitmapColor = 0xFFFFFFFF;
	m_BitmapReseveColor = 0xFFFFFFFF;
	m_dwRotate = MBM_Normal;
	m_bSwaped = false;
}

void MPicture::SetBitmap(MBitmap* pBitmap)
{
	m_pBitmap = pBitmap;

	if(m_pBitmap==NULL) return;

	if( m_iStretch == 0 ) SetSize(m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
}

MBitmap* MPicture::GetBitmap(void)
{
	return m_pBitmap;
}

void MPicture::SetBitmapColor( MCOLOR color )
{
	m_BitmapReseveColor = m_BitmapColor;
	m_BitmapColor.r = color.r;
	m_BitmapColor.g = color.g;
	m_BitmapColor.b = color.b;
}

} // namespace mint3
*/
