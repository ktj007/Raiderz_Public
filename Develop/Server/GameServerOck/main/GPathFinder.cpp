#include "stdafx.h"
#include "GPathFinder.h"
#include "MAgent.h"
#include "MPath.h"
#include "MPathCollision.h"
#include "GDef.h"
#include "GAIProfiler.h"
#include "MProfiler.h"
#include "MStatisticsProfiler.h"
#include "GConst.h"

float GPathFinder::OFFSET_Z = PICKING_ADDED_VALUE_Z;

int GPathFinder::TEST_PathfindZoneID = -1;
cPosition GPathFinder::TEST_PathfindStartRawPos;
cPosition GPathFinder::TEST_PathfindEndRawPos;
cPosition GPathFinder::TEST_PathfindStartPos;
cPosition GPathFinder::TEST_PathfindEndPos;

const int PATHENGINE_COORDINATE_RANGE = 1500000;

void GQueryCallBack::Set( tSigned32 frequency, tSigned32 count )
{
	m_nStartTime = timeGetTime();
	_frequency = frequency;
	_count = __count = count;
}

bool GQueryCallBack::abort()
{
	if (timeGetTime() - m_nStartTime > 20)
	//if (timeGetTime() - m_nStartTime > 1000)
	{
		vec3 vStartPos(static_cast<float>(_startPos.x), static_cast<float>(_startPos.y), _mesh->heightAtPositionF(_startPos) + 1.1f);
		vec3 vDestPos(static_cast<float>(_destPos.x), static_cast<float>(_destPos.y), _mesh->heightAtPositionF(_destPos) + 1.1f);

		if (GConst::LOG_PATHFIND)
		{
			mlog("%s - timed out. (%d ms, start: %.1f %.1f %.1f, dest: %.1f %.1f %.1f)\n", 
				__FUNCTION__, timeGetTime() - m_nStartTime,
				vStartPos.x, vStartPos.y, vStartPos.z,
				vDestPos.x, vDestPos.y, vDestPos.z
				);
		}

		m_bAborted = true;
		return true;
	}
	--_count;

	//if (_count == 0) 
	//{
	//	mlog("%s - count is zero\n", __FUNCTION__);
	//	m_bAborted = true;
	//	return true;
	//}

	return false;
}

void GQueryCallBack::reset( iMesh* mesh, cPosition startPos, cPosition destPos )
{
	_mesh = mesh;
	_startPos = startPos;
	_destPos = destPos;
	m_bAborted = false;
	_count = __count;
	m_nStartTime = timeGetTime();
}

GPathFinder::GPathFinder()
{
	m_PathFinder.Create();
	m_QueryCallback.Set(3, 7);
}

GPathFinder::~GPathFinder()
{

}

bool GPathFinder::AddZone(int nZoneID, const wchar_t* szFileName, RHeightField *pHeightField)
{
	// 파일 읽기
	return m_PathFinder.AddZone(nZoneID, szFileName, pHeightField);
}

bool GPathFinder::NewPlaceAgent(MAgent &agent, const vec3& position, int nZoneID, SPathMesh* pPathMesh)
{
	// INFO : 보정값 더하기
	vec3 vPos( position.x, position.y, position.z + OFFSET_Z);
	bool bResult = m_PathFinder.NewPlaceAgent(agent, vPos, nZoneID, pPathMesh);

	return bResult;
}


PATHENGINE_RESULT_TYPE GPathFinder::IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position)
{
	// 이 _ASSERT에 걸리는 상황은 절대 없어야 합니다.
	_ASSERT(position.x >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.x <= PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y <= PATHENGINE_COORDINATE_RANGE);

	vec3 vPosition(position.x, position.y, position.z + OFFSET_Z);
	return m_PathFinder.IsValidPosition(height, nID, pContext, vPosition);
}

PATHENGINE_RESULT_TYPE GPathFinder::IsValidPosition(float *height, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position)
{
	// 이 _ASSERT에 걸리는 상황은 절대 없어야 합니다.
	_ASSERT(position.x >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.x <= PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y <= PATHENGINE_COORDINATE_RANGE);

	vec3 vPosition(position.x, position.y, position.z + OFFSET_Z);
	return m_PathFinder.IsValidPosition(height, pPathMesh, pContext, vPosition);
}

