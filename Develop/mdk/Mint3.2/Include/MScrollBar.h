#pragma once

#include "MWidget.h"
#include "MLookBase.h"


// namespace : mint3
namespace mint3
{

class MWidget;
class MScrollBar;



// ScrollBar style
#define MSBS_HORZ					0x0000L
#define MSBS_VERT					0x0001L
#define MSBS_TOPALIGN				0x0002L
#define MSBS_LEFTALIGN				0x0002L
#define MSBS_BOTTOMALIGN			0x0004L
#define MSBS_RIGHTALIGN				0x0004L
#define MSBS_ALWAYSSHOW				0x0008L
#define MSBS_ALWAYSHIDE				0x0010L


// Scrollbar parts
enum MSCROLLBAR_PARTS
{
	MSBP_NONE			= -1,
	MSBP_UPARROW		= 0,
	MSBP_DOWNARROW,
	MSBP_LEFTARROW,
	MSBP_RIGHTARROW,
	MSBP_THUMB,
	MSBP_TRACK,
	MSBP_MAX
};





/// class : MScrollBarLook
class MScrollBarLook : public MLookBase
{
/// Member variables
protected:
	int			m_nWidth;


/// Member functions
public:
	MScrollBarLook();

	void SetWidth( int width)				{ m_nWidth = width;				}
	int GetWidth()							{ return m_nWidth;				}

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawUpArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawDownArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawLeftArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawRightArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawThumb( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawTrack( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC);
};






/// class : MScrollBar
class MScrollBar : public MWidget
{
/// Member variables
protected:
	int					m_nStyle;
	int					m_nValue;
	int					m_nRangeMin, m_nRangeMax;
	int					m_nPushedRgn;
	int					m_nThumbLength;
	MPOINT				m_ptCursor;
	DWORD				m_dwTimer;


/// Member functions
public:
	MScrollBar( const char* szName, MWidget* pParent =NULL, MListener* pListener =NULL, int nStyle =MSBS_VERT);

	const char* GetButtonState( int button);
	int GetButtonStateNum( int button);

	void SetStyle( int style)						{ m_nStyle = style;												}
	int GetStyle() const							{ return m_nStyle;												}

	int SetScrollValue( int nValue);
	int GetScrollValue() const						{ return m_nValue;												}

	void SetScrollRange( int min, int max);
	void GetScrollRange( int* min, int* max);
	int GetMin() const								{ return m_nRangeMin;											}
	int GetMax() const								{ return m_nRangeMax;											}

	void SetAlwaysShow( bool bShow);
	bool IsAlwaysShow() const						{ return ((m_nStyle & MSBS_ALWAYSSHOW) ? true : false);			}

	void SetAlwaysHide( bool bShow);
	bool IsAlwaysHide() const						{ return ((m_nStyle & MSBS_ALWAYSHIDE) ? true : false);			}

	bool IsHorz() const								{ return (((m_nStyle & 0x00001L) == MSBS_HORZ) ? true : false);	}
	bool IsVert() const								{ return (((m_nStyle & 0x00001L) == MSBS_VERT) ? true : false);	}
	bool IsAligned() const							{ return ((m_nStyle & 0x00006L) ? true : false);				}

	MRECT GetLeftArrowRect();
	MRECT GetRightArrowRect();
	MRECT GetUpArrowRect();
	MRECT GetDownArrowRect();
	MRECT GetThumbRect();
	MRECT GetTrackRect();

	int GetWidth()									{ return (HasLookAndFill() ? ((MScrollBarLook*)m_pLookAndFill)->GetWidth() : 15);				}

	void SetThumbLength( int length)				{ m_nThumbLength = length;										}
	int GetThumbLength();

	virtual bool IsFocus() override;

	int GetPushedRgn() const						{ return m_nPushedRgn;											}

	virtual bool IsShowArrow() const				{ return true;													}



protected:
	void SetScrollValueFromCursor( MPOINT point);

	virtual void CalculateAlignRect();



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual bool OnSetCursor( MEvent* pEvent, string* pCursor) override;




/// LUA script
public:
	MWLua::ret_int2 glueSetScrollRange( int _min, int _max);
	MWLua::ret_int2 glueGetScrollRange() const		{ return MWLua::ret_int2( m_nRangeMin, m_nRangeMax);			}
	int glueSetScrollValue( int _val)				{ return SetScrollValue( _val);									}
	int glueGetScrollValue() const					{ return GetScrollValue();										}
	void glueSetThumbLength( int _val)				{ SetThumbLength( _val);										}
	int glueGetThumbLength()						{ return GetThumbLength();										}
	bool glueIsHorz() const							{ return IsHorz();												}
	bool glueIsVert() const							{ return IsVert();												}
	bool glueIsAligned() const						{ return IsAligned();											}




#define MINT_SCROLLBAR	"ScrollBar"
	virtual const char* GetClassName() override		{ return MINT_SCROLLBAR;										}
};



} // namespace mint3