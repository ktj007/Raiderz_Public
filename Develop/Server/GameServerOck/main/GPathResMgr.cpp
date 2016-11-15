#include "stdafx.h"
#include "GPathResMgr.h"
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
#include "GPathResMgr.h"
#include "MLocale.h"

GPathResMgr::GPathResMgr() : m_pPathEngine(NULL), m_pDefaultShape(NULL)
{

}

GPathResMgr::~GPathResMgr()
{

}

SPathMesh* GPathResMgr::GetMesh( int id )
{
	map<int, SPathMesh*>::iterator itor = m_mapPathMesh.find(id);
	if (itor != m_mapPathMesh.end())
	{
		return (*itor).second;
	}

	return NULL;
}

bool GPathResMgr::LoadNavigationMeshFile( int nID, SPathMesh &outPathMesh, BYTE pcOutMD5String[16], const wstring &strFilename, RHeightField *pHeightField )
{
	MFile file;
	if ( !file.Open(MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str()))
	{
		mlog("Navigation %d - %s is not found\n", nID, MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str());
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
		md5.md5_string((BYTE*)pBuffer, fileSize, (BYTE*)pcOutMD5String);
	}

	outPathMesh.m_nID = nID;
	outPathMesh.m_pMesh = m_pPathEngine->loadMeshFromBuffer("tok", pBuffer, fileSize, 0);

	delete [] pBuffer;
	pBuffer = NULL;

	if ( outPathMesh.m_pMesh == NULL )
	{
		mlog("FatalError : Navigation mesh %d - %s is load failed\n", nID, MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str());
		return false;
	}

	if ( pHeightField )
	{
		outPathMesh.m_pTerrainCallback = new MPathTerrainCallback(pHeightField);
		outPathMesh.m_pMesh->setTerrainCallBack(0, outPathMesh.m_pTerrainCallback);
	}

	return true;
}

bool GPathResMgr::LoadPreprocessFile( SPathMesh &pathMesh, const BYTE pcMD5String[16], const wstring &strFilename, const vector<wstring>& vShapes, bool bAutoGeneration )
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
		MPathAgentShape* pShape = m_Shapes.GetShape(MLocale::ConvUTF16ToAnsi(vShapes[i].c_str()).c_str());
		_ASSERT(pShape);
		_ASSERT(pShape->m_pShape);

		// 충돌/길찾기 전처리 파일명
		wstring collisionfilename = strFilename + L"." + vShapes[i] + L".collision.pathengine";
		wstring pathfilename = strFilename + L"." + vShapes[i] + L".pathfind.pathengine";

		// parameter
		const char* pathfind_attributes[3];
		pathfind_attributes[0] = "split_with_circumference_below";
		pathfind_attributes[1] = "4000";
		pathfind_attributes[2] = 0;

		bool bResult = MPathFindPreprocess::LoadPreprocess(m_pPathEngine, pathMesh.m_pMesh, (const char*)pcMD5String, pShape->m_pShape, MLocale::ConvUTF16ToAnsi(pathfilename.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(collisionfilename.c_str()).c_str());

		if (!bResult)
		{
			mlog("%s shape preprocess file loading failed\n", MLocale::ConvUTF16ToAnsi(vShapes[i].c_str()).c_str());
			bAllResult = bAllResult && bResult;
			if ( bAutoGeneration )
			{
				MPathFindPreprocess::GenerateFile(pathMesh.m_pMesh, (const char*)pcMD5String, pShape->m_pShape, MLocale::ConvUTF16ToAnsi(pathfilename.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(collisionfilename.c_str()).c_str(), NULL, pathfind_attributes);
			}
		}
		else
		{
			pathMesh.m_vShapeName.push_back(MLocale::ConvUTF16ToAnsi(vShapes[i].c_str()).c_str());
		}
	}

	return bAllResult;
}

bool GPathResMgr::AddZone( int nID, const wstring& strFilename, const vector<wstring> &vShapes, RHeightField *pHeightField )
{
	if (GetMesh(nID) != NULL)
	{
		// 이미 있음
		return true;
	}

	wstring meshfilename = strFilename + L".navmesh.pathengine";

	SPathMesh* pNewPathMesh = new SPathMesh();

	BYTE pcMD5String[16];
	memset(pcMD5String, 0, sizeof(pcMD5String));

	if ( !LoadNavigationMeshFile(nID, *pNewPathMesh, pcMD5String, meshfilename, pHeightField))
	{
		SAFE_DELETE(pNewPathMesh->m_pTerrainCallback);
		SAFE_DELETE(pNewPathMesh);
		return false;
	}

	if ( !LoadPreprocessFile(*pNewPathMesh, pcMD5String, strFilename, vShapes, false))
	{
		SAFE_DELETE(pNewPathMesh->m_pTerrainCallback);
		SAFE_DELETE(pNewPathMesh);
		return false;
	}

	m_mapPathMesh.insert(map<int, SPathMesh*>::value_type(nID, pNewPathMesh));

	return true;
}

bool GPathResMgr::AddZone( int nID, const wstring& strFilename, RHeightField *pHeightField )
{
	vector<wstring> t;

	// 기본으로 small shape에 대해서만 추가한다.
	t.push_back(L"small");
	//	t.push_back(L"medium");
	//	t.push_back(L"large");

	return AddZone(nID, strFilename, t, pHeightField);
}
void GPathResMgr::ClearZone()
{
	for (map<int, SPathMesh*>::iterator itor = m_mapPathMesh.begin();
		itor != m_mapPathMesh.end(); ++itor)
	{
		SPathMesh* pPathMesh = (*itor).second;

		delete pPathMesh->m_pMesh;
		delete pPathMesh->m_pTerrainCallback;

		delete pPathMesh;
	}

	m_mapPathMesh.clear();
}

iShape* GPathResMgr::GetShape( const wstring& strShape )
{
	MPathAgentShape *pShape = m_Shapes.GetShape(MLocale::ConvUTF16ToAnsi(strShape.c_str()).c_str());
	if ( pShape == NULL )
		return NULL;

	return pShape->m_pShape;
}

iShape* GPathResMgr::GetDefaultShape()
{
	if (m_pDefaultShape) return m_pDefaultShape->m_pShape;

	return NULL;
}

void GPathResMgr::CreateDefaultShapes()
{
	m_Shapes.CreateDefaultShapes(m_pPathEngine);

	m_pDefaultShape = m_Shapes.GetDefaultShape();
}

int GPathResMgr::GetDefaultShapeRange()
{
	MPathAgentShape *pShape = m_Shapes.GetDefaultShape();
	if ( pShape == NULL )
		return 0;

	return pShape->m_nSize;
}