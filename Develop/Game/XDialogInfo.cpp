#include "StdAfx.h"
#include "XDialogInfo.h"
#include "XMMCode.h"
#include "XStrings.h"
#include "XInteractionInfo.h"
#include "XDialogTextInfoMgr.h"

const wchar_t* XDialogInfo::GetSayString()
{
	MUID uidMine = XGetMyUID();
	MUID uidInteractionNPC = XGetInteractionInfo().InteractionTargetUID;
	
	mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(info.dialog_text->Get(m_nSayTextID), uidMine);
	result.Proc(&uidInteractionNPC);
	
	m_strSay = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());
	return m_strSay.c_str();
}

MWLua::table XDialogInfo::GetDialogSelectTable()
{
	MWLua::table t(global.script->GetLua()->GetState());
	if( m_vecDialogSelectNode.size() == 0) return t;

	int nIndex = 5 - m_vecDialogSelectNode.size()+1;

	char buffer[32];
	int i = 1;
	for (; i < nIndex; i++)
	{
		sprintf(buffer,"%d", i);
		t.set(buffer, "");
	}

	vector<TD_DIALOG_SELECT_NODE>::iterator itor = m_vecDialogSelectNode.begin();
	for (; itor != m_vecDialogSelectNode.end(); itor++, i++)
	{
		wstring strStringBuffer;
		XStringResMgr::GetInstance()->TransKeyStr(strStringBuffer, info.dialog_text->Get((*itor).nSelectID));

		sprintf(buffer, "%d", i);
		t.set(buffer, MLocale::ConvUTF16ToAnsi(strStringBuffer.c_str()).c_str());
	}

	return t;
}

void XDialogInfo::Clear()
{
	m_nSayTextID = 0;
	m_vecDialogSelectNode.clear();
}

XDialogInfo::XDialogInfo()
: m_nSayTextID(0)
{

}