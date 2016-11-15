/************************************************************************
MAnimation

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Animation 위젯 클래스 입니다.
*************************************************************************/



#include "stdafx.h"
#include "MAnimation.h"



// namespace : mint3
namespace mint3
{


// 플레이 방향
enum MPLAY_DIRECTION
{
	MPD_FORWARD		= 1,
	MPD_BACKWARD	= -1
};



/*	MAnimation
	생성자
*/
MAnimation::MAnimation( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
: MPicture( szName, pParent, pListener), m_pAnimationTemplate(NULL)
{
	// 현재 씬 번호 초기화
	m_nSceneNum = 0;

	// 타이머 초기화
	m_dwTimer = timeGetTime();

	// 애니메이션을 진행하는지 여부
	m_bPlay = true;

	// 플레이 방식은 정방향 반복 재생
	m_nPlayMode = MPM_FORWARD;

	// 애니메이션 진행 방향
	m_nPlayDir = MPD_FORWARD;
}


/*	~MAnimation
	소멸자
*/
MAnimation::~MAnimation()
{
}

/*	SetCurrScene
	현재 씬 번호를 설정하는 함수
*/
void MAnimation::SetCurrScene( int nNum)
{
	if ( (nNum >= 0) || (nNum < m_pAnimationTemplate->GetSplitCount() ))
		m_nSceneNum = nNum;
}


/*	GetCurrScene
	현재 씬 번호를 구하는 함수
*/
int MAnimation::GetCurrScene()
{
	return m_nSceneNum;
}


/*	SetPlay
	애니메이션을 플레이 시키는 함수
*/
void MAnimation::SetPlay( bool bPlay /*=true*/)
{
	m_bPlay = bPlay;
}

/*	SetPlayMode
	플레이 방식을 설정하는 함수
*/
void MAnimation::SetPlayMode( MPLAYMODE mode)
{
	m_nPlayMode = mode;


	switch ( m_nPlayMode)
	{
		case MPM_FORWARD :
		case MPM_FORWARDONCE :
			m_nPlayDir = MPD_FORWARD;
			break;

		case MPM_BACKWARD :
		case MPM_BACKWARDONCE :
			m_nPlayDir = MPD_BACKWARD;
			break;

		default :
			break;
	}
}


/*	IsPlaying
	애니메이션이 플레이 중인지 확인하는 함수
*/
bool MAnimation::IsPlaying()
{
	return m_bPlay;
}


/*	OnEvent
	OnEvent 이벤트 핸들러
*/
bool MAnimation::OnEvent( MEvent* pEvent, MListener* pListener)
{
	return false;
}


/*	OnDraw
	OnDraw 이벤트 핸들러
*/
void MAnimation::OnDraw( MDrawContext* pDC)
{
	// 비트맵이 없으면 그냥 종료
	if ( m_pAnimationTemplate == NULL)
		return;

	MBitmap* pBitmap = m_pAnimationTemplate->GetBitmap();
	
	if(!pBitmap)
		return;

	
	// 그리기 모드 설정
	DWORD tmpDrawMode = pBitmap->GetDrawMode();
	pBitmap->SetDrawMode( m_dwRotate);


	// Colorize 색상 설정
	MCOLOR tmpColorize = pDC->GetBitmapColor();
	pDC->SetBitmapColor( m_Colorize);


	// 비트맵 설정
	pDC->SetBitmap( pBitmap);


	// 해당 씬의 비트맵을 구한다
	
	int nSplitCount = m_pAnimationTemplate ? m_pAnimationTemplate->GetSplitCount() : 1;

	MRECT rectScene;
	rectScene.w = pBitmap->GetWidth() / nSplitCount;
	rectScene.h = pBitmap->GetHeight();
	rectScene.x = rectScene.w * m_nSceneNum;
	rectScene.y = 0;


	// 비트맵을 늘려찍는다면 원래 클라이언트 영역 전체에 그린다
	if ( m_bStretch)
	{
		MRECT rect = GetClientRect();
		pDC->Draw( rect.x, rect.y, rect.w, rect.h, rectScene.x, rectScene.y, rectScene.w, rectScene.h);
	}

	// 이미지를 그대로 찍는다면...
	else
		pDC->Draw( 0, 0, rectScene.w, rectScene.h, rectScene.x, rectScene.y, rectScene.w, rectScene.h);


	// Colorize 색상 복원
	pDC->SetBitmapColor( tmpColorize);


	// 그리기 모드 복원
	pBitmap->SetDrawMode( tmpDrawMode);
}


/*	OnRun
	OnRun 메시지 핸들러
*/
void MAnimation::OnRun(void)
{
	if(m_pAnimationTemplate)
		m_pAnimationTemplate->UpdateSceneNum( m_nPlayMode, m_nSceneNum, m_bPlay, m_nPlayDir, m_dwTimer );
}


} // namespace mint3














/************************************************************************************
여기서부터 이전코드입니다



#include "stdafx.h"
#include "MAnimation.h"

namespace mint3 {

#define DELAY_CONSTANT	1

void MAnimation::OnDraw(MDrawContext* pDC)
{
	if ( m_pAniBitmap == NULL)
		return;

	// Get current time
	int nCurTime = timeGetTime();

	// Get current frame
	int nCurFrame = GetCurrentFrame();
	
	// Get frame count
	int nFrameCount = m_pAniBitmap->GetFrameCount();


	if ( m_nPlayMode == MAPM_FORWARDONCE)
	{
		m_nCurrFrame = nCurFrame;

		if ( nCurFrame >= nFrameCount)
            m_nCurrFrame = nFrameCount - 1;
	}

	else if ( m_nPlayMode == MAPM_REPETITION)
		m_nCurrFrame = nCurFrame % nFrameCount;

	else if ( m_nPlayMode == MAPM_FORWARDNBACKWARD)
	{
		int nIterCount = nCurFrame / nFrameCount;
		m_nCurrFrame = nCurFrame % nFrameCount;
		if ( (nIterCount % 2) == 1)			// 홀수번째 Play이면 Backward
			m_nCurrFrame = nFrameCount - m_nCurrFrame - 1;
	}

	MBitmap* pBitmap = m_pAniBitmap->Get( m_nCurrFrame);
	pDC->SetBitmap( pBitmap);
	MRECT r = GetClientRect();
	pDC->Draw( r.x, r.y, r.w, r.h);
}

MBitmap* MAnimation::GetBitmap()
{
	if(m_pAniBitmap == NULL) return NULL;
	int nCurTime = timeGetTime();
	int nCurFrame;
	if(m_nDelay!=0) nCurFrame = (nCurTime-m_nStartTime) / (m_nDelay*DELAY_CONSTANT);
	else nCurFrame = (nCurTime-m_nStartTime) / DELAY_CONSTANT;
	int nFrameCount = m_pAniBitmap->GetFrameCount();
	if(nFrameCount==0) return NULL;

	if(m_nPlayMode==MAPM_FORWARDONCE){
		if(nCurFrame>=nFrameCount) nCurFrame = nFrameCount-1;
	}
	else if(m_nPlayMode==MAPM_REPETITION){
		nCurFrame = nCurFrame % nFrameCount;
	}
	else if(m_nPlayMode==MAPM_FORWARDNBACKWARD){
		int nIterCount = nCurFrame/nFrameCount;
		nCurFrame = (nCurFrame%nFrameCount);
		if(nIterCount%2==1){	// 홀수번째 Play이면 Backward
			nCurFrame = nFrameCount - nCurFrame - 1;
		}
	}

	return m_pAniBitmap->Get(nCurFrame);
}

MAnimation::MAnimation(const char* szName, MAniBitmap* pAniBitmap, MWidget* pParent)
: MWidget(szName, pParent, NULL)
{
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_nDelay = pAniBitmap->GetDelay();
	else m_nDelay = 1;	// Default
	m_nStartTime = timeGetTime();

	m_nPlayMode = MAPM_FORWARDONCE;
	m_nCurrFrame = 0;
	m_bRunAnimation = true;
}

void MAnimation::SetAniBitmap(MAniBitmap* pAniBitmap)
{
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_nDelay = pAniBitmap->GetDelay();
}

void MAnimation::InitStartTime()
{
	m_nStartTime = timeGetTime();
}


void MAnimation::SetRunAnimation( bool bRun)
{
	m_bRunAnimation = bRun;

	SetCurrentFrame( m_nCurrFrame);
}

int MAnimation::GetCurrentFrame( void)
{
	if ( !m_bRunAnimation)
		return m_nCurrFrame;

	// Get current time
	int nCurrTime = timeGetTime();

	// Get current frame
	int nCurrFrame;

	if ( m_nDelay != 0)
		nCurrFrame = (nCurrTime - m_nStartTime) / (m_nDelay * DELAY_CONSTANT);
	else
		nCurrFrame = (nCurrTime - m_nStartTime) / DELAY_CONSTANT;

	return nCurrFrame;
}

void MAnimation::SetCurrentFrame( int nFrame)
{
	m_nCurrFrame = nFrame;

	if ( nFrame < m_pAniBitmap->GetFrameCount())
        m_nStartTime = timeGetTime() - ( nFrame * m_nDelay * DELAY_CONSTANT);
}

} // namespace mint3
*/