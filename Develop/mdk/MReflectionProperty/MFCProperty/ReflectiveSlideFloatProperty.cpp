#include "StdAfx.h"
#include "ReflectiveSlideFloatProperty.h"
#include "ReflectiveSlideCtrl.h"


float	CReflectiveSlideFloatProperty::FLOAT_TO_POS_MULTIPLIER = 1000.0f;

CReflectiveSlideFloatProperty::CReflectiveSlideFloatProperty( const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,float min,float max )
:CMFCPropertyGridProperty(strName,varValue,lpszDescr,dwData)
{
	m_min = min;
	m_max = max;
}

CReflectiveSlideFloatProperty::~CReflectiveSlideFloatProperty(void)
{
}


CWnd* CReflectiveSlideFloatProperty::CreateInPlaceEdit( CRect rectEdit,BOOL& bDefaultFormat )
{
	rectEdit.left = (rectEdit.left + rectEdit.right) / 2;
	CReflectiveSlideCtrl *pSlideCtrl = new CReflectiveSlideCtrl(this);
	pSlideCtrl->Create(TBS_AUTOTICKS | TBS_HORZ, rectEdit, m_pWndList, AFX_PROPLIST_ID_INPLACE);
	pSlideCtrl->ShowWindow(SW_SHOW);

	pSlideCtrl->SetRange(FloatToPos(m_min),FloatToPos(m_max),true);

	variant_t variant;
	variant = GetValue();
	float f = variant;
	pSlideCtrl->SetPos(FloatToPos(f));


	return pSlideCtrl;
}

BOOL CReflectiveSlideFloatProperty::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);

	CReflectiveSlideCtrl* pSlideCtrl = (CReflectiveSlideCtrl*)m_pWndInPlace;
	int min = pSlideCtrl->GetRangeMin();
	int max = pSlideCtrl->GetRangeMax();
	int pos = pSlideCtrl->GetPos();
	float f = PosToFloat(pos);
	variant_t variant = f;

	SetValue(variant);

	m_pWndList->OnPropertyChanged(this);

	return TRUE;
}

void CReflectiveSlideFloatProperty::SetValueDirectly( const COleVariant& varValue )
{
	m_varValue = varValue;
	m_pWndList->OnPropertyChanged(this);
}

float CReflectiveSlideFloatProperty::PosToFloat( int pos )
{
	return pos / FLOAT_TO_POS_MULTIPLIER;
}

int CReflectiveSlideFloatProperty::FloatToPos( float f )
{
	return (int)(f * FLOAT_TO_POS_MULTIPLIER);
}



