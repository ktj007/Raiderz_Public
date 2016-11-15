#include "stdafx.h"
#include "XGameStateSwitchResourceMgr.h"
#include "XCharacter.h"
#include "XEngine.h"

XGameStateSwitchResourceMgr::XGameStateSwitchResourceMgr()
{

}

XGameStateSwitchResourceMgr::~XGameStateSwitchResourceMgr()
{

}

void XGameStateSwitchResourceMgr::AddObjectResourceReferenCount()
{
	m_vecResourceBackUp.clear();

	for(XObjectMap::iterator itObject = gg.omgr->BeginItor(); itObject != gg.omgr->EndItor(); itObject++)
	{
		XObject* pObj = itObject->second;

		if(pObj->GetActor() &&
			pObj->GetActor()->m_pMesh)
		{
			wstring strMeshName = MLocale::ConvAnsiToUTF16(pObj->GetActor()->m_pMesh->GetName().c_str());
			RMesh* pMesh = global.engine->CreateMesh(strMeshName.c_str());

			if(pMesh != NULL)
			{
				stResourceBackUp backUp;
				backUp.pMesh		= pMesh;
				backUp.strMeshName	= strMeshName;
				backUp.strModelName = MLocale::ConvAnsiToUTF16(pObj->GetActor()->GetMeshName().c_str());
				m_vecResourceBackUp.push_back(backUp);
			}
		}
	}
}

void XGameStateSwitchResourceMgr::SubObjectResourceReferenCount()
{
	for(vector<stResourceBackUp>::iterator itResource = m_vecResourceBackUp.begin(); itResource != m_vecResourceBackUp.end(); itResource++)
	{
		global.engine->ReleaseMesh((*itResource).pMesh);
	}

	m_vecResourceBackUp.clear();
}

wstring XGameStateSwitchResourceMgr::GetObjectResourceName( const wstring& strModelName )
{
	wstring strMeshName;
	for(vector<stResourceBackUp>::iterator itResource = m_vecResourceBackUp.begin(); itResource != m_vecResourceBackUp.end(); itResource++)
	{
		if((*itResource).strModelName == strModelName)
			strMeshName = (*itResource).strMeshName;
	}

	return strMeshName;
}