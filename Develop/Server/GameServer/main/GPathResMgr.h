#pragma once

#include "MPathFinder.h"
#include "MPathAgentShape.h"
#include <map>
#include "MVector3.h"


class GPathResMgr : public MTestMemPool<GPathResMgr>
{
private:
	iPathEngine*			m_pPathEngine;

	bool AddZone(int nID, const wstring& strFilename, const vector<wstring> &vShapes, RHeightField *pHeightField);
	bool LoadNavigationMeshFile( int nID, SPathMesh &outPathMesh, BYTE pcOutMD5String[16], const wstring &strFilename, RHeightField *pHeightField );
	bool LoadPreprocessFile( SPathMesh &pathMesh, const BYTE pcMD5String[16], const wstring &strFilename, const vector<wstring>& vShapes, bool bAutoGeneration );

	map<int, SPathMesh*>	m_mapPathMesh;
	MPathAgentShapeSet		m_Shapes;

	MPathAgentShape*		m_pDefaultShape;
public:
	GPathResMgr();
	~GPathResMgr();

	void SetPathEngine(iPathEngine* pPathEngine)
	{
		m_pPathEngine = pPathEngine;
	}

	bool AddZone(int nID, const wstring& strFilename, RHeightField *pHeightField);
	void ClearZone();
	void CreateDefaultShapes();


	SPathMesh* GetMesh(int id);
	iShape* GetDefaultShape();
	iShape* GetShape(const wstring& strShape);

	int GetDefaultShapeRange();
};