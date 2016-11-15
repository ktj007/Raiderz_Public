#include "stdafx.h"
#include "MAnimationTemplate.h"

// namespace : mint3
namespace mint3
{


// 플레이 방향
enum MPLAY_DIRECTION
{
	MPD_FORWARD		= 1,
	MPD_BACKWARD	= -1
};

MAnimationTemplate::MAnimationTemplate()
{
	m_pBitmap = NULL;

	// 씬 카운트값 초기화
	m_nSplitCount = 1;

	// 씬 딜레이 설정
	m_Delay.push_back( 200);
}

MAnimationTemplate::~MAnimationTemplate()
{
	if ( !m_Delay.empty())
		m_Delay.clear();
}


/*	SetDelay
	씬의 딜레이를 설정하는 함수
*/
void MAnimationTemplate::SetDelay( string strDelay)
{
	// 스트링의 종료를 확실히 표시한다
	strDelay += ',';


	// 딜레이 리스트 초기화
	if ( !m_Delay.empty())
		m_Delay.clear();

	// 씬 나눔 갯수 초기화
	m_nSplitCount = 0;


	// 딜레이 값을 읽어들임
	char buffer[ 10];
	memset( buffer, 0, sizeof( buffer));
	int count = 0;

	for ( int i = 0;  i < (int)strDelay.size();  i++)
	{
		char ch = strDelay.c_str()[ i];

		if ( (ch >= '0') && (ch <= '9') && (count < 10))
		{
			buffer[ count++] = ch;
		
			continue;
		}


		if ( count > 0)
		{
			int nDelay = atoi( buffer);
			m_Delay.push_back( nDelay);
			m_nSplitCount++;
		}


		// 변수 초기화
		memset( buffer, 0, sizeof( buffer));
		count = 0;
	}
}


/*	SetDelay
	지정한 씬의 딜레이를 설정하는 함수
*/
void MAnimationTemplate::SetDelay( int nSceneNum, int nDelay)
{
	if ( (nSceneNum < 0) || (nSceneNum >= m_nSplitCount))
		return;


	m_Delay[ nSceneNum] = nDelay;
}


void MAnimationTemplate::UpdateSceneNum( MPLAYMODE ePlayMode, int& inoutSceneNum , bool& inoutbPlay, int& inoutPlayDir,DWORD& inoutTimer)
{
	DWORD curr = timeGetTime();

	// 경과된 시간을 구한다
	DWORD diff = curr - inoutTimer;

	// 씬이 2장 이상 되지 않거나 플레이 상태가 아니면 그냥 종료한다
	if ( (m_nSplitCount < 2) || !inoutbPlay)
	{
		inoutTimer = curr;
		return;
	}

	while ( 1)
	{
		// 현재 씬의 딜레이 값을 구한다
		DWORD dwDelay = (DWORD)m_Delay[ inoutSceneNum];


		// 아직 현재 씬의 딜레이 만큼 시간이 안지났으면 종료
		if ( diff < dwDelay)
			break;


		// 시간이 지났으면 다음 씬으로 이동
		inoutSceneNum += inoutPlayDir;


		switch ( ePlayMode)
		{
			case MPM_FORWARD :
				if ( inoutSceneNum >= m_nSplitCount)
					inoutSceneNum = 0;
				break;

			case MPM_BACKWARD :
				if ( inoutSceneNum < 0)
					inoutSceneNum = m_nSplitCount - 1;
				break;

			case MPM_FORWARDONCE :
				if ( inoutSceneNum >= m_nSplitCount)
				{
					inoutSceneNum = 0;
					inoutbPlay = false;
				}
				break;
				
			case MPM_BACKWARDONCE :
				if ( inoutSceneNum < 0)
				{
					inoutSceneNum = m_nSplitCount - 1;
					inoutbPlay = false;
				}
				break;

			case MPM_FORWARDBACKWARD :
				if ( inoutSceneNum >= m_nSplitCount)
				{
					inoutSceneNum = m_nSplitCount - 2;
					inoutPlayDir = MPD_BACKWARD;
				}

				else if ( inoutSceneNum < 0)
				{
					inoutSceneNum = 1;
					inoutPlayDir = MPD_FORWARD;
				}
				break;
		}

		// 타이머 값을 업데이트 함
		inoutTimer += dwDelay;
		diff -= dwDelay;
	}
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