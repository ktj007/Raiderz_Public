///////////////////////////////////////////////////////////////////////
//	CSpeedGrassRT Class
//
//	(c) 2003 IDV, Inc.
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization and may
//	not be copied or disclosed except in accordance with the terms of
//	that agreement.
//
//      Copyright (c) 2001-2003 IDV, Inc.
//      All Rights Reserved.
//
//		IDV, Inc.
//		1233 Washington St. Suite 610
//		Columbia, SC 29201
//		Voice: (803) 799-1699
//		Fax:   (803) 931-0320
//		Web:   http://www.idvinc.com


//#pragma warning (disable : 4786) // stl name length restriction
#include "stdafx.h"
#include "SpeedGrassRT.h"
#include <cstring>
#include <cfloat>
#include "IdvVectorMath.h"

using namespace std;
using namespace idv;

//namespace rs3
//{


#define	INTERPOLATE(a, b, p) (((b) - (a)) * (p) + (a))
#define DEG2RAD(a) ((a) / 57.295779513f)
#define RAD2DEG(a) ((a) * 57.295779513f)

#define c_fPi 3.141592653f
#define c_fHalfPi (c_fPi * 0.5f)

// static variables
float CSpeedGrassRT::m_fLodFarDistance = 100.0f;
float CSpeedGrassRT::m_fLodTransitionLength = 37.5f;
float CSpeedGrassRT::m_afUnitBillboard[16] = { 0.0f, 0.0f, 0.0f, 1.0f,
												0.0f, 0.0f, 0.0f, 1.0f,
												0.0f, 0.0f, 0.0f, 1.0f,
												0.0f, 0.0f, 0.0f, 1.0f };
float CSpeedGrassRT::m_afWindDir[4] = { 1.0f, 0.3f, 0.0f, 0.0f };

// camera
float CSpeedGrassRT::m_afCameraOut[3] = { 0.0f, 1.0f, 0.0f };
float CSpeedGrassRT::m_afCameraUp[3] = { 0.0f, 0.0f, 1.0f };
float CSpeedGrassRT::m_afCameraRight[3] = { 1.0f, 0.0f, 0.0f };
float CSpeedGrassRT::m_afCameraPos[3] = { 0.0f, 0.0f, 0.0f };
float CSpeedGrassRT::m_fFieldOfView = DEG2RAD(40.0f);
float CSpeedGrassRT::m_fAspectRatio = 4.0f / 3.0f;

// culling
float CSpeedGrassRT::m_afFrustumBox[6] = { 0.0f };
float CSpeedGrassRT::m_afFrustumMin[2] = { FLT_MAX, FLT_MAX };
float CSpeedGrassRT::m_afFrustumMax[2] = { -FLT_MAX, -FLT_MAX };
float CSpeedGrassRT::m_afFrustumPlanes[5][4] = { 0.0f };

float CSpeedGrassRT::NOISE = 15.f;
float CSpeedGrassRT::THROW = 1.0f;

///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SBlade::SBlade

void D3DColorToFloat3(MVector3 *pOut, D3DCOLOR dwColor)
{
	D3DCOLOR dwR = ( dwColor >> 16 ) & 0xff;
	D3DCOLOR dwG = ( dwColor >> 8 ) & 0xff;
	D3DCOLOR dwB = ( dwColor ) & 0xff;

	if ( pOut )
	{
		pOut->x = (float)(dwR/255.0f);
		pOut->y = (float)(dwG/255.0f);
		pOut->z = (float)(dwB/255.0f);
	}
}

CSpeedGrassRT::SBlade::SBlade( ) :
  m_fWSize(1.0f)
, m_fHSize(1.0f)
, m_fSize(1.0f)
, m_nWhichTexture(-1)
, m_fNoise(0.0f)
, m_fThrow(0.0f)
{
	m_dwBottomColor = D3DCOLOR_XRGB(255, 255, 255);
	m_dwTopColor = D3DCOLOR_XRGB(255, 255, 255);
}

/**
	Pack Set, UnPack Get
 */

