#pragma once

#include "MWidget.h"
#include "MPopupBox.h"
#include "MTextView.h"
#include "MLookNFeel.h"



namespace mint3 {



// class MTooltipLook
class MTooltipLook : public MPopupBoxLook
{
public:
	MTooltipLook();

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC);
};






// class MTooltip
class MTooltip : public MWidget
{
protected:
	MTextView*		m_pTextView;
	MWidget*		m_pSender;
	int				m_nShowState;
	DWORD			m_dwTimer;
	MPOINT			m_ptPosition;


public:
	MTooltip( MWidget* pParent);
	virtual ~MTooltip();

	void SetTooltip( MWidget* pSender, MPOINT& pos, const string& strMsg);


protected:
	void SetTextView();


protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual void OnRun( void);



	// Declare look
protected:
	DECLARE_LOOK( MTooltipLook)
	DECLARE_LOOK_CLIENT()


#define MINT_TOOLTIP	"ToolTip"
	virtual const char* GetClassName(void){ return MINT_TOOLTIP; }
};


} // namespace mint3