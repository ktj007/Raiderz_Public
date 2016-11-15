#include "stdafx.h"
#include "MPathCollision.h"
#include "MPathFinder.h"
#include "MAgent.h"

MCollisionInfo::MCollisionInfo()
: m_pCollisionInfo(NULL)
{
}

MCollisionInfo::~MCollisionInfo()
{
}

MAgent* MCollisionInfo::GetCollisionAgent()
{
	if ( m_pCollisionInfo == NULL )
		return NULL;

	iAgent *agent = m_pCollisionInfo->getCollidingAgent();
	if ( agent )
	{
		return static_cast<MAgent*>(agent->getUserData());
	}

	return NULL;
}

void MCollisionInfo::Release()
{
	if ( m_pCollisionInfo )
	{
		delete m_pCollisionInfo;
		m_pCollisionInfo = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////

MContext::MContext()
: m_pContext(NULL)
{
}

MContext::~MContext()
{
}

void MContext::Release()
{
	if ( m_pContext )
	{
		// context 의 모든 agent를 순회하여 agent의 Context 포인터를 지운다.
		while ( tSigned32 numAgent = m_pContext->getNumberOfAgents())
		{
			iAgent *agent = m_pContext->getAgent(0);
			MAgent *pAgent = (MAgent*)agent->getUserData();
			if ( pAgent )
				pAgent->RemoveFromContext(this);
		}

		delete m_pContext;
		m_pContext = NULL;
	}
}

bool MContext::AddAgent(MAgent &agent)
{
	if ( agent.IsValid())
	{
		_ASSERT(agent.IsInContext()==false);
		if ( agent.IsInContext()==false )
			return agent.AddToContext(this);

	}

	return false;
}

bool MContext::RemoveAgent(MAgent &agent)
{
	if ( agent.IsValid())
	{
		_ASSERT(agent.IsInContext()==true);
		if ( agent.IsInContext()==true)
			return agent.RemoveFromContext(this);

	}
	return false;
}

int MContext::GetAgentNum()
{
	if ( m_pContext )
		return m_pContext->getNumberOfAgents();
	
	return 0;
}