PATHENGINE_RESULT_TYPE GPathFinder::GetHeightAtPosition( float *height, SPathMesh* pPathMesh, const MVector3& position )
{
	// 이 _ASSERT에 걸리는 상황은 절대 없어야 합니다.
	_ASSERT(position.x >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.x <= PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y <= PATHENGINE_COORDINATE_RANGE);

	vec3 vPosition(position.x, position.y, position.z + OFFSET_Z);
	return m_PathFinder.GetHeightAtPosition(height, pPathMesh, vPosition);
}

bool GPathFinder::IsValidPosition(SPathMesh* pPathMesh, const MVector3& position)
{
	// 이 _ASSERT에 걸리는 상황은 절대 없어야 합니다.
	_ASSERT(position.x >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.x <= PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y <= PATHENGINE_COORDINATE_RANGE);

	vec3 vPosition(position.x, position.y, position.z + OFFSET_Z);
	return m_PathFinder.IsValidPosition(pPathMesh, vPosition);
}

PATHENGINE_RESULT_TYPE GPathFinder::FindNearPosition(MVector3 *pOut, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position
													,int nHorizonRange, int nVerticalRange)
{
	// 이 _ASSERT에 걸리는 상황은 절대 없어야 합니다.
	_ASSERT(position.x >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.x <= PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y >=-PATHENGINE_COORDINATE_RANGE);
	_ASSERT(position.y <= PATHENGINE_COORDINATE_RANGE);
	_ASSERT(nHorizonRange<=PATHENGINE_COORDINATE_RANGE);

	vec3 vPosition(position.x, position.y, position.z + OFFSET_Z);
	return m_PathFinder.FindNearPosition(pOut, pPathMesh, pContext, vPosition, nHorizonRange, nVerticalRange);
}


bool GPathFinder::FindPath_NoAgent(SPathMesh* pPathMesh, vec3 vStart, vec3 vEnd, vector<vec3>& vPath)
{
	FindNearPosition(&vStart, pPathMesh, NULL, vStart, 100, (int)OFFSET_Z*2);
	FindNearPosition(&vEnd, pPathMesh, NULL, vEnd, 100, (int)OFFSET_Z*2);

	float retDistance = m_PathFinder.FindPath(pPathMesh, vStart, vEnd, vPath);
	return (retDistance != 0.0f);
}

GPathFinder::QueryResult GPathFinder::FindPath(MPath &outPath, SPathMesh* pPathMesh, const MAgent &agent, MVector3 vEnd)
{
	vEnd.z += OFFSET_Z;

	return _FindPath(outPath, pPathMesh, agent, vEnd);
}

