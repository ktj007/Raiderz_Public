#include "stdafx.h"
#include "GJobT_Script.h"
#include "GModuleAI.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GNullGlueNPC.h"
#include "GNullGlueActor.h"
#include "GScriptUtil.h"

GJobT_Script::GJobT_Script(GModuleAI* pmAI, MUID uidOpponent, wstring strTable, wstring strFunc) 
: GInstantJob(pmAI)
, m_uidOpponent(uidOpponent)
, m_strTable(strTable)
, m_strFunc(strFunc)
{
}

GJobT_Script::~GJobT_Script()
{

}

bool GJobT_Script::OnStart()
{
	RVALID_RET(GetNPC(), false);
	RVALID_RET(GetNPC()->GetField(), false);
	RVALID_RET(GetOwnerNPC(), false);
	
	GEntityActor* pOpponent = NULL;
	if (m_uidOpponent.IsValid())
	{
		pOpponent = GetOwnerNPC()->FindActor(m_uidOpponent);
	}
	
	
	if (m_strTable.empty())
	{
		WCALL2(WLUA, MLocale::ConvUTF16ToAnsi(m_strFunc.c_str()).c_str(), 
			TO_SAFE_LUAPTR(GetOwnerNPC()), 
			TO_SAFE_LUAPTR(pOpponent), WNULL);	
	}
	else
	{
		WMEMCALL2(WLUA, MLocale::ConvUTF16ToAnsi(m_strTable).c_str(), MLocale::ConvUTF16ToAnsi(m_strFunc).c_str(), 
			TO_SAFE_LUAPTR(GetOwnerNPC()), 
			TO_SAFE_LUAPTR(pOpponent), WNULL);	
	}

	

	return true;
}

string GJobT_Script::GetParamString() const
{
	string strRet;
	if (!m_strTable.empty())
	{
		strRet += "Table: ";
		strRet += MLocale::ConvUTF16ToAnsi(m_strTable);
	}

	if (!m_strFunc.empty())
	{
		if (!strRet.empty())
			strRet += ", ";

		strRet += "Func: ";
		strRet += MLocale::ConvUTF16ToAnsi(m_strFunc);
	}

	if (m_uidOpponent.IsInvalid())
	{
		if (!strRet.empty())
			strRet += ", ";

		strRet += "HasOpponent: true";
	}

	return strRet;
}
