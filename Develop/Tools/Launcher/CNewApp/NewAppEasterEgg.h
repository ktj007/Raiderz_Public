
#include "resource.h"
#include "CDDBDialog.h"
#include "CSound.h"



/////////////////////////////////////////////////////////////////////////////
// Dialog
class CNewAppEasterEgg : public CDDBDialog
{
private:
	//{{AFX_DATA( CNewAppEasterEgg)
	CStatic			m_cMessage;
	CButton			m_cCheck;
	CDDBButton		m_cOK;
	CDDBButton		m_cCancel;
	//}}AFX_DATA

	// Variables
	CString			m_strCaption;
	CString			m_strMessage;
	UINT			m_nButtonType;
	CSound			m_Sound;

	CDialog*		m_pParentDialog;


public:
	CNewAppEasterEgg(CWnd* pParent = NULL);

	void SetDialog( CDialog* pDialog, CString strCaption, CString strMessage, UINT nButtonType = MB_OKCANCEL);

	//{{AFX_VIRTUAL( CNewAppEasterEgg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNewAppEasterEgg)
	afx_msg void OnOK();
	//}}AFX_MSG

protected:
	DECLARE_MESSAGE_MAP()
};