void CSpeedGrassRT::SPackedBlade3::Pack(const SBlade& b)
{
	m_afPos[0] = b.m_afPos[0];
	m_afPos[1] = b.m_afPos[1];
	m_afPos[2] = b.m_afPos[2];
	m_nWhichTexture = b.m_nWhichTexture;
	m_fWSize = b.m_fWSize;
	m_fHSize = b.m_fHSize;
	m_fThrow = b.m_fThrow;
	m_fNoise = b.m_fNoise;
}

void CSpeedGrassRT::SPackedBlade3::Unpack(SBlade& b)
{
	b.m_afPos[0] = m_afPos[0];
	b.m_afPos[1] = m_afPos[1];
	b.m_afPos[2] = m_afPos[2];
	b.m_nWhichTexture = m_nWhichTexture;

	b.m_fWSize = m_fWSize;
	b.m_fHSize = m_fHSize;
	b.m_fThrow = m_fThrow;
	b.m_fNoise = m_fNoise;
	b.m_dwBottomColor = D3DCOLOR_XRGB(255, 255, 255);
	b.m_dwTopColor = D3DCOLOR_XRGB(255, 255, 255);
}

void CSpeedGrassRT::SPackedBlade4::Pack(const SBlade& b)
{
	m_packedBlade3.Pack(b);
	
	m_fSize = b.m_fSize;

	MVector3 color3;
	D3DColorToFloat3(&color3, b.m_dwTopColor);
	m_afTopColor[0] = color3.x;
	m_afTopColor[1] = color3.y;
	m_afTopColor[2] = color3.z;

	D3DColorToFloat3(&color3, b.m_dwBottomColor);
	m_afBottomColor[0] = color3.x;
	m_afBottomColor[1] = color3.y;
	m_afBottomColor[2] = color3.z;
}

void CSpeedGrassRT::SPackedBlade4::Unpack(SBlade& b)
{
	m_packedBlade3.Unpack(b);

	b.m_fSize = m_fSize;
	b.m_dwTopColor = D3DCOLOR_COLORVALUE(m_afTopColor[0], m_afTopColor[1], m_afTopColor[2], 1.0f);
	b.m_dwBottomColor = D3DCOLOR_COLORVALUE(m_afBottomColor[0], m_afBottomColor[1], m_afBottomColor[2], 1.0f);
}

void CSpeedGrassRT::SPackedBlade5::Pack(const SBlade& b)
{
	m_afPos[0] = b.m_afPos[0];
	m_afPos[1] = b.m_afPos[1];
	m_afPos[2] = b.m_afPos[2];
	
	m_nWhichTexture = b.m_nWhichTexture;
	m_fSize = b.m_fSize;
	m_dwBottomColor = b.m_dwBottomColor;
	m_dwTopColor = b.m_dwTopColor;
}

void CSpeedGrassRT::SPackedBlade5::Unpack(SBlade &b)
{
	b.m_afPos[0] = m_afPos[0];
	b.m_afPos[1] = m_afPos[1];
	b.m_afPos[2] = m_afPos[2];

	b.m_nWhichTexture = m_nWhichTexture;

	b.m_fSize = m_fSize;
	b.m_fWSize = 0.0f;

	b.m_fNoise = NOISE;
	b.m_fThrow = THROW;

	b.m_dwBottomColor = m_dwBottomColor;
	b.m_dwTopColor = m_dwTopColor;

	return;
}


void CSpeedGrassRT::SPackedBlade6::Pack(const SBlade& b)
{
	m_packedBlade5.Pack(b);

	m_afNorm[0] = b.m_afNorm[0];
	m_afNorm[1] = b.m_afNorm[1];
	m_afNorm[2] = b.m_afNorm[2];
}

void CSpeedGrassRT::SPackedBlade6::Unpack(SBlade &b)
{
	m_packedBlade5.Unpack(b);

	b.m_afNorm[0] = m_afNorm[0];
	b.m_afNorm[1] = m_afNorm[1];
	b.m_afNorm[2] = m_afNorm[2];

	return;
}

///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SRegion::SRegion

