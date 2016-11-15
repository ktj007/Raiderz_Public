#include "stdafx.h"
#include "MEventHandler.h"
#include "MWidget.h"
#include "Mint.h"
#include "MScriptManager.h"

namespace mint3 {

static string strEventNames[MEVENT_MAX] =
{
	"MEVENT_UNKNOWN",
	"OnShow",
	"OnHide",
	"OnClick",
	"OnRClick",
	"OnEnter",
	"OnLeave",
	"OnDraw",
	"OnChar",
	"OnDoubleClick",
	"OnKeyDown",
	"OnKeyUp",
	"OnMouseDown",
	"OnMouseUp",
	"OnTick",
	"OnTabSelected",
	"OnCheckChanged",
	"OnDrop",
	"OnMouseWheel",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"OnSelectedIndexChanged",
	"",
	"",
	"OnItemSelected",
	"",
	"",
	"OnItemSelLost",
	"",
	"",
	"",
	"",
	"",
	"OnLinkedText"
};

MEventMsg FindEventMsgFromName(const char* szEventName)
{
	for (int i = 0; i < MEVENT_MAX; i++)
	{
		if (!_stricmp(szEventName, strEventNames[i].c_str())) return MEventMsg(i);
	}
	return MEVENT_UNKNOWN;
}

bool MEventHandler::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if (m_pListener != NULL)
	{
		if (m_pListener->OnCommand(pWidget, nMsg, szArgs) == true) return true;
	}

	if (pWidget->OnCommand(pWidget, nMsg, szArgs) == true) return true;

	if ( nMsg != MEVENT_KEY_UP)
		ExistScriptEvent(nMsg);

	if (ExistScriptEvent(nMsg))
	{
		DoScriptEvent(pWidget, nMsg, szArgs);
		return true;
	}
	return false;
}


bool MEventHandler::ExistScriptEvent(MEventMsg nMsg)
{
	if (m_ScriptEvents.empty()) return false;

	set<MEventMsg>::iterator itor = m_ScriptEvents.find(nMsg);
	if (itor != m_ScriptEvents.end()) return true;

	return false;
}

void MEventHandler::DoScriptEvent(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	Mint::GetInstance()->GetScriptManager()->RunScriptWidgetEvent(pWidget, strEventNames[nMsg].c_str());
}

} // namespace mint3