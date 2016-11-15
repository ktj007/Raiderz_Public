#include "stdafx.h"
#include "GPathFinderImpl.h"
#include <MMSystem.h>
#include "MFileSystem.h"
#include "MMath.h"
#include "MAgent.h"
#include "MPath.h"
#include "MPathCollision.h"
#include "MPathPreprocessFile.h"
#include "MPathTerrainCallback.h"
#include "MMD5.h"
#include "MProfiler.h"
#include "MStatisticsProfiler.h"
#include "GPathResMgr.h"

//////////////////////////////////////////////////////////////////////////
//

GPathFinderImpl::GPathFinderImpl() 
: m_pPathEngine(NULL)
{
	m_pPathResMgr = new GPathResMgr();
}

GPathFinderImpl::~GPathFinderImpl()
{
	m_pPathResMgr->ClearZone();
	SAFE_DELETE(m_pPathResMgr);
}

/////////////////////////////////////////////////////////////////////////////
//

typedef iPathEngine* (__stdcall* tGetInterfaceFunction)(iErrorHandler*);


iErrorHandler::eAction GPathFinderImpl::MPathFinderError::handle(const char *type, const char *description, const char *const *attributes)
{
	PFI_BLOCK_THISFUNC(7761);

	if ( strcmp(type, "Assertion") == 0)
	{
		return CONTINUE;
	}

	char pcError[384];
	pcError[0] = 0;
	sprintf_s(pcError, sizeof(pcError), "%s : %s\n", type, description);
	mlog(pcError);

	if ( strcmp(type, "Fatal") == 0)
	{
		_ASSERT(description && 0);
		return BREAK;
	}

	return CONTINUE;
}

/////////////////////////////////////////////////////////////////////////////
// path-engine 초기화

