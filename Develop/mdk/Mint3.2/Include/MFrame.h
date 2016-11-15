#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{


class MFrame;





/// class : MFrameLook
class MFrameLook : public MLookBase
{
/// Member variables
protected:
	int			m_nTitlebarHeight;			///< 타이틀바 높이
	bool		m_bTitleShadow;				///< 타이틀의 그림자를 사용하는지 여부를 설정하는 변수
	MPOINT		m_ptTitleOffset;			///< 타이틀 위치 오프셋 값을 저장하는 변수
	MRECT		m_rIconRect;				///< 아이콘 영역
	MRECT		m_rMagneticectOffset;		///< 자석 효과 영역 오프셋


/// Member functions
public:
	MFrameLook();

	void SetTitlebarHeight( int height)							{ m_nTitlebarHeight = height;					}
	int GetTitlebarHeight() const								{ return m_nTitlebarHeight;						}

	void SetTitleShadow( bool bShadow =true)					{ m_bTitleShadow = bShadow;						}
	bool GetTitleShadow() const									{ return m_bTitleShadow;						}

	void SetTitleOffset( int x, int y)							{ m_ptTitleOffset = MPOINT( x, y);				}
	MPOINT GetTitleOffset() const								{ return m_ptTitleOffset;						}

	void SetIconRect( int x, int y, int w, int h)				{ m_rIconRect = MRECT( x, y, w, h);				}
	MRECT GetIconRect() const									{ return m_rIconRect;							}

	void SetMagneticRectOffset( int x, int y, int w, int h)		{ m_rMagneticectOffset = MRECT( x, y, w, h);	}
	MRECT GetMagneticRectOffset() const							{ return m_rMagneticectOffset;					}


	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawTitlebar( MFrame* pFrame, MRECT& r, MDrawContext* pDC);
};









/// 움직일 수 있는 프레임으로 구성된 위젯
class MFrame : public MWidget
{
protected:
	bool			m_bShowTitlebar;		///< Show titlebar
	MBitmap*		m_pIcon;				///< Icon image
	bool			m_bMovable;				///< Is movable
	bool			m_bMoving;				///< Is moving widget
	bool			m_bResizable;			///< Is Resizable
	int				m_nResizing;			///< Is resizing widget
	MPOINT			m_ptLastCursor;			///< Last cursor position
	MTimer			m_tFade;



public:
	MFrame( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual void SetMovable( bool bMovable) override	{ m_bMovable = bMovable;			}
	virtual bool IsMovable() override					{ return m_bMovable;				}

	virtual void SetResizable( bool bResize) override	{ m_bResizable = bResize;			}
	virtual bool IsResizable() override					{ return m_bResizable;				}

	virtual bool IsFocus();

	void ShowTitlebar( bool bShow)						{ m_bShowTitlebar = bShow;			}
	bool IsShowTitlebar() const							{ return m_bShowTitlebar;			}

	void SetIcon( MBitmap* pIcon)						{ m_pIcon = pIcon;					}
	MBitmap* GetIcon() const							{ return m_pIcon;					}


protected:
	virtual int GetDrawState() override;
	virtual void InitMouseState() override;


protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnPreDraw( MDrawContext* pDC) override;
	virtual void OnShow( bool bShow) override;
	virtual	void OnActivate( bool bActivate) override;
	virtual int OnNcHitTest( MEvent* pEvent) override;



// LUA script
public:
	void	glueShowTitlebar( bool bShow)			{ ShowTitlebar( bShow);			}
	bool	glueIsShowTitlebar() const				{ return IsShowTitlebar();		}
	void	glueSetIcon( const char* szIcon);
	const char* glueGetIcon();



#define MINT_FRAME	"Frame"
	virtual const char* GetClassName() override		{ return MINT_FRAME;				}
};

} // namespace mint3