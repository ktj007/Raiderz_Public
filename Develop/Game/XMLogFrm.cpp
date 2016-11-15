#include "StdAfx.h"
#include "XMLogFrm.h"

#define FRAME_CONSOLE_X			10
#define FRAME_CONSOLE_Y			10
#define FRAME_CONSOLE_WIDTH		400
#define FRAME_CONSOLE_HEIGHT	400

XMLogFrm::XMLogFrm(const char* szName, MWidget* pParent, MListener* pListener)
: mint3::MTextFrame(szName, pParent, pListener)
{
	SetRect(MRECT(FRAME_CONSOLE_X, FRAME_CONSOLE_Y, FRAME_CONSOLE_WIDTH, FRAME_CONSOLE_HEIGHT));
	SetOpacity(0.9f);
	SetText("MLog");
	UpdateScrollBar();
}