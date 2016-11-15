#pragma once

#include "MWidget.h"
#include "MButton.h"
#include "MLookBase.h"


namespace mint3
{


class MTextFrame;



/// class : MTextFrameLook
class MTextFrameLook : public MLookBase
{
/// Member variables
protected:
	int		m_nTitlebarHeight;
	int		m_nItemHeight;


/// Member functions
public:
	MTextFrameLook();

	void SetTitlebarHeight( int height)					{ m_nTitlebarHeight = height;		}
	int GetTitlebarHeight() const						{ return m_nTitlebarHeight;			}

	void SetItemHeight( int height)						{ m_nItemHeight = height;			}
	int GetItemHeight()									{ return m_nItemHeight;				}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawItems( MTextFrame* pTextFrame, MDrawContext* pDC);
	virtual void OnDrawItemText( MTextFrame* pTextFrame, size_t index, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawTitlebar( MTextFrame* pTextFrame, MRECT& r, MDrawContext* pDC);
};






/// class : MTextFrame
class MTextFrame : public MWidget
{
/// Member variables
protected:
	// Frame
	bool			m_bMovable;				///< Is movable
	bool			m_bMoving;				///< Is moving widget
	bool			m_bResizable;			///< Is Resizable
	int				m_nResizing;			///< Is resizing widget
	MPOINT			m_ptLastCursor;			///< Last cursor position
	bool			m_bShowTitleBar;
	MTimer			m_tShowTitleBar;

	// LinePrinter
	string*			m_StringList;
	size_t			m_nHead, m_nTail;
	size_t			m_nCapacity;
	MScrollBar*		m_pScrollBar;
	bool			m_bUpdateScrollBar;

	// Button
	MButton*		m_pCloseBtn;



/// Member functions
public:
	MTextFrame( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MTextFrame();

	virtual void SetMovable( bool bMovable) override	{ m_bMovable = bMovable;				}
	virtual bool IsMovable() override					{ return m_bMovable;					}

	virtual void SetResizable( bool bResize) override	{ m_bResizable = bResize;				}
	virtual bool IsResizable() override					{ return m_bResizable;					}

	virtual bool IsFocus();
	void ShowTitleBar( bool bShow)						{ m_bShowTitleBar = bShow;				}
	bool IsShowTitleBar() const							{ return m_bShowTitleBar;				}


	// LinePrinter
	void AddString( const char* szString);
	void ResetContent();
	void SetCapacity( size_t nCapacity);
	size_t GetCapacity() const							{ return m_nCapacity;					}
	size_t GetHead() const								{ return m_nHead;						}
	size_t GetTail() const								{ return m_nTail;						}
	const char* GetItemText( size_t nIndex) const		{ return m_StringList[ nIndex].c_str();	}
	int GetItemHeight();


	// Scrollbar
	virtual MScrollBar* GetScrollBar() override			{ return m_pScrollBar;					}
	void ScrollPage( int delta);


protected:
	void UpdateScrollBar()								{ m_bUpdateScrollBar = true;			}
	void EnsureTitlebar( bool bShow);
	virtual void InitMouseState() override;



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL);
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;
	virtual int OnNcHitTest( MEvent* pEvent) override;



/// LUA script
public:
	void		glueAddString( const char* szString)	{ AddString( szString);					}
	void		glueResetContent()						{ ResetContent();						}



#define MINT_TEXTFRAME	"TextFrame"
	virtual const char* GetClassName() override			{ return MINT_TEXTFRAME;				}
};


} // namespace mint3
