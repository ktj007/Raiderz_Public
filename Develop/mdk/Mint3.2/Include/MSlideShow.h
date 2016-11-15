#pragma once

#include "MWidget.h"


// namespace : mint3
namespace mint3
{


/// class : MSlideShow
class MSlideShow : public MWidget
{
/// Member variables
protected:
	MBitmap*		m_pBitmap;				///< 비트맵 이미지 포인터
	int				m_nRow;					///< 가로로 나눈 개수
	int				m_nCol;					///< 세로로 나눈 개수
	MSIZE			m_SceneSize;			///< 씬 사이즈
	unsigned int	m_nMaxSceneCount;		///< 최대 가능한 장면 개수
	unsigned int	m_nSceneCount;			///< 사용 가능한 장면 개수
	unsigned int	m_nCurScene;			///< 현재 장면 번호
	unsigned int	m_nPrevScene;			///< 이전 장면 번호
	bool			m_bSlideShow;			///< 슬라이드 쇼
	unsigned int	m_nDelayTime;			///< 슬라이드 쇼 지연 시간(ms)
	DWORD			m_dwSlideShowTimer;		///< 슬라이드 타이머
	bool			m_bPlayOnce;			///< 한 번만 재생하는지 여부
	MTimer			m_tFade;				///< 페이드 타이머
	MDrawEffect		m_Effect;				///< 비트맵 이펙트(add, multiple)
	DWORD			m_dwRotate;				///< 비트맵 회전 상태(좌우 반전, 상하반전, 90도 회전, 90도 역회전)
	MCOLOR			m_Colorize;				///< 비트맵 이미지의 colorize 색상


	
	
/// Member functions
public:
	MSlideShow( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	void SetBitmap( MBitmap* pBitmap);
	MBitmap* GetBitmap() const						{ return m_pBitmap;									}

	int SliceScene( int _row, int _col =0);
	
	MSIZE GetSceneSize() const						{ return m_SceneSize;								}

	int SetSceneCount( int nCount);
	int GetSceneCount() const						{ return m_nSceneCount;								}

	int SetCurScene( int nCurScene);
	int GetCurScene() const							{ return m_nCurScene;								}

	bool SlideShow( bool bSlideShow);
	bool IsSlideShow() const						{ return m_bSlideShow;								}

	void SetDelayTime( int nMilliSec)				{ m_nDelayTime = (unsigned int)nMilliSec;			}
	int GetDelayTime()								{ return (int)m_nDelayTime;							}

	void SetFadeTime( int nMilliSec)				{ m_tFade.SetArrivalTime( (DWORD)nMilliSec);		}
	int GetFadeTime()								{ return (int)m_tFade.GetArrivalTime();				}

	void SetPlayOnce( bool bSet)					{ m_bPlayOnce = bSet;								}
	bool IsPlayOnce() const							{ return m_bPlayOnce;								}

	MDrawEffect SetEffect( MDrawEffect _effect);
	MDrawEffect GetEffect() const					{ return m_Effect;				}

	MCOLOR SetColorize( MCOLOR& color);
	MCOLOR GetColorize() const						{ return m_Colorize;			}

	void SetFlipHorizontal();
	void SetFlipVertical();
	void SetRotate90CW();
	void SetRotate90CCW();
	void SetRotate180();
	void ResetRotate()								{ m_dwRotate = MBM_NORMAL;		}



protected:
	MRECT GetSceneRect( int nSceneNum) const;



/// Event handler
protected:
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnDraw( MDrawContext* pDC) override;



/// LUA script
public:
	void glueSetImage( const char* szName);	
	const char* glueGetImage();
	MWLua::ret_int2 glueGetSceneSize() const		{ return MWLua::ret_int2( m_SceneSize.w, m_SceneSize.h);	}
	int glueSliceScene( int _row, int _col)			{ return SliceScene( _row, _col);							}
	int glueSetSceneCount( int nCount)				{ return SetSceneCount( nCount);							}
	int glueGetSceneCount() const					{ return GetSceneCount();									}
	int glueSetCurScene( int nCurScene)				{ return SetCurScene( nCurScene);							}
	int glueGetCurScene() const						{ return GetCurScene();										}
	bool glueSetSlideShow( bool bSlideShow)			{ return SlideShow( bSlideShow);							}
	bool glueIsSlideShow() const					{ return IsSlideShow();										}
	void glueSetDelayTime( int nMilliSec)			{ SetDelayTime( nMilliSec);									}
	int glueGetDelayTime()							{ return GetDelayTime();									}
	void glueSetFadeTime( int nMilliSec)			{ SetFadeTime( nMilliSec);									}
	int glueGetFadeTime()							{ return GetFadeTime();										}
	void glueSetPlayOnce( bool bSet)				{ SetPlayOnce( bSet);										}
	bool glueIsPlayOnce() const						{ return IsPlayOnce();										}
	void glueSetEffect( const char* szEffect);
	const char* glueGetEffect();
	MWLua::ret_int4 glueSetColorize( int r, int g, int b);
	MWLua::ret_int4 glueGetColorize();
	void glueSetFlipHorizontal()					{ SetFlipHorizontal();								}
	void glueSetFlipVertical()						{ SetFlipVertical();								}
	void glueSetRotate90CW()						{ SetRotate90CW();									}
	void glueSetRotate90CCW()						{ SetRotate90CCW();									}
	void glueSetRotate180()							{ SetRotate180();									}
	void glueResetRotate()							{ ResetRotate();									}



#define MINT_SLIDESHOW		"SlideShow"
	virtual const char* GetClassName() override		{ return MINT_SLIDESHOW;									}
};


} // namespace mint3


