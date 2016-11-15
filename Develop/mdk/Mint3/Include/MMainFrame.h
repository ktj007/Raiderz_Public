#ifndef _MMAIN_FRAME_H
#define _MMAIN_FRAME_H


#include "MWidget.h"
#include "MFrame.h"

namespace mint3 {

class MLabel;
class MComboBox;
class MEdit;
class MButton;
class MPopupMenu;

class MNewWidgetDlg : public MFrame
{
protected:
	MLabel*		m_pLabelWidgetType;
	MComboBox*	m_pWidgetType;
	MLabel*		m_pLabelWidgetName;
	MEdit*		m_pWidgetName;
	MLabel*		m_pLabelWidgetID;
	MEdit*		m_pWidgetID;
	MButton*	m_pOK;
	MButton*	m_pCancel;
	int			m_nMode;

public:
	MWidget*	m_pTarget;
	MPOINT		m_TargetPos;
protected:
	virtual void OnSize(int w, int h);
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage);
	virtual void OnShow(void)
	{

	}
public:
	MNewWidgetDlg(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MNewWidgetDlg(void);

	void Set(const char* szName, const char* szID);
	void Set(const char* szClass, const char* szName, int nID);
	void SetMode(int m);
};

#define DM_ADDWIDGET	"Add Widget"
#define DM_REMOVEWIDGET	"Remove Widget"
#define DM_PROPERTY		"Property"


class MMainFrame : public MWidget{
protected:
	MWidget*		m_pDesignerMenuCaller;
	MNewWidgetDlg*	m_pNewWidget;
	MPopupMenu*		m_pDesignerMenu;
protected:
	virtual void OnDraw(MDrawContext* pDC){
		// Draw Nothing
	}
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
public:
	MMainFrame(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MMainFrame(void){
	}
	void ShowDesignerMenu(MPOINT& p, MWidget* pCaller);
	void HideDesignerMode(void);
};














}



#endif // _MMAIN_FRAME_H