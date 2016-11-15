#pragma once

#include "XEngine.h"
#include "RMeshMgr.h"
#include "RMesh.h"
#include <map>
#include <string>
using namespace std;

/// rs3 พ๎ด๐ลอ
class MockEngine : public XEngine
{
private:
	map<wstring, RMesh*>	m_MeshMap;
	RMesh* FindMesh(const wchar_t* szMeshName);
	void InsertMesh(const wchar_t* szMeshName, RMesh* pMesh);
	void EraseMesh(RMesh* pMesh);
public:
	MockEngine() {}
	virtual ~MockEngine();

	virtual void ReleaseMesh( RMesh* pMesh );
	virtual RMesh* CreateMesh( const wchar_t* szMeshName );
};