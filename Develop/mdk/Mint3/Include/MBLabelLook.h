#pragma once

#include "MLabel.h"

namespace mint3 {

class MBLabelLook : public MLabelLook{
public:
	MFont*		m_pFont;
	MCOLOR		m_FontColor;

protected:
	virtual void OnDraw(MLabel* pLabel, MDrawContext* pDC);

public:
	MBLabelLook(void);

	virtual MRECT GetClientRect(MLabel* pLabel, MRECT& r);
};

} // namespace mint3

