#include "stdafx.h"
#include "MGroup.h"
#include "MColorTable.h"

namespace mint3 {

void MGroupLook::OnDraw(MGroup* pGroup, MDrawContext* pDC)
{
	MRECT r = pGroup->GetInitialClientRect();
	pDC->SetColor(MColorTable::FRAME_PLANE);
	pDC->FillRectangle(r);
	pDC->SetColor(MColorTable::FRAME_OUTLINE);
	pDC->Rectangle(r);

	pDC->SetColor(MColorTable::FRAME_TEXT);
	pDC->Text(r.x+2, r.y+2, pGroup->GetText());
}

MRECT MGroupLook::GetClientRect(MGroup* pGroup, MRECT& r)
{
	return r;
}


IMPLEMENT_LOOK(MGroup, MGroupLook)

MGroup::MGroup(const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()
}

MGroup::~MGroup(void)
{
}


} // namespace mint3