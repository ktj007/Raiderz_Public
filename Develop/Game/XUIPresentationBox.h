#pragma once

#include "MWidget.h"
#include "MTextView.h"


using namespace std;
using namespace mint3;


enum XPRESENTATIONICON_TYPE
{
	PRESENTATION_ICON_NONE			= 0,
	PRESENTATION_ICON_EXCLAMATION	= 1,
	PRESENTATION_ICON_INFORMATION	= 2,
	PRESENTATION_ICON_QUESTION		= 3,
	PRESENTATION_ICON_STOP			= 4
};

// XPresentChild
class XPresentChild : public MTextView
{
protected:
	MTimer	m_tFade;
	bool	m_bNotFadeIn;
	DWORD	m_nLifeTime;
public:
	XPresentChild( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual void SetText( const char* szText) override;
	void SetFadeTime( DWORD dwTime, bool bNotFadeIn=false);
	void SetLifeTime(DWORD dwLifeTime);
	void Disappear();

protected:
	virtual void OnDraw( MDrawContext* pDC) override;
};







/// class : XUIPresentationBox
class XUIPresentationBox : public MWidget
{
public:
	struct _CHILD
	{
		XPresentChild*	m_pTextView;

		_CHILD( XPresentChild* pTextView)	{ m_pTextView = pTextView;		}
		void _delete()						{ delete m_pTextView;			}
	};



protected:
	list< _CHILD>		m_Children;
	MLookBase*			m_pTextLook;
	DWORD				m_dwFadeTime;
	MTimer				m_tMove;
	bool				m_bArrangeChildren;


public:
	XUIPresentationBox( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIPresentationBox();


	void Add( const char* szString, int nIcon);
	void Add( const char* szString, int nIcon, float fLifeTime);
	void Clear();

	void SetFadeSecond( float fSec)						{ m_dwFadeTime = (DWORD)( fSec * 1000);		}
	float GetFadeSecond()								{ return ( (float)m_dwFadeTime / 1000.0);	}

	void SetTextLook( MLookBase* pLook)					{ m_pTextLook = pLook;						}

protected:
	void ArrangeChildren()								{ m_bArrangeChildren = true;				}



protected:
	virtual MRESULT OnPreTranslateMsg( MEvent* pEvent) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;



public:
	void glueAdd( const char* szString, int nIcon)		{ Add( szString, nIcon);					}
	void glueAddAndSetLifeTime( const char* szString, int nIcon, float fLifeTime)		{ Add( szString, nIcon, fLifeTime);					}
	void glueClear()									{ Clear();									}



#define MINT_PRESENTATIONBOX		"PresentationBox"
	virtual const char* GetClassName() override			{ return MINT_PRESENTATIONBOX;				}
};