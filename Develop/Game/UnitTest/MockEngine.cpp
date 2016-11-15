#include "stdafx.h"
#include "MockEngine.h"
#include "MockMesh.h"

void MockEngine::ReleaseMesh( RMesh* pMesh )
{
	pMesh->Release();

	if (pMesh->GetRefCount() <= 0)
	{
		EraseMesh(pMesh);
		delete pMesh;
	}
}

RMesh* MockEngine::CreateMesh( const wchar_t* szMeshName )
{
	USES_CONVERSION_EX;

	RMesh* pMesh = FindMesh(szMeshName);
	if (pMesh) 
	{
		pMesh->AddRef();
		return pMesh;
	}

	RMesh* pNewMesh = new MockMesh(W2A_EX(szMeshName, 0));

	InsertMesh(szMeshName, pNewMesh);

	return pNewMesh;
}

RMesh* MockEngine::FindMesh( const wchar_t* szMeshName )
{
	map<wstring, RMesh*>::iterator itor = m_MeshMap.find(wstring(szMeshName));
	if (itor == m_MeshMap.end()) return NULL;
	return (*itor).second;
}

void MockEngine::InsertMesh( const wchar_t* szMeshName, RMesh* pMesh )
{
	m_MeshMap.insert(map<wstring, RMesh*>::value_type(wstring(szMeshName), pMesh));
}

void MockEngine::EraseMesh( RMesh* pMesh )
{
	for (map<wstring, RMesh*>::iterator itor = m_MeshMap.begin(); itor != m_MeshMap.end(); ++itor)
	{
		RMesh* pThisMesh = (*itor).second;
		if (pMesh == pThisMesh)
		{
			m_MeshMap.erase(itor);
			return;
		}
	}
}

MockEngine::~MockEngine()
{
	for (map<wstring, RMesh*>::iterator itor = m_MeshMap.begin(); itor != m_MeshMap.end(); ++itor)
	{
		RMesh* pThisMesh = (*itor).second;
		delete pThisMesh;
	}

	m_MeshMap.clear();
}