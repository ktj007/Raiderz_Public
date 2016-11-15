#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{

class MProgressCtrl;




// ProgressCtrl text view style
enum MPROGRESSCTRL_TEXTSTYLE
{
	MPCTS_NONE		= 0,
	MPCTS_PERCENT,
	MPCTS_PROGRESS
};




/// class : MProgressCtrlLook
class MProgressCtrlLook : public MLookBase
{
/// Member variables
protected:
	int		m_nTextViewStyle;					///< text view style



/// Member functions
public:
	MProgressCtrlLook();

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;

	void SetTextViewStyle( int nStyle)					{ m_nTextViewStyle = nStyle;						}
	int GetTextViewStyle() const						{ return m_nTextViewStyle;							}



protected:
	virtual void OnDrawGauge( MProgressCtrl* pProgressCtrl, MRECT& r, MDrawContext* pDC);
	virtual void OnDrawText( MProgressCtrl* pProgressCtrl, MRECT& r, MDrawContext* pDC);
};






/// class : MProgressCtrl
class MProgressCtrl : public MWidget
{
/// Member variables
protected:
	int			m_nLower, m_nUpper;
	int			m_nBeginPos, m_nEndPos;
	bool		m_bEnableSliding;
	MTimer		m_tSliding;



/// Member functions
public:
	MProgressCtrl( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	void SetRange( int nLower, int nUpper);
	void GetRange( int& nLower, int& nUpper) const;

	int SetPos( int nPos);
	int GetPos() const;

	void EnableSliding( bool bEnable);



/// LUA script
public:
	void glueSetRange( int nLower, int nUpper)		{ SetRange( nLower, nUpper);					}
	MWLua::ret_int2 glueGetRange()					{ return MWLua::ret_int2( m_nLower, m_nUpper);	}
	int glueSetPos( int nPos)						{ return SetPos( nPos);							}
	int glueGetPos() const							{ return GetPos();								}
	void glueEnableSliding( bool bEnable)			{ EnableSliding( bEnable);						}



#define MINT_PROGRESSCTRL	"ProgressCtrl"
	virtual const char* GetClassName()				{ return MINT_PROGRESSCTRL;						}
};


} // namespace mint3