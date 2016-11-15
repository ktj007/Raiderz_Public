#include "stdafx.h"
#include "GModule.h"
#include "crtdbg.h"
#include "GEntitySync.h"

GModule::GModule(GEntity* pOwner) : m_pContainer(NULL), m_pOwner(pOwner)
{

}

GModule::~GModule()
{

}

void GModule::Active(bool bActive)
{
	m_pContainer->ActiveModule(GetID(), bActive);
}

void GModule::Subscribe(int nMessageID)
{
	m_pContainer->SubscribeMessage(GetID(), nMessageID);
}

bool GModule::IsActivate()
{
	return (m_pContainer->IsActiveModule(GetID()));
}

MUID GModule::GetOwnerUID()
{ 
	if (!GetOwner())
		return MUID::Invalid();

	return GetOwner()->GetUID();
}

//////////////////////////////////////////////
//// GModuleContainer
GModuleContainer::GModuleContainer()
{


}

GModuleContainer::~GModuleContainer()
{

}

GModule *GModuleContainer::GetModule(int nID)
{
	GModuleMap::iterator i = m_Modules.find(nID);

	if(m_Modules.end()!=i) {
		return i->second;
	}

	return NULL;
}
 
void GModuleContainer::AddModule(GModule *pModule,bool bInitialActive)
{
	GModuleMap::iterator i = m_Modules.find(pModule->GetID());
	if(i!=m_Modules.end()) {
		_ASSERT(0);	// 중복되었다
	}

	m_Modules.insert(GModuleMap::value_type(pModule->GetID(),pModule));
	if(bInitialActive)
		ActiveModule(pModule->GetID());

	pModule->m_pContainer = this;
	pModule->OnSubscribeMessage();

	pModule->OnAdd();
}

void GModuleContainer::RemoveModule(GModule *pModule)
{
	if (pModule == NULL) return;		// 널값이면 리턴
	GModuleMap::iterator i = m_Modules.find(pModule->GetID());
	if(i==m_Modules.end()) return;	// 없는 모듈이다

	ActiveModule(pModule->GetID(),false);

	m_Modules.erase(i);
	pModule->OnRemove();
}

void GModuleContainer::ActiveModule(int nID, bool bActive)	// 특정 모듈을 (비)활성화 시킨다
{
	// 활성화 시킨다
	if(bActive) {
		if(IsActiveModule(nID)) return;	// 이미 활성화 되어있다

		GModuleMap::iterator i = m_Modules.find(nID);
		if(i!= m_Modules.end()) {
			i->second->OnActivate();
			m_ActiveModules.push_back(i->second);
		}
	}else{
	// 비활성화 시킨다
		for (vector<GModule*>::iterator itor = m_ActiveModules.begin(); itor != m_ActiveModules.end(); ++itor)
		{
			(*itor)->OnDeactivate();
			if ((*itor)->GetID() == nID)
			{
				m_ActiveModules.erase(itor);
				return;
			}
		}
	}
}

bool GModuleContainer::IsActiveModule(int nID)
{
	for (int i = 0; i < (int)m_ActiveModules.size(); i++)
	{
		if (m_ActiveModules[i]->GetID() == nID)
		{
			return true;
		}
	}

	return false;
}



void GModuleContainer::SubscribeMessage(int nModuleID, int nMessageID)
{
	m_ModuleMessageTypes[nModuleID].insert(nMessageID);
}

void GModuleContainer::Message(GMessage& msg)
{
	int nSize = (int)m_ActiveModules.size();
	for (int i = 0; i < nSize; i++)
	{
		GModule* pModule = m_ActiveModules[i];
		
		if (m_ModuleMessageTypes[pModule->GetID()].find(msg.m_nID) != m_ModuleMessageTypes[pModule->GetID()].end())
		{
			pModule->Message(msg);
		}
	}
}

