#include "StdAfx.h"
#include "ReflectiveFloatProperty.h"
#include "ReflectiveSpinCtrl.h"

CReflectiveFloatProperty::CReflectiveFloatProperty( const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData)
: CMFCPropertyGridProperty(strName,varValue,lpszDescr,dwData)
{
}

CReflectiveFloatProperty::~CReflectiveFloatProperty(void)
{
}

void CReflectiveFloatProperty::AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)
{
	__super::AdjustInPlaceEditRect(rectEdit, rectSpin);

	rectSpin = m_Rect;
	rectSpin.right = rectEdit.right;
	rectSpin.left = rectSpin.right - rectSpin.Height();
	rectSpin.top ++;
	rectEdit.right = rectSpin.left;
}

BOOL CReflectiveFloatProperty::OnEdit(LPPOINT lptClick)
{
	CRect rectEdit;
	CRect rectSpin;
	AdjustInPlaceEditRect(rectEdit, rectSpin);

	variant_t variant = GetValue();
	float f = variant;

	m_pWndSpin = new CReflectiveSpinCtrl(this);

	CWnd* pParent = CWnd::GetDesktopWindow();
	CRect rectSpinDesktop = rectSpin;
	m_pWndList->ClientToScreen( rectSpinDesktop );
	if (!m_pWndSpin->Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS, rectSpinDesktop, pParent, AFX_PROPLIST_ID_INPLACE))
	{
		return NULL;
	}

	return __super::OnEdit( lptClick );
}

// 깜빡거리지 않게 하기 위해 오버라이딩
void CReflectiveFloatProperty::SetValue(const COleVariant& varValue)
{
	m_varValue = varValue;

	if( m_pWndInPlace )
	{
		variant_t varValue = m_varValue;
		float fValue = varValue;
		CString strValue;
		strValue.Format("%f", fValue );
		m_pWndInPlace->SetWindowText(strValue);
	}

	m_pWndList->OnPropertyChanged(this);
}
