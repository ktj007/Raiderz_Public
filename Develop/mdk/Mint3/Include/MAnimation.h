/************************************************************************
MAnimation

Prog. : Lim Dong Hwan
Date  : 10.Sep.2007
Desc. : Animation 위젯 클래스 입니다.
*************************************************************************/

#pragma once

#include "MPicture.h"
#include "MAnimationTemplate.h"

// namespace : mint3
namespace mint3
{

class MAnimationTemplate;

/*	MAnimation
	Animation 위젯 클래스
*/
class MAnimation : public MPicture
{
// Member variables
protected:
	MAnimationTemplate*	m_pAnimationTemplate;	// 애니메이션 템플릿

	int				m_nSceneNum;			// 현재 씬 번호

	DWORD			m_dwTimer;				// 타이머 변수

	bool			m_bPlay;				// 애니메이션을 진행하는지 여부

	MPLAYMODE		m_nPlayMode;			// 플레이 방식

	int				m_nPlayDir;				// 애니메이션 진행 방향

// Member functions
public:
	// 생성자
	MAnimation( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	// 소멸자
	virtual ~MAnimation();

	void SetAnimationTemplate( MAnimationTemplate* pAnimationTemplate );

	// 현재 씬 번호를 설정하는 함수
	void SetCurrScene( int nNum);

	// 현재 씬 번호를 구하는 함수
	int GetCurrScene();

	// 플레이 방식을 설정하는 함수
	void SetPlayMode( MPLAYMODE mode);

	// 애니메이션을 플레이 시키는 함수
	void SetPlay( bool bPlay =true);

	// 애니메이션이 플레이 중인지 확인하는 함수
	bool IsPlaying();



// Event handler
protected:
	// OnDraw 이벤트 핸들러
	virtual void OnDraw( MDrawContext* pDC);

	// OnEvent 이벤트 핸들러
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);

	// OnRun 메시지 핸들러
	void OnRun(void);


#define MINT_ANIMATION		"Animation"
	virtual const char* GetClassName(void)		{ return MINT_ANIMATION; }
};

//////////////////////////////////////////////////////////////////////////
inline void MAnimation::SetAnimationTemplate( MAnimationTemplate* pAnimationTemplate ) { m_pAnimationTemplate = pAnimationTemplate; }


} // namespace mint3















/*****************************************************************************
여기서부터 이전코드 입니다

#pragma once

#include "MWidget.h"

namespace mint3 {

enum MAPlayMode{
	MAPM_FORWARDONCE,
	MAPM_FORWARDNBACKWARD,
	MAPM_REPETITION,
};

class MAnimation : public MWidget{
private:
	unsigned int	m_nStartTime;
	MAniBitmap*		m_pAniBitmap;
public:
	int				m_nDelay;
	MAPlayMode		m_nPlayMode;
protected:
	virtual void OnDraw(MDrawContext* pDC);
public:
	MAnimation(const char* szName=NULL, MAniBitmap* pAniBitmap=NULL, MWidget* pParent=NULL);
	void SetAniBitmap(MAniBitmap* pAniBitmap);
	void InitStartTime();
	MBitmap* GetBitmap();

#define MINT_ANIMATION	"Animation"
	virtual const char* GetClassName(void){ return MINT_ANIMATION; }



//	동환이의 추가 부분 :
//	아래 추가된 부분은 특정 상황마다 이미지를 수시로 변경하는 경우 이를 애니메이션 위젯으로
//	해결하기 위해서 추가한 것이다. 쉽게 말해서 애니메이션을 정지시킨 상태에서 유저가 보여줄
//	이미지 번호만 바꿔주는 형식이다.

protected:
	int				m_nCurrFrame;;			// 현재 보여지고 있는 애니메이션 이미지 번호

public:
	bool			m_bRunAnimation;		// 현재 애니메이션이 되고있는지 여부
	bool			GetRunAnimation( void)  { return m_bRunAnimation;}
	void			SetRunAnimation( bool bRun);
	int				GetCurrentFrame( void);
	void			SetCurrentFrame( int nFrame);
};

} // namespace mint3
*/