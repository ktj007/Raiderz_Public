#include "stdafx.h"

#include "MPxObjClothed.h"
#include "MPhysX.h"

namespace physx {

MPxObjClothed::MPxObjClothed()
{
	m_pClothedEntity = NULL;
}

MPxObjClothed::~MPxObjClothed()
{
	_ASSERT(m_pClothedEntity == NULL);

	ClearActionTables();
}

void MPxObjClothed::RegisterClothedEntity(ClothedEntity* pEntity)
{
	_ASSERT(pEntity);
	m_pClothedEntity = pEntity;
	m_pClothedEntity->m_pPxObjClothed = this;
}

void MPxObjClothed::UnRegisterClothedEntity()
{
	if (m_pClothedEntity)
	{
		m_pClothedEntity->m_pPxObjClothed = NULL;
		m_pClothedEntity = NULL;
	}
}

NxClothDesc* MPxObjClothed::CreateNxClothDesc( const std::string& _rFileName, const std::string& _rContextName )
{
	MPhysX* phx = MPhysX::GetInstance();
	_ASSERT(phx);

	if (!phx->IsCreatedPxInfoXML(_rFileName))
	{
		phx->CreatePxInfoXML(_rFileName, false);
	}

	MXmlElement* pXmlElement = phx->GetPxInfoXML<MPxObjClothed>(_rFileName, _rContextName);
	return NULL;
}

void MPxObjClothed::AddAction( float fDuration, ClothedAction* pAction )
{
	m_listActionTables.push_back( pair<float, ClothedAction*>(fDuration, pAction) );
}

void MPxObjClothed::UpdateAction()
{
	if (m_listActionTables.empty())
		return;

	std::list< pair<float, ClothedAction*> >::iterator itr = m_listActionTables.end();
	--itr;

	bool bLoop = true;
	while (bLoop)
	{
		if (itr == m_listActionTables.begin())
		{
			bLoop = false;
		}

		itr->first -= MPhysX::GetInstance()->GetCurrentElapsed();
		if (itr->first <= FLT_EPSILON)
		{
			itr->first = 0.f;
		}

		bool bRet = itr->second->OnAct(itr->first);
		if (bRet)
		{
			ClearActionTables();
			return;
		}

		if (itr->first <= FLT_EPSILON)
		{
			delete itr->second;
			itr = m_listActionTables.erase( itr );
		}

		if (m_listActionTables.empty())
			return;

		if (bLoop)
			--itr;
	}
}

void MPxObjClothed::ClearActionTables()
{
	for (std::list< pair<float, ClothedAction*> >::iterator itr = m_listActionTables.begin(); itr != m_listActionTables.end(); ++itr)
	{
		delete itr->second;
	}

	m_listActionTables.clear();
}

} // namespace physx