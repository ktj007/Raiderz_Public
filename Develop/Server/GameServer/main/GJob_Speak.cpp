#include "stdafx.h"
#include "GJob_Speak.h"
#include "GEntityNPC.h"

GJob_Speak::GJob_Speak(GModuleAI* pmAI, wstring strType, wstring strText, float fMaintainTime) 
: GJob(pmAI)
, m_strType(strType)
, m_strText(strText)
, m_fMaintainTime(fMaintainTime) 
{	
}

GJob_Speak::~GJob_Speak()
{

}

bool GJob_Speak::OnStart()
{
	SpeakImpl();
	
	if (m_fMaintainTime <= 0.0f)
	{
		return false;	// 대기시간이 없으면 즉시 종료
	}

	SetRunningTask(m_TaskBroker.New_Idle(GetOwnerNPC(), m_fMaintainTime));

	return true;
}

GJobResult GJob_Speak::OnRun( float fDelta )
{
	return JR_RUNNING;
}

void GJob_Speak::SpeakImpl()
{
	GEntityNPC* pOwnerNPC = GetOwnerNPC();
	if (m_strType == L"say")				pOwnerNPC->doSay(m_strText.c_str(), m_fMaintainTime);
	else if (m_strType == L"balloon")	pOwnerNPC->doBalloon(m_strText.c_str(), m_fMaintainTime);
	else if (m_strType == L"shout")		pOwnerNPC->doShout(m_strText.c_str(), m_fMaintainTime);
	else if (m_strType == L"narration")	pOwnerNPC->doNarration(m_strText.c_str());
	else if (m_strType == L"say_and_narration")	pOwnerNPC->doSayAndNarration(m_strText.c_str(), m_fMaintainTime);
}

string GJob_Speak::GetParamString() const
{
	char buff[512];
	sprintf(buff, "Type: %s, Text: %s, MaintainTime: %f", 
		MLocale::ConvUTF16ToAnsi(m_strType).c_str(),
		MLocale::ConvUTF16ToAnsi(m_strText).c_str(),
		m_fMaintainTime);
	return buff;
}
