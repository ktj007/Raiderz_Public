#include "StdAfx.h"
#include "XNPCInteractionInfo.h"
#include "XMMCode.h"
#include "XStrings.h"
#include "XInteractionInfo.h"
#include "MLocale.h"

MWLua::table XNPCInteractionInfo::GetInteractionNodeTable()
{
	MWLua::table t(global.script->GetLua()->GetState());
	if( m_vecInteractionElement.size() == 0) return t;
	
	char buffer[32];

	int i =1;
	vector<TD_INTERACTION_ELEMENT>::iterator itor = m_vecInteractionElement.begin();
	for (; itor != m_vecInteractionElement.end(); itor++, i++)
	{
		wstring strStringBuffer;
		XStringResMgr::GetInstance()->TransKeyStr(strStringBuffer, (*itor).szIEText);

		sprintf(buffer, "%d", i);
		t.set(buffer, MLocale::ConvUTF16ToAnsi(strStringBuffer.c_str()).c_str());
	}

	return t;
}

const wchar_t* XNPCInteractionInfo::GetOpeningString()
{
	MUID uidMine = XGetMyUID();
	MUID uidInteractionNPC = XGetInteractionInfo().InteractionTargetUID;
	
	mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(m_strOpeningTextID.c_str(), uidMine);
	result.Proc(&uidInteractionNPC);
	
	m_strOpeningText = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());

	return m_strOpeningText.c_str();
}

void XNPCInteractionInfo::Clear()
{
	m_MenuType = MENU_NONE;
	m_strOpeningTextID.clear();
	m_vecInteractionElement.clear();
	m_strOpeningText.clear();
}
