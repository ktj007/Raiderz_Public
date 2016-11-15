/// TODO : shape 정책 정리
/// TODO : 

#include "stdafx.h"
#include "MPathFinder.h"
#include <MMSystem.h>
#include "MFileSystem.h"
#include "MMath.h"
#include "MAgent.h"
#include "MPath.h"
#include "MPathCollision.h"
#include "MVector2.h"
#include "MPathPreprocessFile.h"
#include "MPathTerrainCallback.h"

#include "MMD5.h"
#include "MProfiler.h"

//////////////////////////////////////////////////////////////////////////
//

MPathFinder::MPathFinder() 
: m_pPathEngine(NULL)
, m_hInstance(0)
{
}

MPathFinder::~MPathFinder()
{
	ClearZone();
}

/////////////////////////////////////////////////////////////////////////////
//

typedef iPathEngine* (__stdcall* tGetInterfaceFunction)(iErrorHandler*);


iErrorHandler::eAction MPathFinder::MPathFinderError::handle(const char *type, const char *description, const char *const *attributes)
{
	char pcError[384];
	pcError[0] = 0;
	sprintf_s(pcError, sizeof(pcError), "%s : %s\n", type, description);
	mlog(pcError);

#if 0
	MessageBox(NULL, description, type, MB_OK);
#endif
	if ( strcmp(type,"Fatal") == 0)
	{
		_ASSERT(description && 0);
		return BREAK;
	}

	return CONTINUE;
}

/////////////////////////////////////////////////////////////////////////////
// path-engine 초기화