GPathFinder::QueryResult GPathFinder::_FindPath( MPath& outPath, SPathMesh* pPathMesh, const MAgent& agent, const vec3& vEnd)
{
	PFI_BLOCK_THISFUNC(7715);

	if ( !agent.IsValid()) 
	{
		return QUERY_FAILED;
	}

	SPathMesh* pPMesh;
	iMesh* mesh;
	iShape* shape;
	iCollisionContext* context;
	cPosition cEndPos;

	pPMesh = pPathMesh;
	if ( pPMesh == NULL ) return QUERY_FAILED;

	mesh = pPMesh->m_pMesh;
	shape = m_PathFinder.GetShape();

	_ASSERT(mesh);
	_ASSERT(shape);

	context = NULL;
	if ( agent.IsInContext())
		context = agent.GetContext()->m_pContext;
	
	cEndPos = mesh->positionFor3DPointF(vEnd.v);

	//if ( !mesh->positionIsValid(cEndPos))
	//{
	//	tSigned32 iPos[3];
	//	iPos[0] = static_cast<int>(vEnd.x);
	//	iPos[1] = static_cast<int>(vEnd.y);
	//	iPos[2] = static_cast<int>(vEnd.z);
	//	cEndPos = mesh->positionNear3DPoint(iPos, 100, 100);
	//}

	cPosition cStartPos = agent.m_pAgent->getPosition();

	if (cEndPos.x == cStartPos.x && cEndPos.y == cStartPos.y) 
		return QUERY_FAILED;

	TEST_PathfindZoneID = pPMesh->m_nID;
	TEST_PathfindStartRawPos = cStartPos;
	TEST_PathfindEndRawPos = cEndPos;
	if (GConst::LOG_PATHFIND)
	{
		mlog("\t내부: 길찾기 시도 (보정전) (start: %d %d %d) (dest: %d %d %d)\n", 
			cStartPos.x, cStartPos.y, cStartPos.cell, 
			cEndPos.x, cEndPos.y, cEndPos.cell);
	}

	if (!mesh->positionIsValid(cStartPos))
	{
		if (GConst::LOG_PATHFIND)
		{
			mlog("\t내부: 목표 위치 보정 (시작점)\n");
		}

		return QUERY_FAILED;

		//cStartPos = mesh->findClosestUnobstructedPosition(shape, context, cStartPos, 200);
		//if ( !mesh->positionIsValid(cStartPos))
		//	return QUERY_FAILED;

		//agent.m_pAgent->moveTo(cStartPos);
	}	

	if (!mesh->positionIsValid(cEndPos))
	{
		if (GConst::LOG_PATHFIND)
		{
			mlog("\t내부: 목표 위치 보정 (목표점)\n");
		}

		return QUERY_FAILED;

		//cEndPos = mesh->findClosestUnobstructedPosition(shape, context, cEndPos, 200);
		//if ( !mesh->positionIsValid(cEndPos))
		//	return QUERY_FAILED;
	}	

	if (mesh->testPointCollision(shape, context, cStartPos))
	{
		if (GConst::LOG_PATHFIND)
		{
			mlog("\t시작점 충돌\n");
		}
		cStartPos = mesh->findClosestUnobstructedPosition(shape, context, cStartPos, 400);
		if ( !mesh->positionIsValid(cStartPos))
			return QUERY_FAILED;
		if ( mesh->testPointCollision(shape, context, cStartPos))
			return QUERY_FAILED;
	}

	if (mesh->testPointCollision(shape, context, cEndPos))
	{
		if (GConst::LOG_PATHFIND)
		{
			mlog("\t목표점 충돌\n");
		}
		cEndPos = mesh->findClosestUnobstructedPosition(shape, context, cEndPos, 400);
		if ( !mesh->positionIsValid(cEndPos))
			return QUERY_FAILED;
		if ( mesh->testPointCollision(shape, context, cEndPos))
			return QUERY_FAILED;
	}


	TEST_PathfindStartPos = cStartPos;
	TEST_PathfindEndPos = cEndPos;
	if (GConst::LOG_PATHFIND)
	{
		mlog("\t내부: 길찾기 시도 (start: %d %d %d) (dest: %d %d %d)\n", 
			cStartPos.x, cStartPos.y, cStartPos.cell, 
			cEndPos.x, cEndPos.y, cEndPos.cell);
	}
	
	m_QueryCallback.reset(mesh, cStartPos, cEndPos);
	iPath* path = agent.m_pAgent->findShortestPathTo_WithQueryCallBack(context, cEndPos, &m_QueryCallback);

	QueryResult nResult = QUERY_SUCCESS;
	if (m_QueryCallback.IsAborted())
	{
		nResult = QUERY_SUCCESS_BUT_ABORTED;
	}

	if ( path == NULL )
		return QUERY_FAILED;

	_ASSERT(outPath.m_pPath == NULL);
	outPath.m_pPath = path;
	return nResult;
}

float GPathFinder::FindPathAway(SPathMesh* pPathMesh, vec3 vStart, vec3 vCenter, float fSize, int nFreq, int nCount, vector<vec3>& vPath)
{
	PFI_BLOCK_THISFUNC(7716);

	// INFO : 보정값 더하기
	vStart.z += OFFSET_Z;
	vCenter.z += OFFSET_Z;

	float value = m_PathFinder.FindPathAway(pPathMesh, vStart, vCenter, fSize, nFreq, nCount, vPath);

	return value;
}



bool GPathFinder::NewContext( MContext &context, SPathMesh* pPathMesh )
{
	PFI_BLOCK_THISFUNC(7718);

	return m_PathFinder.NewContext(context, pPathMesh);
}

void GPathFinder::PathTo3DPoints( std::vector<MVector3> &out3DPoints, SPathMesh* pPathMesh, MPath &basePath, int offset /*= 0*/ )
{
	PFI_BLOCK_THISFUNC(7719);

	m_PathFinder.PathTo3DPoints(out3DPoints, pPathMesh, basePath, offset);
}

void GPathFinder::PathTo3DPoint( MVector3 &outPos, SPathMesh* pPathMesh, MPath &basePath, int index )
{
	PFI_BLOCK_THISFUNC(7720);

	m_PathFinder.PathTo3DPoint(outPos, pPathMesh, basePath, index);
}


