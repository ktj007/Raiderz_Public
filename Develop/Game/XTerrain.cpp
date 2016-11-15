#include "stdafx.h"
#include "XTerrain.h"
#include "RTerrainPatch.h"
#include "RTerrainPVS.h"

XTerrain::XTerrain() : RTerrain(), m_nRenderFlag(0)
{

}

XTerrain::XTerrain(RDevice *pDevice) : RTerrain(pDevice), m_nRenderFlag(0)
{

}

XTerrain::~XTerrain()
{

}

void XTerrain::OnRender()
{
	RTerrain::OnRender();	

	if ( m_nRenderFlag & FLAG_GRID )
	{
		RenderGrid();
	}
}

#include "RRenderHelper.h"

int XTerrain::RenderGrid()
{
	int sx, sy;
	sx = 100000 / SECTOR_SIZE;
	sy = 100000 / SECTOR_SIZE;

	RMatrix matWorld;
	matWorld.MakeIdentity();

	DWORD color = 0xFF0000FF;

	for (int i = 0; i < sy; i++)
	{
		RRenderHelper::RenderLine(matWorld, RVector(0, i*SECTOR_SIZE, 10), RVector(100000, i*SECTOR_SIZE, 10), color);
	}
	for (int j = 0; j < sx; j++)
	{
		RRenderHelper::RenderLine(matWorld, RVector(j*SECTOR_SIZE, 0, 10), RVector(j*SECTOR_SIZE, 100000, 10), color);
	}

	return 0;
}



// 어쩔 수 없이 RTerrain::CreateFromXML에서 복사해서 사용
XTerrain* XTerrain::CreateFromXML(RDevice *pDevice, const char *szFilename)
{
	MXml kXml;

	if ( !kXml.LoadFile(szFilename, pZFS))
	{
		OutputDebugString("Failed ... Terrain .XML File Load \n");
		return NULL;
	}

	// 엔진이랑 구현이 다름(MAPFILE) - bird
	MXmlElement *pRoot = kXml.Doc()->FirstChildElement("MAPFILE")->FirstChildElement("RTERRAIN");

	XTerrain *pTerrain = NULL;
	pTerrain = new XTerrain();
	pTerrain->m_pDevice = pDevice;

	char szPath[MAX_PATH];
	memset(szPath, 0, sizeof(szPath));
	char *p1 = strrchr(szFilename,'/');
	char *p2 = strrchr(szFilename,'\\');
	char *p = max(p1, p2);

	int len = 0;
	if ( p )
		len = p - szFilename;
	strncpy(szPath,szFilename,len+1);

	assert(pTerrain);
	pTerrain->SetPath(szPath);
	pTerrain->SetTilePath(szPath);
	bool bResult = pTerrain->LoadFromXMLElement(pRoot);
	assert(bResult);

	return pTerrain;
}

