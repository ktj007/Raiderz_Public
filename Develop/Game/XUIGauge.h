#pragma once

#include "MWidget.h"
#include "XUIGauge.h"



class XUIGauge;



/// class : XGaugeLook
class XGaugeLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pSmallFont;
	int			m_nTextViewStyle;					///< text view style



/// Member functions
public:
	XGaugeLook();

	void SetSmallFont( const char* szFont);
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetTextViewStyle( int nStyle)				{ m_nTextViewStyle = nStyle;							}
	int GetTextViewStyle() const					{ return m_nTextViewStyle;								}


	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawGauge( XUIGauge* pGauge, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawText( XUIGauge* pGauge, MRECT& r, MDrawContext* pDC);
};






/// class : XUIGauge
class XUIGauge : public MWidget
{
/// Member variables
protected:
	int			m_nLower, m_nUpper;
	int			m_nBeginPos, m_nEndPos;
	bool		m_bEnableSliding;
	MTimer		m_tSliding;




/// Member functions
public:
	XUIGauge( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	void SetRange( int nLower, int nUpper);
	void GetRange( int& nLower, int& nUpper) const;

	int SetPos( int nPos);
	int GetPos() const;
	int GetEndPos() const;

	void EnableSliding( bool bEnable);



/// LUA script
public:
	void glueSetRange( int nLower, int nUpper)		{ SetRange( nLower, nUpper);					}
	MWLua::ret_int2 glueGetRange()					{ return MWLua::ret_int2( m_nLower, m_nUpper);	}
	int glueSetPos( int nPos)						{ return SetPos( nPos);							}
	int glueGetPos() const							{ return GetPos();								}
	void glueEnableSliding( bool bEnable)			{ EnableSliding( bEnable);						}



#define MINT_GAUGE		"Gauge"
	virtual const char* GetClassName()				{ return MINT_GAUGE;							}
};
