#include "StdAfx.h"
#include "ReflectiveSlideCtrl.h"

BEGIN_MESSAGE_MAP(CReflectiveSlideCtrl, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CReflectiveSlideCtrl::CReflectiveSlideCtrl( CReflectiveSlideFloatProperty* pReflectiveSlideFloatProperty )
{
	m_pReflectiveSlideFloatProperty = pReflectiveSlideFloatProperty;
}

CReflectiveSlideCtrl::~CReflectiveSlideCtrl(void)
{
}

void CReflectiveSlideCtrl::UpdatePosToProperty(void)
{
	int min = GetRangeMin();
	int max = GetRangeMax();
	int pos = GetPos();
	float f = m_pReflectiveSlideFloatProperty->PosToFloat(pos);
	variant_t variant = f;
	SetPos(m_pReflectiveSlideFloatProperty->FloatToPos(f));
	m_pReflectiveSlideFloatProperty->SetValueDirectly(variant);
	m_pReflectiveSlideFloatProperty->Redraw();
}

void CReflectiveSlideCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	UpdatePosToProperty();

	CSliderCtrl::OnMouseMove(nFlags, point);
}
