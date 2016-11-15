#include "stdafx.h"
#include "MPaint.h"

namespace mint3 {


MPaint::MPaint(const char* szName, MWidget* pParent, MListener* pListener) : MWidget(szName, pParent, pListener)
{

}


void MPaint::OnDraw(MDrawContext* pDC)
 {
	m_EventHandler.OnCommand(this, MEVENT_DRAW);
}

} // namespace mint3