#include "StdAfx.h"
#include "DereferenceTalentIDViewer.h"


System::Void Character_NET::DereferenceTalentIDViewer::DereferenceTalentEffectIDViewer_Load( System::Object^ sender, System::EventArgs^ e )
{
	if(m_nTalentID == 0 || m_eDerefType == DT_NONE)
		return;

	for(map<int , CSTalentInfo* >::iterator it = g_pMainApp->m_TTalentInfoMgr.begin(); it != g_pMainApp->m_TTalentInfoMgr.end(); ++it)
	{
		TTalentInfo* pCheckTalentInfo = (TTalentInfo*)(it->second);

		int nRefID = -1;
		
		if(m_eDerefType == DT_EFFECT)
			nRefID = pCheckTalentInfo->GetEffectDataMgr()->m_nRefID;
		else if(m_eDerefType == DT_EVENT)
			nRefID = pCheckTalentInfo->m_Events.m_nRefID;
		else if(m_eDerefType == DT_HIT)
			nRefID = pCheckTalentInfo->m_HitInfo.m_nReferenceID;

		if(nRefID == m_nTalentID)
		{
			String^ strID = gcnew String(String::Format("{0}", pCheckTalentInfo->m_nID));
			DereferenceTalentEffectID_listBox->Items->Add(strID);
		}
	}
}
