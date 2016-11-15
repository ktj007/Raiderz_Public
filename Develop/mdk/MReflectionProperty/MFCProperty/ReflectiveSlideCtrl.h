#pragma once

#include "ReflectiveSlideFloatProperty.h"

class CReflectiveSlideCtrl : public CSliderCtrl
{
public:
	CReflectiveSlideCtrl(CReflectiveSlideFloatProperty* pReflectiveSlideFloatProperty);
	virtual ~CReflectiveSlideCtrl(void);

protected:
	void	UpdatePosToProperty(void);
	CReflectiveSlideFloatProperty*	m_pReflectiveSlideFloatProperty;
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
