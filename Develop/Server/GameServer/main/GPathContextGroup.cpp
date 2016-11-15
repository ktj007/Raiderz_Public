#include "stdafx.h"
#include "GPathContextGroup.h"
#include "GPathFinder.h"
#include "GEntityNPC.h"
#include "GNPCPathFinder.h"
#include "GGlobal.h"

void GPathContextGroup::Register( MUID& uidTarget, GEntityNPC* pNPC )
{
	if (m_pPathMesh == 0) return;

	MContext* pContext = NULL;
	map<MUID, MContext*>::iterator itor = m_ContextMap.find(uidTarget);
	if (itor != m_ContextMap.end())
	{
		pContext = (*itor).second;
	}
	else
	{
		pContext = InsertContext(uidTarget);
	}

	if (pContext == NULL) return;

	pNPC->GetPathFinder()->RegisterToContext(*pContext);
}

void GPathContextGroup::Revoke( GEntityNPC* pNPC )
{
	if (m_pPathMesh == 0) return;

	pNPC->GetPathFinder()->RevokeFromContext();
}

GPathContextGroup::GPathContextGroup() : m_pPathMesh(NULL)
{
	const float UPDATE_TICK = 30.0f;
	m_Updater.SetTickTime(UPDATE_TICK);
}

GPathContextGroup::~GPathContextGroup()
{
	while (!m_ContextMap.empty())
	{
		map<MUID, MContext*>::iterator itor = m_ContextMap.begin();
		MContext* pContext = (*itor).second;
		DeleteContext(pContext);
		m_ContextMap.erase(itor);
	}
}

MContext* GPathContextGroup::InsertContext( MUID& uidTarget )
{
	MContext* pContext = new MContext();
	m_ContextMap.insert(map<MUID,MContext*>::value_type(uidTarget, pContext));
	gsys.pPathFinder->NewContext(*pContext, m_pPathMesh);

	return pContext;
}

void GPathContextGroup::DeleteContext( MContext* pContext )
{
	pContext->Release();
	delete pContext;
}

void GPathContextGroup::Init( int nZoneID )
{
	m_pPathMesh = gsys.pPathFinder->GetMesh(nZoneID);
}

void GPathContextGroup::Update( float fDelta )
{
	if (m_pPathMesh == NULL) return;
	if (m_Updater.IsReady(fDelta) == false) return;

	for (map<MUID, MContext*>::iterator itor = m_ContextMap.begin(); itor != m_ContextMap.end(); )
	{
		MContext* pContext = (*itor).second;
		if (pContext->GetAgentNum() == 0)
		{
			DeleteContext(pContext);
			itor = m_ContextMap.erase(itor);
		}
		else
		{
			++itor;
		}
	}

#ifdef _DEBUG
	int nCnt = 0;
	for (map<MUID, MContext*>::iterator itor = m_ContextMap.begin(); itor != m_ContextMap.end(); ++itor)
	{
		MContext* pContext = (*itor).second;
		nCnt += pContext->GetAgentNum();
	}

	blog("PathContext: Context Count(%d), Total Agent(%d)\n", (int)m_ContextMap.size(), nCnt);
#endif
}

void GPathContextGroup::OnRemoveEntity( GEntity* pEntity )
{
	if (pEntity->GetTypeID() == ETID_NPC)
	{
		GEntityNPC* pEntityNPC = static_cast<GEntityNPC*>(pEntity);
		Revoke(pEntityNPC);
	}

	map<MUID, MContext*>::iterator itor = m_ContextMap.find(pEntity->GetUID());
	if (itor != m_ContextMap.end())
	{
		MContext* pContext = (*itor).second;
		DeleteContext(pContext);
		m_ContextMap.erase(itor);
	}
}