bool MPathFinder::LoadPathEngine()
{
	m_hInstance = LoadLibrary("pathengine.dll");
	if ( !m_hInstance )
		return false;

	FARPROC procAddress;
	SetLastError(0);
	procAddress = GetProcAddress(m_hInstance, (LPCSTR)1);
	if ( !procAddress )
		return false;

	tGetInterfaceFunction getInterfaceFunction = (tGetInterfaceFunction)procAddress;
	m_pPathEngine = getInterfaceFunction(&m_ErrorHandler);

	tSigned32 nReleaseNumber, nMajorNumber, nMinorNumber;
	m_pPathEngine->getReleaseNumbers(nMajorNumber, nMinorNumber, nReleaseNumber);

	mlog("PathEngine Version %d,%d - %d\n", nMajorNumber, nMinorNumber, nReleaseNumber);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//

bool MPathFinder::Create()
{
	mlog("pathengine initializing...\n");

	bool bResultLoadPathEngineDLL = LoadPathEngine();
	_ASSERT(bResultLoadPathEngineDLL);
	if ( bResultLoadPathEngineDLL == false )
		return false;

	// 기본 shape 세트를 만든다. 하드코딩 되어 있음
	m_Shapes.CreateDefaultShapes(m_pPathEngine);

	mlog("pathengine init succeeded\n");
	return true;
}

void MPathFinder::Destroy()
{
	ClearZone();

	if(m_hInstance)
	{
		FreeLibrary( m_hInstance );
		m_hInstance = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
//

SPathMesh* MPathFinder::GetMesh(int id)
{
	// TODO : Optimize 필요성 있음.
	int nSize = (int)m_vZoneList.size();
	for (int i = 0; i<nSize; ++i)
	{
		if (m_vZoneList[i].m_nID == id)
			return &(m_vZoneList[i]);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//

iMesh* MPathFinder::GetIMesh(int id)
{
	SPathMesh* pMesh = GetMesh(id);
	if (pMesh == NULL) return NULL;

	return pMesh->m_pMesh;
}

/////////////////////////////////////////////////////////////////////////////
//
iShape* MPathFinder::GetShape(const string& strShape)
{
	MPathAgentShape *pShape = m_Shapes.GetShape(strShape);
	if ( pShape == NULL )
		return NULL;

	return pShape->m_pShape;
}

int MPathFinder::GetShapeRange(const string& strShape)
{
	MPathAgentShape *pShape = m_Shapes.GetShape(strShape);
	if ( pShape == NULL )
		return 0;

	return pShape->m_nSize;
}

/////////////////////////////////////////////////////////////////////////////
//

PATHENGINE_RESULT_TYPE MPathFinder::IsValidPosition(float *height, int nID, const MContext *pContext, const MVector3& position
													, const string& strShape /* =  */)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *shape = m_Shapes.GetShape(strShape);
	if ( shape == NULL )
		return INVALID_PARAMETER;

	cPosition cPos;
	cPos = mesh->positionFor3DPointF((const float*)&position);
	
	// server 측의 GPathFinder 클래스에서 Z 값 offset 을 둬서 체크하므로, 이 부분을 사용하지 않음.
	if ( !mesh->positionIsValid(cPos))
		return INVALID_POSITION;

	iCollisionContext *context = NULL;
	if ( pContext )
		context = pContext->m_pContext;

	if ( mesh->testPointCollision(shape->m_pShape, context, cPos))
		return COLLISION;

	if ( height )
		*height = mesh->heightAtPositionF(cPos) + 1.f;

	return PATH_SUCCESS;
}

PATHENGINE_RESULT_TYPE MPathFinder::IsNearPosition(MVector3 *pOut, int nID, const MContext *pContext, const MVector3& position 
												   ,int nHorizonRange, int nVerticalRange, const string& strShape /* =  */)

{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *shape = m_Shapes.GetShape(strShape);
	if ( shape == NULL )
		return INVALID_PARAMETER;

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

	if ( mesh->testPointCollision(shape->m_pShape, context,cPos) )
	{
		int nSize = GetShapeRange(strShape);

		cPos = mesh->findClosestUnobstructedPosition(shape->m_pShape, context, cPos, nSize*2);
		if ( !mesh->positionIsValid(cPos) 
			|| mesh->testPointCollision(shape->m_pShape, context,cPos) )
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

//////////////////////////////////////////////////////////////////////////
//
bool MPathFinder::LoadNavigationMeshFile(int nID, SPathMesh &outPathMesh, char pcOutMD5String[16], const string &strFilename, RHeightField *pHeightField)
{
	MFile file;
	if ( !file.Open(strFilename.c_str()))
	{
		mlog("Navigation %d - %s is not found\n", nID, strFilename.c_str());
		return false;
	}

	// read file stream
	UINT fileSize = file.GetLength();
	char *pBuffer = new char[fileSize];
	if ( file.Read((void*)pBuffer, fileSize) == false 
		|| fileSize == 0)
	{
		delete [] pBuffer;
		pBuffer = NULL;
		return false;
	}
	file.Close();

	// hash 를 기록
	{
		MMD5 md5;
		md5.md5_string((unsigned char*)pBuffer, fileSize, (unsigned char*)pcOutMD5String);
	}
	
	outPathMesh.m_nID = nID;
	outPathMesh.m_pMesh = m_pPathEngine->loadMeshFromBuffer("tok", pBuffer, fileSize, 0);

	delete [] pBuffer;
	pBuffer = NULL;

	if ( outPathMesh.m_pMesh == NULL )
	{
		mlog("FatalError : Navigation mesh %d - %s is load failed\n", nID, strFilename.c_str());
		return false;
	}

	if ( pHeightField )
	{
		outPathMesh.m_pTerrainCallback = new MPathTerrainCallback(pHeightField);
		outPathMesh.m_pMesh->setTerrainCallBack(0, outPathMesh.m_pTerrainCallback);
	}

	return true;
}

bool MPathFinder::LoadPreprocessFile(SPathMesh &pathMesh, const char pcMD5String[16], const string &strFilename, const vector<string>& vShapes, bool bAutoGeneration)
{
	_ASSERT(pathMesh.m_nID>=0);
	_ASSERT(pathMesh.m_pMesh!=NULL);

	if ( vShapes.empty())
		return false;

	bool bAllResult = true;
	// pre-process file loading
	for (size_t i=0; i<vShapes.size(); ++i )
	{
		// name으로 shape를 가져온다.
		MPathAgentShape* pShape = m_Shapes.GetShape(vShapes[i]);
		_ASSERT(pShape);
		_ASSERT(pShape->m_pShape);

		// 충돌/길찾기 전처리 파일명
		string collisionfilename = strFilename + "." + vShapes[i] + ".collision.pathengine";
		string pathfilename = strFilename + "." + vShapes[i] + ".pathfind.pathengine";

		// parameter
		const char* pathfind_attributes[3];
		pathfind_attributes[0] = "split_with_circumference_below";
		pathfind_attributes[1] = "4000";
		pathfind_attributes[2] = 0;

		bool bResult = MPathFindPreprocess::LoadPreprocess(GetPathEngineInterface(), pathMesh.m_pMesh, pcMD5String, pShape->m_pShape, pathfilename.c_str(), collisionfilename.c_str());

		if (!bResult)
		{
			mlog("%s shape preprocess file loading failed\n", vShapes[i].c_str());
			bAllResult = bAllResult && bResult;
			if ( bAutoGeneration )
			{
				MPathFindPreprocess::GenerateFile(pathMesh.m_pMesh, pcMD5String, pShape->m_pShape, pathfilename.c_str(), collisionfilename.c_str(), NULL, pathfind_attributes);
			}
		}
		else
		{
			//mlog("%s shape preprocess file loading success\n", vShapes[i].c_str());
			pathMesh.m_vShapeName.push_back(vShapes[i]);
		}
	}

	if ( bAutoGeneration )
		return true;
	else
		return bAllResult;
}

/////////////////////////////////////////////////////////////////////////////
//

bool MPathFinder::AddZone(int nID, const string& strFilename, RHeightField *pHeightField, bool bAutoGeneratePreprocess /* = true */)
{
	vector<string> t;

	// 기본으로 small shape에 대해서만 추가한다.
	t.push_back("small");
//	t.push_back("medium");
//	t.push_back("large");
	return AddZone(nID, strFilename, t, pHeightField);	
}

/////////////////////////////////////////////////////////////////////////////
//
bool MPathFinder::AddZone(int nID, const string& strFilename, const vector<string>& vShapes, RHeightField *pHeightField /* = NULL */, bool bAutoGeneratePreprocess /* = true */)
{
	string meshfilename = strFilename + ".navmesh.pathengine";

	SPathMesh pathMesh;

	char pcMD5String[16];
	memset(pcMD5String, 0, sizeof(pcMD5String));

	if ( !LoadNavigationMeshFile(nID, pathMesh, pcMD5String, meshfilename, pHeightField))
		return false;

	if ( !LoadPreprocessFile(pathMesh, pcMD5String, strFilename, vShapes, bAutoGeneratePreprocess))
		return false;

	m_vZoneList.push_back(pathMesh);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
bool MPathFinder::PreProcessQuery(iMesh* pMesh, MPathAgentShape* pShape, const MVector3& vPos, cPosition &cPos
								  ,int nHorizonRange, int nVerticalRange)
{
	if ( pMesh == NULL )
		return false;

	if ( pShape == NULL )
		return false;

	iShape* shape = pShape->m_pShape;
	if ( shape == NULL )
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

	if ( pMesh->testPointCollision(shape, NULL, cPos))
	{
		if ( nHorizonRange == 0 )
			return false;

		cPos = pMesh->findClosestUnobstructedPosition(shape, NULL, cPos, nHorizonRange);
		if ( !pMesh->positionIsValid(cPos))
			return false;
		if ( pMesh->testPointCollision(shape, NULL, cPos))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//

void MPathFinder::BuildPath(iMesh* pMesh, iPath* pPath, vector<MVector3>& ret, int startoffset)
{
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

bool MPathFinder::NewContext(MContext &context, int nID)
{
	_ASSERT(context.m_pContext==NULL);
	iMesh *mesh = GetIMesh(nID);
	if ( mesh == NULL )
		return false;

	iCollisionContext *pcontext = mesh->newContext();
	if ( pcontext == NULL )
		return false;

	context.m_pContext = pcontext;
	//context.m_nID = nID;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
bool MPathFinder::NewPlaceAgent(MAgent &agent, const MVector3& position, int nID, const string& strShape /* =  */)
{
	_ASSERT(agent.m_pAgent==NULL);
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return false;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *shape = m_Shapes.GetShape(strShape);

	cPosition cPos;
	if ( PreProcessQuery(mesh, shape, position, cPos))
	{
		agent.m_pAgent = mesh->placeAgent(shape->m_pShape, cPos);
		if ( agent.m_pAgent )
		{
			agent.m_pAgent->setUserData(static_cast<void*>(&agent));
			agent.m_nMeshID = nID;

			_ASSERT(!mesh->testPointCollision(shape->m_pShape, NULL,cPos));
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
PATHENGINE_RESULT_TYPE MPathFinder::AdvancedAlongPath(MCollisionInfo *pCollisionInfoOut, int nID, MAgent &agent, MPath &path, float fDistance)
{
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

//////////////////////////////////////////////////////////////////////////
//

bool MPathFinder::FindPath(int nID, const MVector3 &vStart, const MVector3 &vEnd, MPath &outPath, MContext *pContext
						   ,const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return false;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);

	cPosition st, en;
	if (!PreProcessQuery(mesh, mshape, vStart, st))
		return 0.0f;
	if (!PreProcessQuery(mesh, mshape, vEnd, en))
		return 0.0f;

	outPath.Release();
	iCollisionContext *context = NULL;
	if ( pContext )
		context = pContext->m_pContext;

	outPath.m_pPath = mesh->findShortestPath(mshape->m_pShape, context, st, en);
	
	return outPath.IsValid();
}

//////////////////////////////////////////////////////////////////////////
//
bool MPathFinder::FindPath(MPath &outPath, int nID, const MAgent &agent, const MVector3 &vEnd, const std::string &strShape)
{
	if ( !agent.IsValid())
		return false;

	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return false;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *shape = m_Shapes.GetShape(strShape);

	_ASSERT(mesh);
	_ASSERT(shape);

	cPosition cPos;
	if ( PreProcessQuery(mesh, shape, vEnd, cPos))
	{
		iCollisionContext *context = NULL;
		if ( agent.IsInContext())
			context = agent.m_pContext->m_pContext;

		{
			vec3 vStartPos;
			agent.GetPosition(&vStartPos);

			// 무한루프가 되어서 임시로 막아놓음 - birdkr
			if (MMath::IsZero(vStartPos.x - vEnd.x) && MMath::IsZero(vStartPos.y - vEnd.y)) 
			{
				return false;
			}
		}

		iPath *path;
		
		path = agent.m_pAgent->findShortestPathTo(context, cPos);

		if ( path == NULL )
			return false;


		_ASSERT(outPath.m_pPath==NULL);
		outPath.m_pPath = path;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
bool MPathFinder::FindPath(MPath &outPath, int nID, const MAgent &agent, const MVector3 &vEnd, int nFreq, int nCount, const std::string &strShape)
{
	_ASSERT(agent.m_pAgent!=NULL);
	if (agent.m_pAgent == NULL) return false;

	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return false;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *shape = m_Shapes.GetShape(strShape);

	_ASSERT(mesh);
	_ASSERT(shape);

	cPosition cPos;
	if ( PreProcessQuery(mesh, shape, vEnd, cPos))
	{
		iCollisionContext *context = NULL;
		if ( agent.IsInContext())
			context = agent.m_pContext->m_pContext;

		iPath *path = agent.m_pAgent->findShortestPathTo_WithQueryCallBack(context, cPos,&m_QueryCallBack);
		if ( path == NULL )
			return false;

		_ASSERT(outPath.m_pPath==NULL);
		outPath.m_pPath = path;

		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
//

float MPathFinder::FindPathAway(int nID, const MVector3& vStart, const MVector3& vCenter, float fSize, vector<MVector3>& vPath, const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return 0.0f;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);

	vPath.clear();

	cPosition st, center;

	if (!PreProcessQuery(mesh, mshape, vStart, st))
		return 0.0f;
	if (!PreProcessQuery(mesh, mshape, vCenter, center))
		return 0.0f;

	iPath* p;
	p = mesh->findPathAway(mshape->m_pShape, 0, st, center, (int)fSize);

	float ret = 0.0f;
	if (p != NULL)
	{
		BuildPath(mesh, p, vPath, 1);
		ret = (float)p->getLength();

		delete p;
	}

	return ret;	
}



float MPathFinder::FindPath(int nID, const MVector3& vStart, const MVector3& vEnd, vector<MVector3>& vPath, const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return 0.0f;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);
	vPath.clear();

	cPosition st, en;

	if (!PreProcessQuery(mesh, mshape, vStart, st))
		return 0.0f;
	if (!PreProcessQuery(mesh, mshape, vEnd, en))
		return 0.0f;

	iPath* p;
	p = mesh->findShortestPath(mshape->m_pShape, 0, st, en);

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
//

float MPathFinder::FindPath(int nID, const MVector3& vStart, const MVector3& vEnd, vector<MVector3>& vPath, int nFreq, int nCount, const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return 0.0f;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);

	vPath.clear();

	cPosition st, en;

	if (!PreProcessQuery(mesh, mshape, vStart, st))
		return 0.0f;
	if (!PreProcessQuery(mesh, mshape, vEnd, en))
		return 0.0f;

	iPath* p;
	m_QueryCallBack.Set(nFreq, nCount);
	p =mesh->findShortestPath_WithQueryCallBack(mshape->m_pShape, 0, st, en, &m_QueryCallBack);

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
//

float MPathFinder::FindPathAway(int nID, const MVector3& vStart, const MVector3& vCenter, float fSize, int nFreq, int nCount, vector<MVector3>& vPath, const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return 0.f;
	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);
	vPath.clear();

	cPosition st, center;

	if (!PreProcessQuery(mesh, mshape, vStart, st))
		return 0.0f;
	if (!PreProcessQuery(mesh, mshape, vCenter, center))
		return 0.0f;

	iPath* p;
	m_QueryCallBack.Set(nFreq, nCount);
	p = mesh->findPathAway_WithQueryCallBack(mshape->m_pShape, 0, st, center, (int)fSize, &m_QueryCallBack);

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
//

void MPathFinder::PathTest(int nID, const MVector3& vStart, const MVector3& vEnd, FILE* fp, const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);

	if (mesh == NULL) return;
	if (mshape == NULL) return;

	iShape* shape = mshape->m_pShape;
	if (shape == NULL) return;

	cPosition st, en;

	st = mesh->positionFor3DPointF((const float*)&vStart);
	en = mesh->positionFor3DPointF((const float*)&vEnd);

	if (st.cell == -1) return;
	if (en.cell == -1) return;

	if (!mesh->positionIsValid(st)) return;
	if (!mesh->positionIsValid(en)) return;

	if (mesh->testPointCollision(shape,0,st)) return;
	if (mesh->testPointCollision(shape,0,en)) return;

	m_QueryCallBack.Set(5, 10);
	__int64 freq;
	QueryPerformanceFrequency((_LARGE_INTEGER*)&freq);
	

	__int64 t1, t2;
	QueryPerformanceCounter((_LARGE_INTEGER*)&t1); 
	iPath* p;
	int l = 10;
	for (int i=0; i<l; i++)
	{
//		p =mesh->findShortestPath(shape, 0, st, en);
		p =mesh->findShortestPath_WithQueryCallBack(shape, 0, st, en, &m_QueryCallBack);
		m_QueryCallBack.reset();
	}

	QueryPerformanceCounter((_LARGE_INTEGER*)&t2); 

	float t = (float)(((double)(t2 - t1) / (double)freq * 1000.0) / (float)l);

	int len = -1;
	if (p != NULL)
		len = p->getLength();
	fprintf(fp, "%d, %f, %d, %d, %d, %d\n", len, t, st.x, st.y, en.x, en.y);
	delete p;
}


PATHENGINE_RESULT_TYPE MPathFinder::FindFirstCollision(int nID, const MVector3& vStart, const MVector3& vEnd, MVector3& vRet, const string& strShape)
{
	SPathMesh *pPMesh = GetMesh(nID);
	if ( pPMesh == NULL )
		return INVALID_PARAMETER;

	iMesh *mesh = pPMesh->m_pMesh;
	MPathAgentShape *mshape = m_Shapes.GetShape(strShape);

	vRet = vEnd;

	cPosition st, en;

	if (!PreProcessQuery(mesh, mshape, vStart, st))
		return INVALID_POSITION;
	if (!PreProcessQuery(mesh, mshape, vEnd, en))
		return INVALID_POSITION;

	en.x = (int)vEnd.x;
	en.y = (int)vEnd.y;

	/**
	 충돌하는 점을 구하는게 아니라 충돌하는 선을 구한다.
	 ( 2D 선분교차 알고리즘으로 교점을 판별 )
	*/
	iCollisionInfo* pCollision = mesh->firstCollision(mshape->m_pShape, NULL, st, en.x, en.y, en.cell);
	if (pCollision != NULL)
	{
		tSigned32 pBuf[4];
		pCollision->getCollidingLine(pBuf);
		MVector3 vDir = (vEnd - vStart);

		MVector2 p;
		if (MMath::IntersectSegment2D(
			MVector2((float)st.x, (float)st.y)
			, MVector2((float)en.x, (float)en.y)
			, MVector2((float)pBuf[0], (float)pBuf[1])
			, MVector2((float)pBuf[2], (float)pBuf[3]), p) == 0)
		{
			return COLLISION;
		}

		MVector3 out = MVector3(p.x, p.y, vStart.z + 1000);

		cPosition cRet;
		cRet = mesh->positionFor3DPointF((const float*)&out);
		if (!mesh->positionIsValid(cRet))
			return INVALID_POSITION;

		vRet.x = p.x;
		vRet.y = p.y;
		vRet.z = mesh->heightAtPositionF(cRet);
		
		delete pCollision;
		return COLLISION;
	}

	return PATH_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//

void MPathFinder::ClearZone()
{
	for (size_t i=0; i<m_vZoneList.size(); i++)
	{
		delete m_vZoneList[i].m_pMesh;
		delete m_vZoneList[i].m_pTerrainCallback;
	}

	m_vZoneList.clear();
}

/////////////////////////////////////////////////////////////////////////////

void MPathFinder::PathTo3DPoint(MVector3 &outPos, int nID, MPath &basePath, int index)
{
	if ( basePath.IsValid())
	{
		iPath *path = basePath.m_pPath;
		iMesh *mesh = GetIMesh(nID);
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

void MPathFinder::PathTo3DPoints(std::vector<MVector3> &out3DPoints, int nID, MPath &basePath, int offset)
{
	if ( basePath.IsValid())
	{
		iPath *path = basePath.m_pPath;
		iMesh *mesh = GetIMesh(nID);
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

#if 0
PATHENGINE_RESULT_TYPE MPathFinder::LineCollisionTest(MVector3 *pOut, int nID, const MVector3 &vStart, const MVector3 &vEnd, const string& strShape)
{
	iMesh *mesh = GetIMesh(nID);
	if (mesh == NULL)
		return INVALID_PARAMETER;

	MPathAgentShape *pShape = m_Shapes.GetShape(strShape);
	if (pShape == NULL)
		return INVALID_PARAMETER;

	cPosition start, end;
	if (!PreProcessQuery(mesh, pShape, vStart, start, 0, 5))
		return INVALID_POSITION;
	if (!PreProcessQuery(mesh, pShape, vEnd, end, 0, 5))
		return INVALID_POSITION;

	bool bResult = mesh->testLineCollision(pShape->m_pShape, NULL, start, end);
	if ( bResult )
		return COLLISION;

	return PATH_SUCCESS;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Debug rendering function ( navigation mesh, obstacle )

int MPathFinder::GetStaticObstacleDebugMesh(int nID, std::vector<MVector3> &triangleArray, float fShapeHeight)
{
	iMesh *mesh = GetIMesh(nID);
	if (mesh == NULL)
		return 0;

	triangleArray.clear();
	
	long ObstacleNumber = mesh->getNumberOfBurntInObstacles();
	if ( ObstacleNumber )
	{
		for ( tSigned32 i = 0; i<ObstacleNumber; ++i )
		{
			cPosition cPos = mesh->getBurntInObstacleRoot(i);
			float fHeight = mesh->heightAtPositionF(cPos);

			MVector3 vPosition = MVector3(static_cast<float>(cPos.x), static_cast<float>(cPos.y), fHeight);
			tSigned32 numVertices = mesh->getBurntInObstacleVertices(i);
			_ASSERT(numVertices!=0);

			for ( tSigned32 j = 0; j<numVertices; ++j )
			{
				tSigned32 cx1, cy1;
				tSigned32 cx2, cy2;
				mesh->getBurntInObstacleVertex(i, j, cx1, cy1);
				mesh->getBurntInObstacleVertex(i, (j+1)%numVertices, cx2, cy2);

				// translation
				float fx1 = static_cast<float>(cx1 + cPos.x);
				float fx2 = static_cast<float>(cx2 + cPos.x);
				float fy1 = static_cast<float>(cy1 + cPos.y);
				float fy2 = static_cast<float>(cy2 + cPos.y);			

				// 2 triangles
				triangleArray.push_back(MVector3(fx1,fy1,fHeight));
				triangleArray.push_back(MVector3(fx1,fy1,fHeight+fShapeHeight));
				triangleArray.push_back(MVector3(fx2,fy2,fHeight));
				triangleArray.push_back(MVector3(fx2,fy2,fHeight));
				triangleArray.push_back(MVector3(fx1,fy1,fHeight+fShapeHeight));
				triangleArray.push_back(MVector3(fx2,fy2,fHeight+fShapeHeight));
			}
		}

		return (int)triangleArray.size();
	}

	return 0;
}

int MPathFinder::GetNavigationMeshFaceCount(int nID)
{
	iMesh *mesh = GetIMesh(nID);
	if ( mesh == NULL )
		return 0;

	return mesh->getNumberOf3DFaces();
}

int MPathFinder::GetNavigationMeshFaceList(int nID, MVector3 *pVertices, int nFaceCount)
{
	_ASSERT(pVertices);
	_ASSERT(nFaceCount>0);

	iMesh *mesh = GetIMesh(nID);
	if ( mesh == NULL )
		return 0;

	MVector3 *pV = pVertices;
	int i = 0;
	for ( i = 0; i<nFaceCount; ++i )
	{
		tSigned32 x, y;
		float z;

		mesh->get3DFaceVertex(i, 0, x, y, z);
		pV->x = static_cast<float>(x);
		pV->y = static_cast<float>(y);
		pV->z = z;
		++pV;

		mesh->get3DFaceVertex(i, 1, x, y, z);
		pV->x = static_cast<float>(x);
		pV->y = static_cast<float>(y);
		pV->z = z;
		++pV;

		mesh->get3DFaceVertex(i, 2, x, y, z);
		pV->x = static_cast<float>(x);
		pV->y = static_cast<float>(y);
		pV->z = z;
		++pV;
	}

	return i;
}