bool GPathFinderImpl::LoadPathEngine()
{
	HINSTANCE hInstance;
	hInstance = LoadLibraryW(L"pathengine.dll");	// CoreTool에서도 참조. 툴은 멀티바이트라서, 유니코드 함수 명시적 호출.
	if ( !hInstance )
		return false;

	FARPROC procAddress;
	SetLastError(0);
	procAddress = GetProcAddress(hInstance, (LPCSTR)1);
	if ( !procAddress )
		return false;

	tGetInterfaceFunction getInterfaceFunction = (tGetInterfaceFunction)procAddress;
	m_pPathEngine = getInterfaceFunction(&m_ErrorHandler);

	//tSigned32 nReleaseNumber, nMajorNumber, nMinorNumber;
	//m_pPathEngine->getReleaseNumbers(nMajorNumber, nMinorNumber, nReleaseNumber);

	//mlog("PathEngine Version %d,%d - %d\n", nMajorNumber, nMinorNumber, nReleaseNumber);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//

bool GPathFinderImpl::Create()
{
	bool bResultLoadPathEngineDLL = LoadPathEngine();
	_ASSERT(bResultLoadPathEngineDLL);
	if ( bResultLoadPathEngineDLL == false )
	{
		mlog("PathEngine DLL Loading Failed\n");

		return false;
	}

	m_pPathResMgr->SetPathEngine(m_pPathEngine);
	m_pPathResMgr->CreateDefaultShapes();

	m_QueryCallBack.Set(3, 7);

	mlog("pathengine init succeeded\n");
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//

SPathMesh* GPathFinderImpl::GetMesh(int id)
{
	return m_pPathResMgr->GetMesh(id);
}

/////////////////////////////////////////////////////////////////////////////
//

iMesh* GPathFinderImpl::GetIMesh(int id)
{
	SPathMesh* pMesh = GetMesh(id);
	if (pMesh == NULL) return NULL;

	return pMesh->m_pMesh;
}

/////////////////////////////////////////////////////////////////////////////
//
iShape* GPathFinderImpl::GetShape()
{
	return m_pPathResMgr->GetDefaultShape();
}


/////////////////////////////////////////////////////////////////////////////
//
PATHENGINE_RESULT_TYPE GPathFinderImpl::IsValidPosition(float *height, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position)
{
	PFI_BLOCK_THISFUNC(7762);

	if ( pPathMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPathMesh->m_pMesh;

	cPosition cPos = mesh->positionFor3DPointF((const float*)&position);

	// server 측의 GPathFinder 클래스에서 Z 값 offset 을 둬서 체크함
	if ( !mesh->positionIsValid(cPos))
		return INVALID_POSITION;

	iShape* shape = m_pPathResMgr->GetDefaultShape();
	if ( shape != NULL )
	{
		iCollisionContext *context = NULL;
		if ( pContext )
			context = pContext->m_pContext;

		if ( mesh->testPointCollision(shape, context, cPos))
			return COLLISION;
	}

	if ( height )
	{
		*height = mesh->heightAtPositionF(cPos) + 1.f;
	}

	return PATH_SUCCESS;
}

PATHENGINE_RESULT_TYPE GPathFinderImpl::GetHeightAtPosition(float* height, SPathMesh* pPathMesh, const MVector3& position)
{
	PFI_BLOCK_THISFUNC(7763);

	iMesh *mesh = pPathMesh->m_pMesh;
	cPosition cPos = mesh->positionFor3DPointF((const float*)&position);

	return GetHeightAtPosition(height, pPathMesh, cPos);
}

PATHENGINE_RESULT_TYPE GPathFinderImpl::GetHeightAtPosition( float *height, SPathMesh* pPathMesh, const cPosition& position )
{
	PFI_BLOCK_THISFUNC(7763);

	if ( pPathMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPathMesh->m_pMesh;

	if ( !mesh->positionIsValid(position))
	{
		return INVALID_POSITION;
	}

	if ( height )
	{
		*height = mesh->heightAtPositionF(position) + 1.f;
	}

	return PATH_SUCCESS;
}

PATHENGINE_RESULT_TYPE GPathFinderImpl::IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position)
{
	SPathMesh *pPMesh = GetMesh(nID);
	return IsValidPosition(height, pPMesh, pContext, position);
}

bool GPathFinderImpl::IsValidPosition(SPathMesh* pPathMesh, const MVector3& position)
{
	PFI_BLOCK_THISFUNC(7764);

	if ( pPathMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPathMesh->m_pMesh;

	cPosition cPos = mesh->positionFor3DPointF((const float*)&position);

	return mesh->positionIsValid(cPos);
}

PATHENGINE_RESULT_TYPE GPathFinderImpl::FindNearPosition(MVector3 *pOut, SPathMesh* pPathMesh, const MContext *pContext, const MVector3& position 
												   ,int nHorizonRange, int nVerticalRange)

{
	PFI_BLOCK_THISFUNC(7765);

	if ( pPathMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPathMesh->m_pMesh;

	cPosition cPos;

	tSigned32 iPos[3];
	iPos[0] = static_cast<int>(position.x);
	iPos[1] = static_cast<int>(position.y);
	iPos[2] = static_cast<int>(position.z);

	cPos = mesh->positionNear3DPoint(iPos, nHorizonRange, nVerticalRange);
	if ( !mesh->positionIsValid(cPos))
		return INVALID_POSITION;
	
	iCollisionContext *context = NULL;
	if ( pContext )
		context = pContext->m_pContext;

	iShape* shape = m_pPathResMgr->GetDefaultShape();
	if ( shape == NULL )
		return INVALID_PARAMETER;

	if ( mesh->testPointCollision(shape, context,cPos) )
	{
		int nSize = m_pPathResMgr->GetDefaultShapeRange();

		cPos = mesh->findClosestUnobstructedPosition(shape, context, cPos, nSize*2);
		if ( !mesh->positionIsValid(cPos) 
			|| mesh->testPointCollision(shape, context,cPos) )
			return COLLISION;
	}

	if ( pOut )
	{
		_ASSERT(cPos.cell!=-1);

		pOut->x = (float)cPos.x;
		pOut->y = (float)cPos.y;

		// WARNING ( 임의 보정값 ) : cPos -> 3D position 을 한 후 3D position에서 cPos를 다시 계산하면 제대로 된 값이 나오지 않음.
		pOut->z = mesh->heightAtPositionF(cPos) + 1.1f; 
	}

	return PATH_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//

bool GPathFinderImpl::AddZone(int nID, const wstring& strFilename, RHeightField *pHeightField)
{
	return m_pPathResMgr->AddZone(nID, strFilename, pHeightField);
}

/////////////////////////////////////////////////////////////////////////////
//
bool GPathFinderImpl::PreProcessQuery(iMesh* pMesh, iShape* pShape, const MVector3& vPos, cPosition &cPos
								  ,int nHorizonRange, int nVerticalRange)
{
	PFI_BLOCK_THISFUNC(7766);

	if ( pMesh == NULL )
		return false;

	if ( pShape == NULL )
		return false;

	cPos = pMesh->positionFor3DPointF((const float*)&vPos);
	// 실패할 경우 근사값을 검사
	if ( !pMesh->positionIsValid(cPos))
	{
		if ( nVerticalRange == 0 )
		{
			return false;
		}
		else
		{
			tSigned32 iPos[3];
			iPos[0] = static_cast<int>(vPos.x);
			iPos[1] = static_cast<int>(vPos.y);
			iPos[2] = static_cast<int>(vPos.z);

			cPos = pMesh->positionNear3DPoint(iPos, nHorizonRange, nVerticalRange);
			if ( !pMesh->positionIsValid(cPos))
				return false;
		}
	}

	if (pMesh->testPointCollision(pShape, NULL, cPos))
	{
		if ( nHorizonRange == 0 )
			return false;

		cPos = pMesh->findClosestUnobstructedPosition(pShape, NULL, cPos, nHorizonRange);
		if ( !pMesh->positionIsValid(cPos))
			return false;
		if ( pMesh->testPointCollision(pShape, NULL, cPos))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//

void GPathFinderImpl::BuildPath(iMesh* pMesh, iPath* pPath, vector<MVector3>& ret, int startoffset)
{
	PFI_BLOCK_THISFUNC(7767);

	// 만들어진 iPath*를 가지고 vector<MVector> 를 만들어낸다.
	// startoffset은 몇번째 점부터 출력 벡터에 넣을것인가?
	ret.clear();
	if (pPath == NULL) return;

	const cPosition* posarray = pPath->getPositionArray();

	for (int i=startoffset; i<pPath->size(); i++)
	{
		MVector3 v;
		v.x = (float)posarray[i].x;
		v.y = (float)posarray[i].y;
		v.z = pMesh->heightAtPositionF(posarray[i]);
		ret.push_back(v);
	}
}

//////////////////////////////////////////////////////////////////////////
//

bool GPathFinderImpl::NewContext(MContext &context, SPathMesh* pPathMesh)
{
	PFI_BLOCK_THISFUNC(7768);

	_ASSERT(context.m_pContext==NULL);

	if (pPathMesh == NULL) 
		return false;

	iMesh* mesh = pPathMesh->m_pMesh;
	if ( mesh == NULL )
		return false;

	iCollisionContext *pcontext = mesh->newContext();
	if ( pcontext == NULL )
		return false;

	context.m_pContext = pcontext;
	//context.SetZoneID(nID);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
bool GPathFinderImpl::NewPlaceAgent(MAgent &agent, const MVector3& position, int nZoneID, SPathMesh* pPathMesh)
{
	PFI_BLOCK_THISFUNC(7769);

	_ASSERT(agent.m_pAgent==NULL);

	if (pPathMesh == NULL) 
		return false;

	iMesh *mesh = pPathMesh->m_pMesh;
	iShape* shape = m_pPathResMgr->GetDefaultShape();
	if (shape == NULL) return false;

	cPosition cPos;
	if ( PreProcessQuery(mesh, shape, position, cPos))
	{
		agent.m_pAgent = mesh->placeAgent(shape, cPos);
		if ( agent.m_pAgent )
		{
			agent.m_pAgent->setUserData(static_cast<void*>(&agent));
			agent.SetMeshID(nZoneID);

			_ASSERT(!mesh->testPointCollision(shape, NULL,cPos));
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
PATHENGINE_RESULT_TYPE GPathFinderImpl::AdvancedAlongPath(MCollisionInfo *pCollisionInfoOut, int nID, MAgent &agent, MPath &path, float fDistance)
{
	PFI_BLOCK_THISFUNC(7770);

	if ( path.IsValid() )
	{
		if( !agent.IsValid())
			return INVALID_PARAMETER;

		iCollisionContext *context = NULL;
		if ( agent.m_pContext )
			context = agent.m_pContext->m_pContext;

		iCollisionInfo *pCollisionInfo = agent.m_pAgent->advanceAlongPath(path.m_pPath, fDistance, context);
		if ( pCollisionInfo )
		{
			if ( pCollisionInfoOut )
				pCollisionInfoOut->m_pCollisionInfo = pCollisionInfo;
			else
				delete pCollisionInfo;

			// 충돌 !!
			return COLLISION;
		}
		
		// 충돌하지 않음
		return PATH_SUCCESS;
	}

	// path가 무효
	return INVALID_PARAMETER;
}


/////////////////////////////////////////////////////////////////////////////
//
float GPathFinderImpl::FindPath(SPathMesh* pPathMesh, const MVector3& vStart, const MVector3& vEnd, vector<MVector3>& vPath)
{
	PFI_BLOCK_THISFUNC(7771);

	if ( pPathMesh == NULL )
		return 0.0f;

	if (vStart.XYDistanceToSq(vEnd) <= 1.0f)
	{
		return 0.0f;
	}

	iMesh *mesh = pPathMesh->m_pMesh;
	iShape* shape = m_pPathResMgr->GetDefaultShape();

	vPath.clear();

	cPosition st, en;

	if (!PreProcessQuery(mesh, shape, vStart, st, 0, (int)PICKING_ADDED_VALUE_Z*2))
		return 0.0f;
	if (!PreProcessQuery(mesh, shape, vEnd, en, 0, (int)PICKING_ADDED_VALUE_Z*2))
		return 0.0f;

	iPath* p;
	m_QueryCallBack.reset();
	p = mesh->findShortestPath_WithQueryCallBack(shape, 0, st, en, &m_QueryCallBack);

	float ret = 0.0f;
	if (p != NULL)
	{
		BuildPath(mesh, p, vPath, 1);
		ret = (float)p->getLength();

		delete p;
	}

	return ret;	
}



float GPathFinderImpl::FindPathAway(SPathMesh* pPathMesh, const MVector3& vStart, const MVector3& vCenter, float fSize, int nFreq, int nCount, vector<MVector3>& vPath)
{
	PFI_BLOCK_THISFUNC(7772);

	if ( pPathMesh == NULL )
		return 0.f;
	iMesh *mesh = pPathMesh->m_pMesh;
	iShape* shape = m_pPathResMgr->GetDefaultShape();

	vPath.clear();

	cPosition st, center;

	if (!PreProcessQuery(mesh, shape, vStart, st))
		return 0.0f;
	if (!PreProcessQuery(mesh, shape, vCenter, center))
		return 0.0f;

	iPath* p;
	m_QueryCallBack.Set(nFreq, nCount);
	p = mesh->findPathAway_WithQueryCallBack(shape, 0, st, center, (int)fSize, &m_QueryCallBack);

	float ret = 0.0f;
	if (p != NULL)
	{
		BuildPath(mesh, p, vPath, 1);
		ret = (float)p->getLength();

		delete p;
	}

	return ret;	
}


/////////////////////////////////////////////////////////////////////////////

void GPathFinderImpl::PathTo3DPoint(MVector3 &outPos, SPathMesh* pPathMesh, MPath &basePath, int index)
{
	PFI_BLOCK_THISFUNC(7773);

	if ( basePath.IsValid() && pPathMesh)
	{
		iPath *path = basePath.m_pPath;
		iMesh *mesh = pPathMesh->m_pMesh;
		if ( mesh == NULL )
			return;

		tSigned32 size = path->size();
		if ( index < size )
		{
			cPosition cPos = path->position(index);

			if (!mesh->positionIsValid(cPos))
				return;

			outPos.x = static_cast<float>(cPos.x);
			outPos.y = static_cast<float>(cPos.y);
			outPos.z = mesh->heightAtPositionF(cPos) + 1.1f;
		}
	}
}

void GPathFinderImpl::PathTo3DPoints(std::vector<MVector3> &out3DPoints, SPathMesh* pPathMesh, MPath &basePath, int offset)
{
	PFI_BLOCK_THISFUNC(7774);

	if ( basePath.IsValid() && pPathMesh)
	{
		iPath *path = basePath.m_pPath;
		iMesh *mesh = pPathMesh->m_pMesh;
		if ( mesh == NULL )
			return;

		int nSize = path->size();
		if ( offset >= nSize )
			offset = nSize-1;

		out3DPoints.resize(nSize-offset);
		if ( nSize > offset)
		{
			for ( int i = offset; i<nSize; ++i )
			{
				cPosition cPos = path->position(i);

				if (!mesh->positionIsValid(cPos))
					continue;
				
				out3DPoints[i-offset].x = static_cast<float>(cPos.x);
				out3DPoints[i-offset].y = static_cast<float>(cPos.y);
				out3DPoints[i-offset].z = mesh->heightAtPositionF(cPos) + 1.1f;
			}
		}
	}
}