CSpeedGrassRT::SRegion::SRegion( ) :
	m_bCulled(false),
	m_fCullingRadius(1.0f)
	//m_pGeomBuffer(new RTreeMeshBuffer<SGrassVertex>(RGetDevice(), BT_INDEXEDTRIANGLELIST))
{
	m_afCenter[0] = m_afCenter[1] = m_afCenter[2] = 0.5f;
	m_afMin[0] = m_afMin[1] = m_afMin[2] = 0.0f;
	m_afMax[0] = m_afMax[1] = m_afMax[2] = 1.0f;
}


CSpeedGrassRT::SRegion::SRegion(const SRegion& rhs) :
	m_bCulled(rhs.m_bCulled),
	m_fCullingRadius(rhs.m_fCullingRadius)
	//m_pGeomBuffer(new RTreeMeshBuffer<SGrassVertex>(RGetDevice(), BT_INDEXEDTRIANGLELIST))
{
	m_afCenter[0] = m_afCenter[1] = m_afCenter[2] = 0.5f;
	m_afMin[0] = m_afMin[1] = m_afMin[2] = 0.0f;
	m_afMax[0] = m_afMax[1] = m_afMax[2] = 1.0f;
}
///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SRegion::~SRegion

CSpeedGrassRT::SRegion::~SRegion( )
{
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::CSpeedGrassRT

CSpeedGrassRT::CSpeedGrassRT( ) :
	m_nNumRegionCols(0),
	m_nNumRegionRows(0),
	m_bAllRegionsCulled(false)
{
	m_afBoundingBox[0] = m_afBoundingBox[1] = m_afBoundingBox[2] = 0.0f;
	m_afBoundingBox[3] = m_afBoundingBox[4] = m_afBoundingBox[5] = 1.0f;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::~CSpeedGrassRT

CSpeedGrassRT::~CSpeedGrassRT( )
{
	
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::GetRegions

CSpeedGrassRT::SRegion* CSpeedGrassRT::GetRegions(unsigned int& uiNumRegions)
{
	uiNumRegions = (unsigned int)m_vRegions.size( );
	return &m_vRegions[0];
}



///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::CustomPlacement
//
//	Use this function to perform custom grass blade placement.  Feel free
//	to add parameters as necessary but be sure to call CreateRegions( )
//	at the end of the function to set up the SpeedGrass region system.
/**
bool CSpeedGrassRT::CustomPlacement(unsigned int uiRows, unsigned int uiCols)
{
	// copy region settings (do not remove)
	m_nNumRegionCols = int(uiCols);
	m_nNumRegionRows = int(uiRows);

	// initialize bounding box (do not remove)
	m_afBoundingBox[0] = m_afBoundingBox[1] = m_afBoundingBox[2] = FLT_MAX;
	m_afBoundingBox[3] = m_afBoundingBox[4] = m_afBoundingBox[5] = -FLT_MAX;

	// place one blade as an example
	vector<SBlade> vSceneBlades;

	SBlade sBlade;

	sBlade.m_afPos[0] = 0.0f;
	sBlade.m_afPos[1] = 0.0f;
	sBlade.m_afPos[2] = 0.0f;

	sBlade.m_afNormal[0] = 0.0f;
	sBlade.m_afNormal[1] = 0.0f;
	sBlade.m_afNormal[2] = 1.0f;

	// check against overall scene bounding box (always do this)
	for (int nAxis = 0; nAxis < 3; ++nAxis)
	{
		m_afBoundingBox[nAxis] = min(m_afBoundingBox[nAxis], sBlade.m_afPos[nAxis]);
		m_afBoundingBox[nAxis + 3] = max(m_afBoundingBox[nAxis + 3], sBlade.m_afPos[nAxis]);
	}

	// set bottom and top color
	memcpy(sBlade.m_afBottomColor, sBlade.m_afNormal, 12);
	memcpy(sBlade.m_afTopColor, sBlade.m_afNormal, 12);

	// assign which blade texture map
	sBlade.m_ucWhichTexture = (unsigned char)RandomNumber(0.0f, c_nNumBladeMaps - 1.0f);

	// compute wind effects
	sBlade.m_fNoise = RandomNumber(c_fMinBladeNoise, c_fMaxBladeNoise);
	sBlade.m_fThrow = RandomNumber(c_fMinBladeThrow, c_fMaxBladeThrow);

	// compute dimensions
	sBlade.m_fSize = RandomNumber(c_fMinBladeSize, c_fMaxBladeSize);

	// store all blades together
	vSceneBlades.push_back(sBlade);

	// create regions based on blades (do not remove)
	CreateRegions(vSceneBlades);

	// true = success, false = error
	return true;
}
*/

///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::GetLodParams
 
void CSpeedGrassRT::GetLodParams(float& fFarDistance, float& fTransitionLength)
{
	fFarDistance = m_fLodFarDistance;
	fTransitionLength = m_fLodTransitionLength;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SetLodParams

void CSpeedGrassRT::SetLodParams(float fFarDistance, float fTransitionLength)
{
	m_fLodFarDistance = fFarDistance;
	m_fLodTransitionLength = fTransitionLength;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::Cull
//
//	Using a two-dimensional projection, determine which regions
//	intersect with the view frustum (+Z is assumed to be up)

void CSpeedGrassRT::Cull(void)
{
	RPFC_THISFUNC;
	// convert raw frustum min and max values into min and max region cell indices
	int anFrustumCellsMin[2], anFrustumCellsMax[2];
	ConvertCoordsToCell(m_afFrustumMin, anFrustumCellsMin);
	ConvertCoordsToCell(m_afFrustumMax, anFrustumCellsMax);

	// set all regions to culled, modify later
	for (int i = 0; i < (int)m_vRegions.size( ); ++i)
		m_vRegions[i].m_bCulled = true;
	
	//int nRegionsDrawn = 0;

	// is the entire set of regions culled?
	if ((anFrustumCellsMin[0] < 0 && anFrustumCellsMax[0] < 0) ||
		(anFrustumCellsMin[0] >= m_nNumRegionCols && anFrustumCellsMax[0] >= m_nNumRegionCols) ||
		(anFrustumCellsMin[1] < 0 && anFrustumCellsMax[1] < 0) ||
		(anFrustumCellsMin[1] >= m_nNumRegionRows && anFrustumCellsMax[1] >= m_nNumRegionRows))
		m_bAllRegionsCulled = true;
	else
	{
		// clip cell values
		anFrustumCellsMin[0] = max(anFrustumCellsMin[0], 0);
		anFrustumCellsMin[1] = max(anFrustumCellsMin[1], 0);
		anFrustumCellsMax[0] = min(anFrustumCellsMax[0], m_nNumRegionCols - 1);
		anFrustumCellsMax[1] = min(anFrustumCellsMax[1], m_nNumRegionRows - 1);

		for (int i = anFrustumCellsMin[0]; i <= anFrustumCellsMax[0]; ++i)
			for (int j = anFrustumCellsMin[1]; j <= anFrustumCellsMax[1]; ++j)
			{
				SRegion* pRegion = &m_vRegions[GetRegionIndex(j, i)];
				pRegion->m_bCulled = OutsideFrustum(pRegion);
			}

		m_bAllRegionsCulled = false;
	}
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SetWindDirection

void CSpeedGrassRT::SetWindDirection(const float* pWindDir)
{
	memcpy(m_afWindDir, pWindDir, 3 * sizeof(float));
	m_afWindDir[3] = 0.0f;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::GetWindDirection

const float* CSpeedGrassRT::GetWindDirection(void)
{
	return m_afWindDir;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::GetCameraPos

const float* CSpeedGrassRT::GetCameraPos(void)
{
	return m_afCameraPos;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SetCamera

void CSpeedGrassRT::SetCamera(const float* pPosition, const float* pModelviewMatrix)
{
	memcpy(m_afCameraPos, pPosition, 3 * sizeof(float));

	// "right" vector
	m_afCameraRight[0] = pModelviewMatrix[0];
	m_afCameraRight[1] = pModelviewMatrix[4];
	m_afCameraRight[2] = pModelviewMatrix[8];

	// "up" vector
	m_afCameraUp[0] = pModelviewMatrix[1];
	m_afCameraUp[1] = pModelviewMatrix[5];
	m_afCameraUp[2] = pModelviewMatrix[9];

	// "out of screen" vector
	m_afCameraOut[0] = pModelviewMatrix[2];
	m_afCameraOut[1] = pModelviewMatrix[6];
	m_afCameraOut[2] = pModelviewMatrix[10];

	// with direction changed, billboard turns
	ComputeUnitBillboard( );

	// compute new frustum box
	ComputeFrustum( );
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::SetPerspective

void CSpeedGrassRT::SetPerspective(float fAspectRatio, float fFieldOfView)
{
	m_fAspectRatio = fAspectRatio;
	m_fFieldOfView = (fAspectRatio * fFieldOfView);
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::CreateRegions

#if 0
void CSpeedGrassRT::CreateRegions(const vector<SBlade>& vSceneBlades, float fCollisionDistance)
{
	// create regions based on overall extents
	m_vRegions.clear( );
	for (int i=0; i<m_nNumRegionRows * m_nNumRegionCols; i++)
		m_vRegions.push_back(SRegion());

	// run through all regions, computing extents for each
	float fCellWidth = (m_afBoundingBox[3] - m_afBoundingBox[0]) / m_nNumRegionCols;
	float fCellHeight = (m_afBoundingBox[4] - m_afBoundingBox[1]) / m_nNumRegionRows;

	float fY = m_afBoundingBox[1];
	for (int nRow = 0; nRow < m_nNumRegionRows; ++nRow)
	{
		float fX = m_afBoundingBox[0];
		for (int nCol = 0; nCol < m_nNumRegionCols; ++nCol)
		{
			SRegion* pRegion = &m_vRegions[GetRegionIndex(nRow, nCol)];

			// compute extents
			pRegion->m_afMin[0] = fX;
			pRegion->m_afMax[0] = fX + fCellWidth;
			pRegion->m_afMin[1] = fY;
			pRegion->m_afMax[1] = fY + fCellHeight;
		
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

	// assign each blade of grass to its particular region
	for (vector<SBlade>::const_iterator iBlade = vSceneBlades.begin( ); iBlade != vSceneBlades.end( ); ++iBlade)
	{
		// convert position to row/col index
		float fPercentAlongX = (iBlade->m_afPos[0] - m_afBoundingBox[0]) / (m_afBoundingBox[3] - m_afBoundingBox[0]);
		float fPercentAlongY = (iBlade->m_afPos[1] - m_afBoundingBox[1]) / (m_afBoundingBox[4] - m_afBoundingBox[1]);

		// clip values
		unsigned int uiCol = (unsigned int)min(fPercentAlongX * m_nNumRegionCols, m_nNumRegionCols - 1);
		unsigned int uiRow = (unsigned int)min(fPercentAlongY * m_nNumRegionRows, m_nNumRegionRows - 1);

		m_vRegions[GetRegionIndex(uiRow, uiCol)].m_vBlades.push_back(*iBlade);
	}

	// compute z extents (now that the blades are in)
	for (int i = 0; i < (int)m_vRegions.size( ); ++i)
	{
		SRegion* pRegion = &m_vRegions[i];

		pRegion->m_afMin[2] = FLT_MAX;
		pRegion->m_afMax[2] = -FLT_MAX;
		for (vector<SBlade>::iterator iBlade = pRegion->m_vBlades.begin( ); iBlade != pRegion->m_vBlades.end( ); ++iBlade)
		{
			pRegion->m_afMin[2] = min(pRegion->m_afMin[2], iBlade->m_afPos[2]);
			pRegion->m_afMax[2] = max(pRegion->m_afMax[2], iBlade->m_afPos[2] + iBlade->m_fHSize);
		}

		pRegion->m_afCenter[0] = 0.5f * (pRegion->m_afMin[0] + pRegion->m_afMax[0]);
		pRegion->m_afCenter[1] = 0.5f * (pRegion->m_afMin[1] + pRegion->m_afMax[1]);
		pRegion->m_afCenter[2] = 0.5f * (pRegion->m_afMin[2] + pRegion->m_afMax[2]);

		// compute culling radius
		pRegion->m_fCullingRadius = 1.1f * sqrt(
			((pRegion->m_afMax[0] - pRegion->m_afCenter[0]) * (pRegion->m_afMax[0] - pRegion->m_afCenter[0])) +
			((pRegion->m_afMax[1] - pRegion->m_afCenter[1]) * (pRegion->m_afMax[1] - pRegion->m_afCenter[1])) +
			((pRegion->m_afMax[2] - pRegion->m_afCenter[2]) * (pRegion->m_afMax[2] - pRegion->m_afCenter[2])) 
			);
	}
}
#endif

///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::ComputeFrustum

void CSpeedGrassRT::ComputeFrustum(void)
{
	// setup useful vectors
	idv::Vec3 cCameraIn(-m_afCameraOut[0], -m_afCameraOut[1], -m_afCameraOut[2]);
	idv::Vec3 cCameraUp(m_afCameraUp[0], m_afCameraUp[1], m_afCameraUp[2]);
	idv::Vec3 cCameraRight(m_afCameraRight[0], m_afCameraRight[1], m_afCameraRight[2]);
	idv::Vec3 cCameraPos(m_afCameraPos[0], m_afCameraPos[1], m_afCameraPos[2]);
	idv::Vec3 cFarPoint = cCameraPos + cCameraIn * (m_fLodFarDistance + m_fLodTransitionLength);

	// far plane
	memcpy(m_afFrustumPlanes[0], &cCameraIn[0], 3 * sizeof(float));
	m_afFrustumPlanes[0][3] = -cCameraIn.Dot(cFarPoint);
	idv::RotTransform cRotate;

	// upper plane
	cRotate.LoadIdentity( );
	cRotate.RotateAxis(idv::RadToDeg(0.5f * m_fFieldOfView * m_fAspectRatio + c_fHalfPi), cCameraRight);
	idv::Vec3 cNormal = cRotate * cCameraIn;
	cNormal.Normalize( );
	memcpy(m_afFrustumPlanes[1], &cNormal[0], 3 * sizeof(float));
	m_afFrustumPlanes[1][3] = -cNormal.Dot(cCameraPos);

	// left plane
	cRotate.LoadIdentity( );
	cRotate.RotateAxis(idv::RadToDeg(0.5f * m_fFieldOfView + c_fHalfPi), cCameraUp);
	cNormal = cRotate * cCameraIn;
	cNormal.Normalize( );
	memcpy(m_afFrustumPlanes[2], &cNormal[0], 3 * sizeof(float));
	m_afFrustumPlanes[2][3] = -cNormal.Dot(cCameraPos);

	// lower plane
	cRotate.LoadIdentity( );
	cRotate.RotateAxis(-idv::RadToDeg(0.5f * m_fFieldOfView * m_fAspectRatio + c_fHalfPi), cCameraRight);
	cNormal = cRotate * cCameraIn;
	cNormal.Normalize( );
	memcpy(m_afFrustumPlanes[3], &cNormal[0], 3 * sizeof(float));
	m_afFrustumPlanes[3][3] = -cNormal.Dot(cCameraPos);

	// right plane
	cRotate.LoadIdentity( );
	cRotate.RotateAxis(-idv::RadToDeg(0.5f * m_fFieldOfView + c_fHalfPi), cCameraUp);
	cNormal = cRotate * cCameraIn;
	cNormal.Normalize( );
	memcpy(m_afFrustumPlanes[4], &cNormal[0], 3 * sizeof(float));
	m_afFrustumPlanes[4][3] = -cNormal.Dot(cCameraPos);

	// frustum points
	float fFrustumHeight = (m_fLodFarDistance + m_fLodTransitionLength) * tanf(0.5f * m_fFieldOfView);
	float fFrustumWidth = (m_fLodFarDistance + m_fLodTransitionLength) * tanf(0.5f * m_fFieldOfView * m_fAspectRatio);

	idv::Vec3 acFrustum[5];
	acFrustum[0] = cCameraPos;
	acFrustum[1] = cFarPoint + cCameraRight * fFrustumWidth + cCameraUp * fFrustumHeight;
	acFrustum[2] = cFarPoint - cCameraRight * fFrustumWidth + cCameraUp * fFrustumHeight;
	acFrustum[3] = cFarPoint - cCameraRight * fFrustumWidth - cCameraUp * fFrustumHeight;
	acFrustum[4] = cFarPoint + cCameraRight * fFrustumWidth - cCameraUp * fFrustumHeight;

	// find min/max (x,y) coordinates
	m_afFrustumMin[0] = m_afFrustumMin[1] = FLT_MAX;
	m_afFrustumMax[0] = m_afFrustumMax[1] = -FLT_MAX;
	for (int i = 0; i < 5; ++i)
	{
		m_afFrustumMin[0] = min(m_afFrustumMin[0], acFrustum[i][0]);
		m_afFrustumMax[0] = max(m_afFrustumMax[0], acFrustum[i][0]);
		m_afFrustumMin[1] = min(m_afFrustumMin[1], acFrustum[i][1]);
		m_afFrustumMax[1] = max(m_afFrustumMax[1], acFrustum[i][1]);
	}
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::ComputeUnitBillboard

void CSpeedGrassRT::ComputeUnitBillboard(void)
{
	idv::RotTransform cTrans;
	cTrans.RotateZ(-idv::RadToDeg(atan2(-m_afCameraOut[1], -m_afCameraOut[0])));

	static idv::Vec3 afCorner1(0.0f, 0.5f, 1.0f);
	static idv::Vec3 afCorner2(0.0f, -0.5f, 1.0f);
	static idv::Vec3 afCorner3(0.0f, -0.5f, 0.0f);
	static idv::Vec3 afCorner4(0.0f, 0.5f, 0.0f);

	idv::Vec3 afNewCorner1 = cTrans * afCorner1;
	idv::Vec3 afNewCorner2 = cTrans * afCorner2;
	idv::Vec3 afNewCorner3 = cTrans * afCorner3;
	idv::Vec3 afNewCorner4 = cTrans * afCorner4;

	memcpy(m_afUnitBillboard + 0, afNewCorner1.m_afData, 3 * sizeof(float));
	memcpy(m_afUnitBillboard + 4, afNewCorner2.m_afData, 3 * sizeof(float));
	memcpy(m_afUnitBillboard + 8, afNewCorner3.m_afData, 3 * sizeof(float));
	memcpy(m_afUnitBillboard + 12, afNewCorner4.m_afData, 3 * sizeof(float));
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::ConvertCoordsToCell

void CSpeedGrassRT::ConvertCoordsToCell(const float* pCoords, int* pGridCoords) const
{
    float fPercentAlongX = (pCoords[0] - m_afBoundingBox[0]) / (m_afBoundingBox[3] - m_afBoundingBox[0]);
    float fPercentAlongY = (pCoords[1] - m_afBoundingBox[1]) / (m_afBoundingBox[4] - m_afBoundingBox[1]);

    if (fPercentAlongX < 0.0f)
		pGridCoords[0] = -1;
	else if (fPercentAlongX > 1.0f)
		pGridCoords[0] = m_nNumRegionCols;
	else
	    pGridCoords[0] = (int)(fPercentAlongX * m_nNumRegionCols);

	if (fPercentAlongY < 0.0f)
		pGridCoords[1] = -1;
	else if (fPercentAlongY > 1.0f)
		pGridCoords[1] = m_nNumRegionRows;
	else
	    pGridCoords[1] = (int)(fPercentAlongY * m_nNumRegionRows);
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::OutsideFrustum

inline bool CSpeedGrassRT::OutsideFrustum(CSpeedGrassRT::SRegion* pRegion)
{
	bool bOutside = false;

	for (int i = 0; i < 5 && !bOutside; ++i)
		if (m_afFrustumPlanes[i][0] * pRegion->m_afCenter[0] + 
			m_afFrustumPlanes[i][1] * pRegion->m_afCenter[1] + 
			m_afFrustumPlanes[i][2] * pRegion->m_afCenter[2] +
			m_afFrustumPlanes[i][3] > pRegion->m_fCullingRadius)
			bOutside = true;

	return bOutside;
}


///////////////////////////////////////////////////////////////////////  
//	CSpeedGrassRT::DeleteRegions

void CSpeedGrassRT::DeleteRegions( )
{
	m_vRegions.clear( );
	m_nNumRegionRows = 0;
	m_nNumRegionCols = 0;
}


//};