#pragma once

#include "MTabCtrl.h"

namespace mint3 {

class MBTabCtrlLook : public MTabCtrlLook {
public:
	MBitmap*	m_pFrameBitmaps[9];

public:
	MBTabCtrlLook(void);

//	virtual void OnFrameDraw(MTabCtrl* pTabCtrl, MDrawContext* pDC);
	virtual MRECT GetClientRect(MTabCtrl* pTabCtrl, MRECT& r);

protected:
	virtual void	OnDraw(MTabCtrl* pTabCtrl, MDrawContext* pDC);
};

} // namespace mint3