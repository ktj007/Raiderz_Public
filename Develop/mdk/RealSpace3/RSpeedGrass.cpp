#include "stdafx.h"
#include "RSpeedGrass.h"
#include "RShaderFX.h"
#include "RSceneManager.h"
#include "RDevice.h"
#include "RCameraSceneNode.h"
#include "RTerrain.h"

#include <list>

namespace rs3
{


RSpeedGrass::RSpeedGrass(RDevice* dev) 
: CSpeedGrassRT()
, m_pDevice(dev)
, m_vWindDirection(RVector(1, 0, 0))
, m_fWindStrength(1.0f)
, m_vAmbientColor(0, 0, 0, 1)
, m_vDiffuseColor(1, 1, 1, 1)
, m_nGrassCountLimitPerRegion(-1)
, m_fGrassJigglingPeriod(0.0f)
, m_fGrassJigglingStrength(0.0f)
, m_fGrassIncliningStrength(0.0f)

{
	CSpeedGrassRT::SetWindDirection((float*)&m_vWindDirection);
	// 초기화
	for ( UINT ui = 0; ui<GRASS_MAX_NUM; ++ui )
		m_pGrassTable[ui] = NULL;
}

RSpeedGrass::~RSpeedGrass()
{
	for ( UINT ui = 0; ui<GRASS_MAX_NUM; ++ui )
		SAFE_DELETE(m_pGrassTable[ui]);
}

void RSpeedGrass::Destroy()
{
	for ( vector<SRegionData>::iterator it = m_vRegionDatas.begin();
		it != m_vRegionDatas.end(); ++it )
	{
		m_pDevice->DeleteVertexBuffer( it->hVertexBuffer );
		m_pDevice->DeleteIndexBuffer ( it->hIndexBuffer );

		it->nVertexBufferSize = 0;
		it->nIndexBufferSize = 0;
		it->nPrimitiveNum = 0;
		it->nVertexNum = 0;
	}	

	m_vRegionDatas.clear();
	m_vRegions.clear();
}

void RSpeedGrass::GetGrassVertex(SBlade& b, vector<SGrassVertex>& vert, vector<WORD>& idx, int idxoff, RTerrain* pTerrain)
{
	SGrassVertex v[4];
	int nIndex = b.m_nWhichTexture;
	_ASSERT(nIndex>=0);
	SCompositeMapInfo *pInfo = m_pGrassTable[nIndex];
	_ASSERT(pInfo!=NULL);

	float tWid = pInfo->fWidth;
	float tHei = pInfo->fHeight;
	_ASSERT(tWid!=0.0f);
	_ASSERT(tHei!=0.0f);

	float woffset = (1.0f/m_nCompTexWidth);
	float hoffset = (1.0f/m_nCompTexHeight);

	float tx = pInfo->fUOffset;
	float ty = pInfo->fVOffset;

	tx += woffset*0.5f;
	ty += hoffset*0.5f;
	tWid -= woffset*0.5f;
	tHei -= hoffset*0.5f;

	float fWSize = b.m_fSize * (GetGrassWidth(nIndex)/256.0f);
	float fHSize = abs(b.m_fSize) * (GetGrassHeight(nIndex)/256.0f);

// 	// 피킹하여 노말을 얻음
// 	RPICKINFO pickInfo;
// 	RVector vPos = RVector( b.m_afPos[0], b.m_afPos[1], b.m_afPos[2]);
// 	pickInfo.inPickOrigin = vPos + RVector(0,0,300000);
// 	pickInfo.inPickTo = vPos + RVector(0,0,-30000);
// 	pickInfo.outNormal = RVector( 0,0,1);
// 	// 씬 전체를 타겟으로 하면 너무 느리다. 터레인만 타겟으로.
// 	if( pTerrain)
// 	{
// 		pTerrain->OnPick( pickInfo);
// 		// 노말라이즈!!!
// 		pickInfo.outNormal = pickInfo.outNormal.Normalize();
// 	}

	for (int nCorner = 0; nCorner < 4; ++nCorner)
	{
		// texcoord 0
		switch (nCorner)
		{
		case 0:
			v[nCorner].Tex0[0] = tx;
			v[nCorner].Tex0[1] = ty;
			break;
		case 1:
			v[nCorner].Tex0[0] = tx + tWid;
			v[nCorner].Tex0[1] = ty;
			break;
		case 2:
			v[nCorner].Tex0[0] = tx + tWid;
			v[nCorner].Tex0[1] = ty + tHei;
			break;
		case 3:
			v[nCorner].Tex0[0] = tx;
			v[nCorner].Tex0[1] = ty + tHei;
			break;
		default:
			_ASSERT(!"?!ERROR");
		}

		// texcoord 1
		v[nCorner].Tex1[0] = (float)nCorner;
		if (nCorner < 2)
		{
			v[nCorner].Tex1[1] = b.m_fThrow;
		}
		else
			v[nCorner].Tex1[1] = 0.0f;
		
		v[nCorner].Tex1[2] = fWSize;	// Grass Width
		v[nCorner].Tex1[3] = fHSize;	// Grass Height

		// color
		unsigned char aucColor[4];
		if (nCorner == 0 || nCorner == 1)
		{
			aucColor[0] = b.m_dwTopColor >> 16 & 0xff;
			aucColor[1] = b.m_dwTopColor >> 8 & 0xff;
			aucColor[2] = b.m_dwTopColor & 0xff;
			aucColor[3] = b.m_dwTopColor >> 24 & 0xff;
		}
		else
		{
			aucColor[0] = b.m_dwBottomColor >> 16 & 0xff;
			aucColor[1] = b.m_dwBottomColor >> 8 & 0xff;
			aucColor[2] = b.m_dwBottomColor & 0xff;
			aucColor[3] = b.m_dwBottomColor >> 24 & 0xff;
		}

		v[nCorner].Color[0] = aucColor[0] / 255.0f;
		v[nCorner].Color[1] = aucColor[1] / 255.0f;
		v[nCorner].Color[2] = aucColor[2] / 255.0f;
		v[nCorner].Color[3] = aucColor[3] / 255.0f;

		v[nCorner].Position[0] = b.m_afPos[0];
		v[nCorner].Position[1] = b.m_afPos[1];
		v[nCorner].Position[2] = b.m_afPos[2];

		v[nCorner].Normal[0] = b.m_afNorm[0];
		v[nCorner].Normal[1] = b.m_afNorm[1];
		v[nCorner].Normal[2] = b.m_afNorm[2];

// 		// 노말 정보를 버텍스에 그대로 적용
// 		v[nCorner].Normal[0] = pickInfo.outNormal.x;
// 		v[nCorner].Normal[1] = pickInfo.outNormal.y;
// 		v[nCorner].Normal[2] = pickInfo.outNormal.z;
	}

	vert.push_back(v[0]);
	vert.push_back(v[1]);
	vert.push_back(v[2]);
	vert.push_back(v[3]);

	idx.push_back(idxoff + 0);
	idx.push_back(idxoff + 1);
	idx.push_back(idxoff + 2);
	idx.push_back(idxoff + 0);
	idx.push_back(idxoff + 2);
	idx.push_back(idxoff + 3);
}

void RSpeedGrass::CreateRegions(int row, int col, float fWidth, float fHeight)
{
	m_nNumRegionCols = col;
	m_nNumRegionRows = row;

	// create regions based on overall extents
	RSpeedGrass::Destroy();
	for (int i=0; i<m_nNumRegionRows * m_nNumRegionCols; i++)
	{
		m_vRegions.push_back(SRegion());
		m_vRegionDatas.push_back(SRegionData());
	}

	// run through all regions, computing extents for each
	float fCellWidth = fWidth / m_nNumRegionCols;
	float fCellHeight = fHeight / m_nNumRegionRows;

	float fY = 0;
	for (int nRow = 0; nRow < m_nNumRegionRows; ++nRow)
	{
		float fX = 0;
		for (int nCol = 0; nCol < m_nNumRegionCols; ++nCol)
		{
			int nIndex = GetRegionIndex(nRow, nCol);
			SRegion* pRegion = &m_vRegions[nIndex];

			// compute extents
			pRegion->m_afMin[0] = fX;
			pRegion->m_afMax[0] = fX + fCellWidth;
			pRegion->m_afMin[1] = fY;
			pRegion->m_afMax[1] = fY + fCellHeight;
			pRegion->m_afMin[2] = FLT_MAX;
			pRegion->m_afMax[2] =-FLT_MAX;

			// compute center
			pRegion->m_afCenter[0] = 0.5f * (pRegion->m_afMin[0] + pRegion->m_afMax[0]);
			pRegion->m_afCenter[1] = 0.5f * (pRegion->m_afMin[1] + pRegion->m_afMax[1]);

			// compute culling radius
			pRegion->m_fCullingRadius = 1.1f * sqrt(
				((pRegion->m_afMax[0] - pRegion->m_afCenter[0]) * (pRegion->m_afMax[0] - pRegion->m_afCenter[0])) +
				((pRegion->m_afMax[1] - pRegion->m_afCenter[1]) * (pRegion->m_afMax[1] - pRegion->m_afCenter[1]))
				);

			fX += fCellWidth;
		}

		fY += fCellHeight;
	}

	m_afBoundingBox[0] = 0;
	m_afBoundingBox[1] = 0;
	m_afBoundingBox[2] = -10000;

	m_afBoundingBox[3] = fWidth;
	m_afBoundingBox[4] = fHeight;
	m_afBoundingBox[5] = +10000;
}

bool RSpeedGrass::AddGrass(const SBlade& b)	// 결과를 리턴하게 변경 - 090610, OZ
{
	float fPercentAlongX = (b.m_afPos[0] - m_afBoundingBox[0]) / (m_afBoundingBox[3] - m_afBoundingBox[0]);
	float fPercentAlongY = (b.m_afPos[1] - m_afBoundingBox[1]) / (m_afBoundingBox[4] - m_afBoundingBox[1]);

	// clip values
	unsigned int uiCol = (unsigned int)min(fPercentAlongX * m_nNumRegionCols, m_nNumRegionCols - 1);
	unsigned int uiRow = (unsigned int)min(fPercentAlongY * m_nNumRegionRows, m_nNumRegionRows - 1);

	int rIdx = GetRegionIndex(uiRow, uiCol);
	SRegion& nRegion = m_vRegions[rIdx];
	// 인덱스 버퍼의 자료 구조를 넘지 않게 갯수를 제한한다.
	if( m_nGrassCountLimitPerRegion == -1 || (int)nRegion.m_vBlades.size() < m_nGrassCountLimitPerRegion)
	{
		nRegion.m_vBlades.push_back(b);
		return true;
	}
	else
		return false;
}

float RSpeedGrass::GetDistSq(float x1, float y1, float x2, float y2)
{
	return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}


void RSpeedGrass::DeleteGrass(float x, float y, float fRadius)
{
	float RadSq = fRadius * fRadius;
	for (int i = 0; i < (int)m_vRegions.size( ); ++i)
	{
		SRegion &nRegion = m_vRegions[i];
		
		float fx = nRegion.m_afCenter[0] - x;
		float fy = nRegion.m_afCenter[1] - y;
		float flen = sqrtf(fx*fx + fy*fy);

		if ( flen < nRegion.m_fCullingRadius + fRadius )
		{
			std::vector<SBlade>::iterator it;
			for ( it = nRegion.m_vBlades.begin(); it != nRegion.m_vBlades.end(); )
			{
				SBlade &b = (*it);
				if ( GetDistSq( b.m_afPos[0], b.m_afPos[1], x, y) < RadSq )
					it = nRegion.m_vBlades.erase(it);
				else
					++it;
			}
		}
	}
}

void RSpeedGrass::SetWindDirection(const RVector& windDir)
{
	m_vWindDirection = windDir;
	m_vWindDirection.Normalize();
	CSpeedGrassRT::SetWindDirection((float*)&m_vWindDirection);
}

void RSpeedGrass::SetWindStrength(float windStr)
{
	m_fWindStrength = windStr;
}

int RSpeedGrass::GrassTypeCount()
{
	int nCount = 0;
	for ( UINT ui = 0; ui<GRASS_MAX_NUM; ++ui )
		if ( m_pGrassTable[ui] )
			++nCount;

	return nCount;
}

void RSpeedGrass::UpdateGrass()
{
	for (int i = 0; i < (int)m_vRegions.size( ); ++i)
	{
		SRegion &nRegion = m_vRegions[i];
		
		std::vector<SBlade>::iterator it;
		for ( it = nRegion.m_vBlades.begin(); it != nRegion.m_vBlades.end(); )
		{
			SBlade &b = (*it);
			int nIndex = b.m_nWhichTexture;
			
			if ( m_pGrassTable[nIndex] == NULL )
			{
				it = nRegion.m_vBlades.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void RSpeedGrass::LoadGrass(MXmlElement &xmlElement, const char *szNameA, const char *szNameB)	// 추가 / 삭제
{
	_ASSERT(m_pGrassTable!=NULL);

	list<SCompositeMapInfo> listInfo;

	MXmlElement *pChildElement = xmlElement.FirstChildElement("GRASSTEXTURE");
	for ( ; pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
	{
		SCompositeMapInfo kInfo;
		_Contents(kInfo.strFilename, pChildElement);

		kInfo.fUOffset = atof(pChildElement->Attribute("uoffset"));
		kInfo.fVOffset = atof(pChildElement->Attribute("voffset"));
		kInfo.fWidth = atof(pChildElement->Attribute("uwidth"));
		kInfo.fHeight = atof(pChildElement->Attribute("vheight"));

		listInfo.push_back(kInfo);
	}

	list<SCompositeMapInfo>::iterator it;
	for ( UINT ui = 0; ui<GRASS_MAX_NUM; ++ui )
	{
		if ( m_pGrassTable[ui] == NULL )
			continue;

		m_pGrassTable[ui]->fWidth = 0.0f;
		m_pGrassTable[ui]->fHeight = 0.0f;

		string &str = m_pGrassTable[ui]->strFilename;

		// 우선 같은 것을 검색하여ㅡ, 목록에서 삭제한다. 그렇
		for ( it = listInfo.begin(); it != listInfo.end() ; )
		{
			if ( it->strFilename == str )	// 같으면, 전부 대입 ㅡ 갱신
			{
				*m_pGrassTable[ui] = (SCompositeMapInfo)(*it);
				it = listInfo.erase(it);
				break;
			}
			else if ( szNameA && szNameB )
			{
				if ( it->strFilename == szNameB	// 변경
					&& str == szNameA )
				{
					*m_pGrassTable[ui] = (SCompositeMapInfo)(*it);
					it = listInfo.erase(it);
					break;
				}
			}
			++it;
		}
	}
	// 끝나면
	// m_pGrassTable[ui]->fWidth & fHeight 가 0.0f가 있는 것은 삭제 대상
	// listInfo에 남아 있는 것은 추가 대상
	
	// 삭제
	it = listInfo.begin();
	for ( UINT ui = 0; ui<GRASS_MAX_NUM; ++ui )
	{
		if ( m_pGrassTable[ui] )	// 삭제
		{
			if ( m_pGrassTable[ui]->fWidth == 0.0f 
				|| m_pGrassTable[ui]->fHeight == 0.0f )
			{
				delete m_pGrassTable[ui];
				m_pGrassTable[ui] = NULL;
			}
		}
		else // 추가를 한다.
		{
			if ( it != listInfo.end())
			{
				m_pGrassTable[ui] = new SCompositeMapInfo();
				*m_pGrassTable[ui] = (SCompositeMapInfo)(*it);
				++it;
			}
		}
	}
	
	// 끝
	return;
}

int RSpeedGrass::GetGrassID(const char *szTextureFilename)
{
	for ( UINT ui = 0; ui<GRASS_MAX_NUM; ++ui )
	{
		if ( m_pGrassTable[ui] )
			if ( m_pGrassTable[ui]->strFilename == szTextureFilename )
				return (int)ui;
	}

	return -1;
}

int RSpeedGrass::GetGrassWidth(int nIndex)
{
	_ASSERT(nIndex>=0);
	_ASSERT(m_pGrassTable[nIndex]!=NULL);

	return m_pGrassTable[nIndex]->fWidth * m_nCompTexWidth;
}

int RSpeedGrass::GetGrassHeight(int nIndex)
{
	_ASSERT(nIndex>=0);
	_ASSERT(m_pGrassTable[nIndex]!=NULL);

	return m_pGrassTable[nIndex]->fHeight * m_nCompTexHeight;
}

void RSpeedGrass::SetAmbientColor(rs3::RVector4 &vColor)
{
	m_vAmbientColor = vColor;
}

RVector4 RSpeedGrass::GetAmbientColor()
{
	return m_vAmbientColor;
}

void RSpeedGrass::SetDiffuseColor(RVector4 &vColor)
{
	m_vDiffuseColor = vColor;
}

RVector4 RSpeedGrass::GetDiffuseColor()
{
	return m_vDiffuseColor;
}

};