#pragma once

/////////////////////////////////////////////////////////////////////////////
// CReflectiveColorPropertyCtrl object

class CTabCustom;

class CReflectiveColorPropertyCtrl : public CMFCPropertyGridColorProperty
{
	DECLARE_DYNAMIC(CReflectiveColorPropertyCtrl)

	// Construction
public:
	CReflectiveColorPropertyCtrl(const CString& strName, const COLORREF& color, CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CReflectiveColorPropertyCtrl();

	// Overrides
public:
	void OnClickButton(CPoint point) override;
	void OnKillSelection(CMFCPropertyGridProperty* /*pNewSel*/) override;

	CTabCustom*	m_pXColorDialog;	///< external color pick dialog
};