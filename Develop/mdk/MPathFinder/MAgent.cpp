#include "stdafx.h"
#include "MPathFinder.h"
#include "MAgent.h"
#include "MPath.h"
#include "MPathCollision.h"
#include "MStatisticsProfiler.h"

#define NEAR_3D_POINT_RANGE_HORIZON		100
#define NEAR_3D_POINT_RANGE_VERTICAL	100

MAgent::MAgent()
: m_pAgent(NULL)
, m_nMeshID(-1)
, m_pContext(NULL)
{
}

MAgent::~MAgent()
{
}

bool MAgent::IsInContext() const
{
	return ( m_pContext ? true : false );
}

void MAgent::Release()
{
	if ( IsInContext())
		RemoveFromContext(m_pContext);

	if ( m_pAgent )
	{
		m_pAgent->setUserData(NULL);
		delete m_pAgent;
	}
		
	m_pAgent = NULL;
	m_nMeshID = -1;
}

bool MAgent::AddToContext(MContext *pContext)
{
	if (m_pContext != NULL)
		return false;

	pContext->m_pContext->addAgent(m_pAgent);
	m_pContext = pContext;

	return true;
}

bool MAgent::RemoveFromContext(MContext *pContext)
{
	if (m_pContext!=pContext)
		return false;

	pContext->m_pContext->removeAgent(m_pAgent);
	m_pContext = NULL;

	return true;
}

bool MAgent::GetPosition(MVector3 *pOut) const
{
	PFI_BLOCK_THISFUNC(7003);
	if ( m_pAgent )
	{
		cPosition cPos = m_pAgent->getPosition();
		iMesh *mesh = m_pAgent->getMesh();
		float fZ = mesh->heightAtPositionF(cPos);

		if ( pOut )
		{
			pOut->x = static_cast<float>(cPos.x);
			pOut->y = static_cast<float>(cPos.y);
			pOut->z = fZ;
		}
		return true;
	}
	return false;
}

PATHENGINE_RESULT_TYPE MAgent::MoveTo(const MVector3 &pos, bool bForceMove)
{
	PFI_BLOCK_THISFUNC(7001);

	if ( m_pAgent )
	{
		iMesh *mesh = m_pAgent->getMesh();
		if ( mesh == NULL )
		{
			dlog("Agent Mesh is NULL\n");
			return INVALID_PARAMETER;
		}

		MVector3 vec(pos);
		vec.z += 50.f;

		//tSigned32 iPos[3];
		//iPos[0] = static_cast<int>(vec.x);
		//iPos[1] = static_cast<int>(vec.y);
		//iPos[2] = static_cast<int>(vec.z);
		//cPosition cPos = mesh->positionNear3DPoint(iPos, NEAR_3D_POINT_RANGE_HORIZON, NEAR_3D_POINT_RANGE_VERTICAL);
		cPosition cPos;
		cPos = mesh->positionFor3DPointF(vec.v);
		if ( !mesh->positionIsValid(cPos))
			return INVALID_POSITION;

		iCollisionContext *context = NULL;
		if ( m_pContext && !bForceMove)
			context = m_pContext->m_pContext;

		//if ( mesh->testPointCollision(m_pAgent->getShape(), context, cPos))
		//	return COLLISION;

		m_pAgent->moveTo(cPos);

		return PATH_SUCCESS;
	}

	return INVALID_PARAMETER;
}

PATHENGINE_RESULT_TYPE MAgent::MoveTo(const MVector3 &pos)
{
	PFI_BLOCK_THISFUNC(7002);

	if ( !m_pAgent )
		return INVALID_PARAMETER;

	if ( !m_pAgent->isMoveable())
		return INVALID_PARAMETER;

	iMesh *mesh = m_pAgent->getMesh();
	if ( mesh == NULL )
	{
		return INVALID_PARAMETER;
	}

	MVector3 vec(pos);
	vec.z += 50.f;

	cPosition cPos;
	cPos = mesh->positionFor3DPointF(vec.v);

	if ( !mesh->positionIsValid(cPos))
		return INVALID_POSITION;

	//if ( !mesh->positionIsValid(cPos))
	//{
	//	tSigned32 iPos[3];
	//	iPos[0] = static_cast<int>(vec.x);
	//	iPos[1] = static_cast<int>(vec.y);
	//	iPos[2] = static_cast<int>(vec.z);
	//	cPos = mesh->positionNear3DPoint(iPos, NEAR_3D_POINT_RANGE_HORIZON, NEAR_3D_POINT_RANGE_VERTICAL);

	//	if ( !mesh->positionIsValid(cPos))
	//		return INVALID_POSITION;
	//}

	m_pAgent->moveTo(cPos);

	return PATH_SUCCESS;
}

void MAgent::MakeCurvePath(MPath &basePath, int sectionLength, float turnRatio1, float turnRation2)
{
	if (basePath.IsValid() && m_pAgent)
	{
		iPath *pPath = m_pAgent->generateCurvedPath(basePath.m_pPath, 0, 0, 0, sectionLength, turnRatio1, turnRation2);

		basePath.Release();
		basePath.m_pPath = pPath;
	}
}

void MAgent::SetTraverseCost( float fCost )
{
	if (m_pAgent) m_pAgent->setTraverseCost(fCost);
}

float MAgent::GetTraverseCost()
{
	if (m_pAgent) return m_pAgent->getTraverseCost();
	return -1.0f;
}