#include "stdafx.h"
#include "MSheet.h"
#include "Mint.h"

namespace mint3 {

bool MSheet::OnEvent(MEvent* pEvent, MListener* pListener)
{
	return MWidget::OnEvent(pEvent, pListener);
}

void MSheet::OnShow(void)
{
	MWidget::OnShow();
}

MSheet::MSheet(const char* szName, MWidget* pParent, MListener* pListener)
	: MWidget(szName, pParent, pListener), m_bDragAndDrop(false)
{
	SetAnchor( MAnchorStyle( true, true, true, true));
	SetBounds(MRECT(0, 0, MGetWorkspaceWidth(), MGetWorkspaceHeight()));
	SetFocusEnable(true);
}

MSheet::~MSheet(void)
{

}

bool MSheet::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	return false;
}

void MSheet::EnableDragAndDrop( bool bEnable )
{
	m_bDragAndDrop = bEnable;
}

} // namespace mint3