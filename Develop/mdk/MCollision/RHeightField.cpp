#include "stdafx.h"
#include "RHeightField.h"

#include <stdio.h>
#include "ddsfile.h"
#include "MDebug.h"
#include "MMath.h"
#include "MFileSystem.h"

using namespace std;

int RHeightField::GRID_SIZE = 16;
int RHeightField::DESC_VERSION = 0x00020001;	// major version, minor version

vector<pair<int,int> > RHeightField::m_vecGridArray;
vector<pair<int,int> > RHeightField::m_vecTriangleArray;

RHeightField::RHeightField()
: m_pHeightMap(NULL)
, m_vScale(1, 1)
, m_nHeightMapPitch(0)
, m_pGridMap(NULL)
, m_nGridWidth(0)
, m_nGridHeight(0)
, m_nGridSize(0)
, m_nHeightMapArraySize(0)
, m_pHeightMapDesc(NULL)
{
	m_aabb.Initialize();
}

RHeightField::~RHeightField()
{
	SAFE_DELETE_ARRAY(m_pGridMap);
	SAFE_DELETE_ARRAY(m_pHeightMap);
	SAFE_DELETE_ARRAY(m_pHeightMapDesc);

	m_nHeightMapArraySize = 0;
}

void RHeightField::UpdateHeightField()
{
	if (m_pHeightMap)
	{
		// 각 RECT 에 따라 SGridInfo 를 업데이트 한다
		for ( int i = 0; i<m_nGridWidth*m_nGridHeight; ++i )
			GetGridAABB(&m_pGridMap[i].m_aabb,&m_pGridMap[i].m_kRect);
	}
}

void RHeightField::GetGridAABB(MBox *pAABB, const RECT *pRect)
{
	MVector3 vPos = MVector3(pRect->left*GetXScale(), pRect->top*GetYScale(), GetHeightAtXYIndex(pRect->left, pRect->top));

	pAABB->vmin = vPos;
	pAABB->vmax = vPos;

	for ( int h = pRect->top; h<=pRect->bottom; ++h)
		for ( int w = pRect->left; w<=pRect->right; ++w)
		{
			_ASSERT(w>=0);
			_ASSERT(w<=m_nWidthMap);
			_ASSERT(h>=0);
			_ASSERT(h<=m_nHeightMap);

			pAABB->Add(MVector3(w*GetXScale(), h*GetYScale(), GetHeightAtXYIndex(w,h)));
		}
}

void RHeightField::CreateDesc()
{
	_ASSERT(m_pHeightMap!=NULL);
	_ASSERT(m_pHeightMapDesc==NULL);

	_ASSERT(m_nWidthMap>=0);
	_ASSERT(m_nHeightMap>=0);

	int nSize = m_nWidthMap*m_nHeightMap;
	m_pHeightMapDesc = new SDesc[nSize];
	memset(m_pHeightMapDesc, 0, sizeof(SDesc)*nSize);

	return;
}

void RHeightField::ClearDescriptMap()
{
	int nSize = m_nWidthMap*m_nHeightMap;
	if (nSize && m_pHeightMapDesc)
		memset(m_pHeightMapDesc, 0, sizeof(SDesc)*nSize);
}

void RHeightField::CreateGridInfo(int nGridSize)
{
	_ASSERT(m_pHeightMap!=NULL);
	_ASSERT(m_pGridMap==NULL);

	float dx = (float)m_nWidthMap/(float)nGridSize;
	float dy = (float)m_nHeightMap/(float)nGridSize;

	m_nGridWidth = static_cast<int>(ceil(dx));
	m_nGridHeight = static_cast<int>(ceil(dy));
	m_nGridSize = nGridSize;

	int nSize = m_nGridWidth*m_nGridHeight;

	m_pGridMap = new SGridInfo[nSize];
	
	// Initialize.
	for ( int h = 0; h<m_nGridHeight; ++h )
		for ( int w = 0; w<m_nGridWidth; ++w )
		{
			RECT rt;
			rt.left	= w*nGridSize;
			rt.top	= h*nGridSize;
			rt.right = rt.left+nGridSize;
			rt.bottom = rt.top+nGridSize;

			rt.right = min(rt.right, m_nWidthMap);
			rt.bottom = min(rt.bottom, m_nHeightMap);

			int nGridIndex = w + h*m_nGridWidth;
			m_pGridMap[nGridIndex].m_kRect = rt;
		}

	m_vecGridArray.reserve(nSize);
	m_vecTriangleArray.reserve(m_nGridSize*m_nGridSize);
}

bool RHeightField::LoadHeightFieldDescFile(const char *filename)
{
	_ASSERT(m_pHeightMapDesc);

	MFile descFile;
	if ( !descFile.Open(filename))
		return false;

	unsigned int nSize = descFile.GetLength();
	char *pBuffer = new char[nSize];
	
	descFile.ReadAll(pBuffer, nSize);
	descFile.Close();

	SDescHeader *pHeader = (SDescHeader*)pBuffer;
	if (strcmp(pHeader->pcHeader,"TED")!=0
		|| pHeader->dwVersion != DESC_VERSION )
	{
		mlog("desc file version mis-match\n");
		SAFE_DELETE_ARRAY(pBuffer);
		return false;
	}

	char *ptr = pBuffer + sizeof(SDescHeader);
	DWORD dwWidth = *((DWORD*)ptr);		ptr+=sizeof(DWORD);
	DWORD dwHeight = *((DWORD*)ptr);	ptr+=sizeof(DWORD);
	DWORD dwSize = dwWidth*dwHeight;
	_ASSERT(nSize==( sizeof(SDescHeader)+sizeof(DWORD)*2+dwSize*sizeof(SDesc)));
	if ((m_nWidthMap != dwWidth) || (m_nHeightMap != dwHeight))
	{
		SAFE_DELETE_ARRAY(pBuffer);
		return false;
	}

	memcpy(m_pHeightMapDesc, ptr, sizeof(SDesc)*dwSize);
	SAFE_DELETE_ARRAY(pBuffer);

	return true;
}

void RHeightField::SaveHeightFieldDescFile(const char *filename)
{
	DWORD dwWidth = GetWidthMap();
	DWORD dwHeight = GetHeightMap();

	FILE *fp = NULL;
	if ( fopen_s(&fp, filename, "wb") != 0 )
		return;

	// Header 이니셜은 TED
	SDescHeader Header;
	strncpy_s(Header.pcHeader, sizeof(Header.pcHeader), "TED", 3);
	Header.pcHeader[3] = '\0';
	Header.dwVersion = DESC_VERSION;

	/**
	 DWORD	displacementMapWidth;
	 DWORD	displacementMapHeight;
	 SLONG	...(displacementMapWidth*displacementMapHeight 만큼 long 데이터들)
	*/
	fwrite(&Header, sizeof(Header), 1, fp);
	
	DWORD dwSize = dwWidth*dwHeight;
	fwrite(&dwWidth, sizeof(DWORD), 1, fp);
	fwrite(&dwHeight, sizeof(DWORD), 1, fp);
	fwrite((void*)m_pHeightMapDesc, sizeof(SDesc)*dwSize, 1, fp);

	fclose(fp);
	return;
}

float *RHeightField::LoadR32FDDSFromFile(const char *filename, int *outWidth, int *outHeight, float fZScale, float fZOffset)
{
	MFile file;
	if ( !file.Open(filename))
		return NULL;

	char pcHeader[4];
	file.Read((void*)pcHeader, 4);
	if ( pcHeader[0] != 'D' || pcHeader[1] != 'D' || pcHeader[2] !='S' )
		return NULL;

	MDDSDESC2 ddsd;
	memset(&ddsd, 0, sizeof(MDDSDESC2));
	file.Read((void*)&ddsd, sizeof(MDDSDESC2));
	if ( ddsd.ddpfPixelFormat.dwFourCC != DDS_FMT_R32F )
		return NULL;

	DWORD dwPitch = ddsd.dwLinearSize;
	DWORD dwWidth = ddsd.dwWidth;
	DWORD dwHeight = ddsd.dwHeight;
	dwPitch = dwWidth*sizeof(float);

	DWORD dwSize = dwPitch * dwHeight;

	BYTE *pData = new BYTE[dwSize];
	bool bDataLoad = file.Read(pData, dwSize);
	_ASSERT(bDataLoad);

	int nImageWidth = (int)dwWidth;
	int nImageHeight = (int)dwHeight;
	int nPointNum = nImageWidth * nImageHeight;

	float* pFP32ARGB = new float[nPointNum];

	//HeightField 생성 - Picking 및 생성하기 전에 기본 데이터로 사용된다.
	for ( int y = 0; y < nImageHeight; ++y )
	{
		BYTE *pScanLine = pData + dwPitch*(nImageHeight-y-1);
		float *pHeight = (float*)pScanLine;
		for ( int x = 0; x < nImageWidth; ++x )
		{
			int nIndex = x+y*nImageWidth;
			float fData = *pHeight;
			_ASSERT( *((DWORD*)pHeight) != 0x7fc00000 && "유효하지 않은 값 ( from displacement map )" );

			pFP32ARGB[nIndex] = (fData * fZScale) - fZOffset;
			++pHeight;
		}
	}
	delete [] pData;
	pData = NULL;

	if ( outWidth )	
		*outWidth = nImageWidth;

	if ( outHeight )
		*outHeight = nImageHeight;

	return pFP32ARGB;
}

bool RHeightField::LoadHeightFieldmapFromR32F(const char *filename, const MVector2 &vSize)
{
	_ASSERT(m_pHeightMap==NULL);
	
	int nImageWidth = 0;
	int nImageHeight = 0;

	m_pHeightMap = LoadR32FDDSFromFile(filename,&nImageWidth,&nImageHeight, 1.0f, 0.0f);
	if ( m_pHeightMap == NULL )
	{
		mlog("load heightfield failed - %s\n", filename);
		return false;
	}

	if ( nImageWidth != nImageHeight )
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);
		mlog("this is not regular square map - %s\n", filename);
		return false;
	}

	int nImageSize = nImageWidth * nImageHeight;

	if ( fmod(MMath::Log2((float)(nImageWidth-1)), 1.0f) != 0.0 
		|| fmod(MMath::Log2((float)(nImageHeight-1)), 1.0f) != 0.0 )
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);

		mlog("width is not 2^n + 1\n");
		return false;
	}

	m_nHeightMapPitch = nImageWidth;
	m_nWidthMap = nImageWidth-1;
	m_nHeightMap = nImageHeight-1;

	_ASSERT(vSize.x >= 0.0f );
	_ASSERT(vSize.y >= 0.0f );
	if (vSize.x < 0.0f || vSize.y < 0.0f)
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);
		return false;
	}

	m_vScale.x = vSize.x/(float)m_nWidthMap;
	m_vScale.y = vSize.y/(float)m_nHeightMap;

	_ASSERT(m_vScale.x >= 0.0f );
	_ASSERT(m_vScale.y >= 0.0f );
	if (m_vScale.x < 0.0f || m_vScale.y < 0.0f)
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);
		return false;
	}

	m_nHeightMapArraySize = nImageSize;

	_ASSERT(GetWidthSize()==vSize.x);
	_ASSERT(GetHeightSize()==vSize.y);
	
	CreateGridInfo(GRID_SIZE);
	CreateDesc();
	UpdateAABB();
	return true;
}

bool RHeightField::Create(int nImageWidth, int nImageHeight, MVector2 &vSize)
{
	_ASSERT(m_pHeightMap==NULL);

	int nImageSize = nImageWidth * nImageHeight;
	m_pHeightMap = new float[nImageSize];

	if ( m_pHeightMap == NULL )
	{
		mlog("failed file create\n");
		return false;
	}

	if ( nImageWidth != nImageHeight )
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);

		mlog("this is not regular square map\n");
		return false;
	}

	if ( fmod(MMath::Log2((float)(nImageWidth-1)), 1.0f) != 0.0 
		|| fmod(MMath::Log2((float)(nImageHeight-1)), 1.0f) != 0.0 )
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);

		mlog("width is not 2^n + 1\n");
		return false;
	}

	for ( int i = 0; i<nImageWidth*nImageHeight; ++i )
		m_pHeightMap[i] = 0.0f;

	m_nHeightMapPitch = nImageWidth;
	m_nWidthMap = nImageWidth-1;
	m_nHeightMap = nImageHeight-1;

	_ASSERT(vSize.x >= 0.0f );
	_ASSERT(vSize.y >= 0.0f );

	if (vSize.x < 0.0f || vSize.y < 0.0f)
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);
		return false;
	}

	m_vScale.x = vSize.x/(float)m_nWidthMap;
	m_vScale.y = vSize.y/(float)m_nHeightMap;
	
	_ASSERT(m_vScale.x >= 0.0f );
	_ASSERT(m_vScale.y >= 0.0f );

	if (m_vScale.x < 0.0f || m_vScale.y < 0.0f)
	{
		SAFE_DELETE_ARRAY(m_pHeightMap);
		return false;
	}

	_ASSERT(GetWidthSize()==vSize.x);
	_ASSERT(GetHeightSize()==vSize.y);
	
	m_nHeightMapArraySize = nImageSize;

	CreateGridInfo(GRID_SIZE);
	CreateDesc();
	UpdateAABB();
	return true;
}

void RHeightField::SaveHeightFieldmapToR32F(const char *filename)
{
	int nWidth = m_nWidthMap + 1;
	int nHeight = m_nHeightMapPitch;

	FILE *fp = NULL;
	if ( fopen_s(&fp, filename, "wb") != 0)
		return;

	// Header
	fwrite("DDS ", sizeof(char), 4, fp);

	MDDSDESC2 ddsd;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = nWidth;
	ddsd.dwHeight = nHeight;
	ddsd.ddpfPixelFormat.dwSize = sizeof(MDDSPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
	ddsd.ddpfPixelFormat.dwFourCC = DDS_FMT_R32F;
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	fwrite(&ddsd, sizeof(ddsd), 1, fp);

	//HeightField 생성 - Picking 및 생성하기 전에 기본 데이터로 사용된다.
	for ( int y = 0; y < nHeight; ++y )
	{
		// 뒤집는다. 읽을 때도 뒤집도록 되어 있다. 옛날 .bmp 를 사용했을 때 뒤집어서 저장해버려서 ...
		float *pHeight = &m_pHeightMap[nWidth*(nHeight-y-1)];
		fwrite(pHeight, sizeof(float), nWidth, fp);
	}

	fclose(fp);
	return;
}

float RHeightField::GetHeightAtXYIndex(int x, int y)
{
	_ASSERT(x>=0);
	_ASSERT(y>=0);
	_ASSERT(x<=m_nWidthMap);
	_ASSERT(y<=m_nHeightMap);

	return m_pHeightMap[x + y*m_nHeightMapPitch];
}

bool RHeightField::GetAttribute(int x, int y, TERRAIN_DESC_BOOLEAN_ATTRIB attrib)
{
	if ( x < 0 )	return false;
	if ( y < 0 )	return false;

	if ( x == m_nWidthMap )
		--x;
	if ( y == m_nHeightMap)
		--y;

	if ( x >= m_nWidthMap )
		return false;
	if ( y >= m_nHeightMap )
		return false;

	_ASSERT( m_pHeightMapDesc);
	return ( m_pHeightMapDesc[x + y*m_nWidthMap].dwAttribute & attrib ? true : false );
}

bool RHeightField::GetAttribute(float x, float y, TERRAIN_DESC_BOOLEAN_ATTRIB attrib)
{
	int nx = static_cast<long>(x/GetXScale());
	int ny = static_cast<long>(y/GetYScale());
	
	return GetAttribute(nx, ny, attrib);
}

DWORD RHeightField::GetMaterial(float x, float y)
{
	int nx = static_cast<long>(x/GetXScale());
	int ny = static_cast<long>(y/GetYScale());

	return GetMaterial(nx, ny);
}

DWORD RHeightField::GetMaterial(int x, int y)
{
	if ( x < 0 )	return 0;
	if ( y < 0 )	return 0;

	if ( x == m_nWidthMap )
		--x;
	if ( y == m_nHeightMap)
		--y;

	if ( x >= m_nWidthMap )
		return 0;
	if ( y >= m_nHeightMap )
		return 0;

	_ASSERT( m_pHeightMapDesc);
	return m_pHeightMapDesc[x + y*m_nWidthMap].dwMaterial;
}

void RHeightField::SetAttribute(int nx, int ny, TERRAIN_DESC_BOOLEAN_ATTRIB attrib, bool bFlag)
{
	_ASSERT(nx>=0);
	_ASSERT(ny>=0);
	_ASSERT(nx<m_nWidthMap);
	_ASSERT(ny<m_nHeightMap);
	_ASSERT(m_pHeightMapDesc);

	DWORD *ptr = &m_pHeightMapDesc[nx + ny*m_nWidthMap].dwAttribute;
	*ptr &= (~attrib);	// clear bit
	if ( bFlag )
		*ptr |= attrib;

	return;
}

void RHeightField::SetMaterial(int nx, int ny, DWORD dwID)
{
	_ASSERT(nx>=0);
	_ASSERT(ny>=0);
	_ASSERT(nx<m_nWidthMap);
	_ASSERT(ny<m_nHeightMap);
	_ASSERT(m_pHeightMapDesc);

	m_pHeightMapDesc[nx + ny*m_nWidthMap].dwMaterial = dwID;
	return;

}

void RHeightField::UpdateAABB()
{
	m_fMinZ = FLT_MAX;
	m_fMaxZ =-FLT_MAX;

	if ( m_pGridMap == NULL )
	{
		int nImageWidth = GetWidthMap()+1;
		int nImageHeight = GetHeightMap()+1;

		// Min, Max 높이 구하기
		for ( int y = 0; y<nImageHeight; ++y )
		{
			for ( int x = 0; x<nImageWidth; ++x )
			{
				float fHeight = m_pHeightMap[x + m_nHeightMapPitch*y];
				if ( fHeight > m_fMaxZ )
					m_fMaxZ = fHeight;
				if ( fHeight < m_fMinZ )
					m_fMinZ = fHeight;
			}
		}
	}
	else 
	{
		UpdateHeightField();
		for ( int i = 0; i<m_nGridWidth* m_nGridHeight; ++i )
		{
			const MBox *pAABB = &m_pGridMap[i].m_aabb;
			if ( pAABB->vmin.z < m_fMinZ )
				m_fMinZ = pAABB->vmin.z;
			if ( pAABB->vmax.z > m_fMaxZ )
				m_fMaxZ = pAABB->vmax.z;
		}
	}

	m_aabb.vmin.x = 0.0f;
	m_aabb.vmin.y = 0.0f;
	m_aabb.vmin.z = m_fMinZ;

	m_aabb.vmax.x = GetWidthSize();
	m_aabb.vmax.y = GetHeightSize();
	m_aabb.vmax.z = m_fMaxZ;
}

bool RHeightField::GetHeightAt(float *height, MVector3 *pNormal, float x, float y)
{
	if ( x < 0.0f 
		|| y < 0.0f 
		|| x > GetWidthSize()
		|| y > GetHeightSize())
	{
		return false;
	}

	return GetHeightAtWithHole(height, pNormal, x, y);

	//float fX = x / m_vScale.x;
	//float fY = y / m_vScale.y;

	//int nX = (int)fX;
	//int nY = (int)fY;

	//fX = fX - nX;
	//fY = fY - nY;

	//_ASSERT(fX>=0.0f);
	//_ASSERT(fY>=0.0f);
	//_ASSERT(fX<1.0f);
	//_ASSERT(fY<1.0f);

	//float z;
	//float u, v;

	//MVector3 v0, v1, v2;

	//if ( fX + fY == 0.0f )
	//{
	//	z = ( m_pHeightMap[nX+nY*m_nHeightMapPitch] );
	//	
	//	v0 = MVector3(nX * GetXScale(), nY * GetYScale(), z);
	//	v1 = MVector3((nX+1)*GetXScale(), nY * GetYScale(), m_pHeightMap[nX+1 + nY*m_nHeightMapPitch]);
	//	v2 = MVector3(nX * GetXScale(), (nY+1) * GetYScale(), m_pHeightMap[nX+ (nY+1)*m_nHeightMapPitch]);
	//}
	//else if ( (fX + fY) < 1.0f )
	//{
	//	z = ( m_pHeightMap[nX+nY*m_nHeightMapPitch] );	
	//	u = m_pHeightMap[nX+1 + nY*m_nHeightMapPitch] - z;
	//	v = m_pHeightMap[nX+ (nY+1)*m_nHeightMapPitch] - z;

	//	v0 = MVector3(nX * GetXScale(), nY * GetYScale(), z);
	//	v1 = MVector3((nX+1)*GetXScale(), nY * GetYScale(), u + z);
	//	v2 = MVector3(nX * GetXScale(), (nY+1) * GetYScale(), v + z);

	//	z += u*fX + v*fY;
	//}
	//else
	//{
	//	z = m_pHeightMap[nX+1+(nY+1)*m_nHeightMapPitch];
	//	u = m_pHeightMap[nX + (nY+1)*m_nHeightMapPitch] - z;
	//	v = m_pHeightMap[nX+1+ nY*m_nHeightMapPitch] - z;

	//	v0 = MVector3((nX+1) * GetXScale(), (nY+1)* GetYScale(), z);
	//	v1 = MVector3(nX * GetXScale(), (nY+1) * GetYScale(), u + z);
	//	v2 = MVector3((nX+1)*GetXScale(), nY * GetYScale(), v + z);

	//	z += (1.0f-fX)*u + (1.0f-fY)*v;
	//}

	//if ( pNormal )
	//{
	//	MVector3 A = v1 - v0;
	//	MVector3 B = v2 - v0;
	//	MVector3 N = A.CrossProduct(B);

	//	*pNormal = N.Normalize();

	//	_ASSERT(N.z >= 0.0f );
	//}
	//
	//if ( height )
	//{
	//	*height = z;
	//}

	//return true;
}

bool RHeightField::GetHeightAtWithHole( float *height, MVector3 *pNormal, float x, float y )
{
	if ( x < 0.0f 
		|| y < 0.0f 
		|| x > GetWidthSize()
		|| y > GetHeightSize())
	{
		return false;
	}

	float fX = x / m_vScale.x;
	float fY = y / m_vScale.y;

	int nX = (int)fX;
	int nY = (int)fY;

	fX = fX - nX;
	fY = fY - nY;

	_ASSERT(fX>=0.0f);
	_ASSERT(fY>=0.0f);
	_ASSERT(fX<1.0f);
	_ASSERT(fY<1.0f);

	//_ASSERT(nY+1 != m_nHeightMapPitch);

	bool bHeightMapAccessable = (nY+1  >= m_nHeightMap) ? false : true;
	float z(0.f), u(0.f), v(0.f);
	MVector3 v0, v1, v2;

	if ( fX + fY == 0.0f )
	{
		if (bHeightMapAccessable)
		{
			z = ( m_pHeightMap[nX+nY*m_nHeightMapPitch] );
			u = m_pHeightMap[nX+1 + nY*m_nHeightMapPitch];
			v = m_pHeightMap[nX+ (nY+1)*m_nHeightMapPitch];
		}

		v0 = MVector3(nX * GetXScale(), nY * GetYScale(), z);
		v1 = MVector3((nX+1)*GetXScale(), nY * GetYScale(), u);
		v2 = MVector3(nX * GetXScale(), (nY+1) * GetYScale(), v);
	}
	else if ( (fX + fY) < 1.0f )
	{
		if (bHeightMapAccessable)
		{
			z = ( m_pHeightMap[nX+nY*m_nHeightMapPitch] );	
			u = m_pHeightMap[nX+1 + nY*m_nHeightMapPitch] - z;
			v = m_pHeightMap[nX+ (nY+1)*m_nHeightMapPitch] - z;
		}

		v0 = MVector3(nX * GetXScale(), nY * GetYScale(), z);
		v1 = MVector3((nX+1)*GetXScale(), nY * GetYScale(), u + z);
		v2 = MVector3(nX * GetXScale(), (nY+1) * GetYScale(), v + z);

		z += u*fX + v*fY;
	}
	else
	{
		if (bHeightMapAccessable)
		{
			z = m_pHeightMap[nX+1+(nY+1)*m_nHeightMapPitch];
			u = m_pHeightMap[nX + (nY+1)*m_nHeightMapPitch] - z;
			v = m_pHeightMap[nX+1+ nY*m_nHeightMapPitch] - z;
		}

		v0 = MVector3((nX+1) * GetXScale(), (nY+1)* GetYScale(), z);
		v1 = MVector3(nX * GetXScale(), (nY+1) * GetYScale(), u + z);
		v2 = MVector3((nX+1)*GetXScale(), nY * GetYScale(), v + z);

		z += (1.0f-fX)*u + (1.0f-fY)*v;
	}

	if ( pNormal )
	{
		MVector3 A = v1 - v0;
		MVector3 B = v2 - v0;
		MVector3 N = A.CrossProduct(B);

		*pNormal = N.Normalize();

		_ASSERT(N.z >= 0.0f );
	}

	if ( height )
	{
		*height = z;
	}

	return true;
}

void RHeightField::GetLineIntersectGrid(std::vector<std::pair<int, int> > &vecArray
										, const MVector2 &P1, const MVector2 &P2, float fXScale, float fYScale)
{
	_ASSERT(P1.x<=P2.x);

	vecArray.clear();

	float dx = (P2.x-P1.x);
	float dy = (P2.y-P1.y);

	if ( dx == 0.0f )
	{
		int x = static_cast<int> (P1.x / fXScale);
		int nY1 = static_cast<int>(P1.y / fYScale);
		int nY2 = static_cast<int>(P2.y / fYScale);

		pair<int, int> nIndex2D;

		for ( int y = nY1; y<=nY2; ++y )
		{
			nIndex2D.first = x;
			nIndex2D.second = y;
			vecArray.push_back(nIndex2D);
		}

		return;
	}

	float dydx = dy/dx;

	int nX1 = static_cast<int>(P1.x/fXScale);
	int nX2 = static_cast<int>(P2.x/fXScale);

	float px = (nX1+1)*fXScale - P1.x;

	float y1 = P1.y;
	float y2 = y1 + px*dydx;

	while ( nX1 < nX2 )
	{
		float fy1 = y1/fYScale;
		float fy2 = y2/fYScale;

		if ( fy2 < fy1 )
			swap(fy2, fy1);

		int nY1 = static_cast<int>(fy1);
		int nY2 = static_cast<int>(fy2);

		for ( int y = nY1; y <nY2; y++)
			vecArray.push_back(pair<int, int>(nX1, y));
		
		// 경계에 걸리는 부분 검사
		if ( static_cast<float>(nY2) != fy2 )
			vecArray.push_back(pair<int, int>(nX1, nY2));

		y1 = y2;
		y2 += fXScale*dydx;

		++nX1;
	}
	
	// 경계에 걸리는 부분 검사
	if ( static_cast<float>(nX1) != (P2.x/fXScale) )
	{
		y1 /= fYScale;
		y2 = P2.y/fYScale;

		if ( y2 < y1 )
			swap(y1, y2);

		int nY1 = static_cast<int>(y1);
		int nY2 = static_cast<int>(y2);

		for ( int y = nY1; y <nY2; y++)
			vecArray.push_back(pair<int, int>(nX1, y));
		
		// 경계에 걸리는 부분 검사
		if ( static_cast<float>(nY2) != y2 )
			vecArray.push_back(pair<int, int>(nX1, nY2));
	}

	return;
}

void RHeightField::GetPositionAtIndexXY(MVector3 *pOut, int w, int h)
{
	_ASSERT(m_pHeightMap);

	float x = w*m_vScale.x;
	float y = h*m_vScale.y;

	_ASSERT(x<=GetWidthSize());
	_ASSERT(y<=GetHeightSize());

	int nIndex = w + h*m_nHeightMapPitch;

	_ASSERT(nIndex>=0);
	_ASSERT(nIndex<m_nHeightMapArraySize);

	if ( pOut )
	{
		pOut->x = x;
		pOut->y = y;
		pOut->z = m_pHeightMap[nIndex];
	}
}

// line intersect 를 이용하여 더 최적화된 Picking 테스트로 변경.
bool RHeightField::GetPickLine(MVector3 *pOut, MVector3 *pOutNormal, float *pPickDistance, const MVector3 &vP, const MVector3 &vQ, TERRAIN_DESC_BOOLEAN_ATTRIB NonPickingAttrib)
{
	_ASSERT(m_pHeightMap);
	if ( !m_aabb.ContainsLineSegment(vP, vQ))
		return false;

	MVector2 P1(vP.x, vP.y);
	MVector2 P2(vQ.x, vQ.y);

	MRectF tRect(m_aabb.vmin.x, m_aabb.vmin.y, m_aabb.vmax.x, m_aabb.vmax.y);

	// 클리핑 실패 (즉 OutSide) 이면 Picking 할 필요가 없음.
	if ( !MMath::LineClipping2D(P1, P2,&tRect))
		return false;

	// x, y가 같으면 높이만 구하면 OK
	if ( P1 == P2 )
	{
		float fHeight = -FLT_MAX;
		if ( !GetHeightAt(&fHeight, pOutNormal, P1.x, P1.y))
			return false;

		if ( pOut )
			*pOut = MVector3(P1.x, P1.y, fHeight);

		if ( pPickDistance )
			*pPickDistance = vP.z - fHeight;
		
		float fMax = max(vP.z, vQ.z);
		float fMin = min(vP.z, vQ.z);

		if ( fHeight <= fMax 
			&& fHeight >= fMin )
			return true;

		return false;
	}

	MVector3 vDir = vQ - vP;
	float fLineLength = vDir.Length();
	vDir /= fLineLength;

	// 그리드 맵이 없으면 전체를 일괄 검사, 작은 크기의 맵(8x8 이하) 일 때는 m_pGridMap이 생성되지 않는다.
	if ( m_pGridMap == NULL )
	{
		RECT rt;
		rt.left = 0;
		rt.top = 0;
		rt.right = m_nWidthMap;
		rt.bottom = m_nHeightMap;

		MVector3 vTriangle[3];
		float fDist = GetPickLineRect(pOut, vP, vQ, vDir, &rt, vTriangle, NonPickingAttrib);

		if ( fDist < 0.0f 
			|| fDist > fLineLength )
		{
			return false;
		}

		if ( pOut )
			*pOut = vP + vDir*fDist;
		
		if ( pPickDistance )
			*pPickDistance = fDist;

		if ( pOutNormal )
		{
			MVector3 V1 = vTriangle[1] - vTriangle[0];
			MVector3 V2 = vTriangle[2] - vTriangle[1];
			MVector3 N = V1.CrossProduct(V2);

			*pOutNormal = N.Normalize();
		}

		return true;
	}

	float fDist = FLT_MAX;
	MVector3 vTriangle[3];

	float fGridXScale = m_nGridSize*GetXScale();
	float fGridYScale = m_nGridSize*GetYScale();

	if ( P1.x > P2.x )
		std::swap(P1, P2);

	GetLineIntersectGrid(m_vecGridArray, P1, P2, fGridXScale, fGridYScale);

	for ( size_t si = 0; si<m_vecGridArray.size(); ++si )
	{
		int i = m_vecGridArray[si].first + m_vecGridArray[si].second*m_nGridWidth;
		if ( m_pGridMap[i].m_aabb.ContainsLineSegment(vP, vQ))
		{
			float fHitDist = FLT_MAX;
			MVector3 vHitTriangle[3];
			{
				fHitDist = GetPickLineRect(NULL, vP, vQ, vDir,&m_pGridMap[i].m_kRect, vHitTriangle, NonPickingAttrib);
				if ( fHitDist < fDist && fHitDist >= 0.0f )
				{
					fDist = fHitDist;
					vTriangle[0] = vHitTriangle[0];
					vTriangle[1] = vHitTriangle[1];
					vTriangle[2] = vHitTriangle[2];
				}
			}
		}
	}

	if ( fDist <= fLineLength)
	{
		if ( pOut )
			*pOut = vP + vDir*fDist;

		if ( pPickDistance )
			*pPickDistance = fDist;

		if ( pOutNormal )
		{
			MVector3 V1 = vTriangle[1] - vTriangle[0];
			MVector3 V2 = vTriangle[2] - vTriangle[1];
			MVector3 N = V1.CrossProduct(V2);

			*pOutNormal = N.Normalize();
		}

		return true;
	}

	return false;
}

float RHeightField::GetPickLineRect(MVector3 *pOut, const MVector3 &vP, const MVector3 &vQ, const MVector3 &vDir, const RECT *pRect , MVector3 *pTriangle, TERRAIN_DESC_BOOLEAN_ATTRIB NonPickingAttrib)
{
	_ASSERT(pRect!=NULL);

	float t = FLT_MAX;

	MVector3 kOut;

	MVector2 P1(vP.x, vP.y);
	MVector2 P2(vQ.x, vQ.y);

	int nPitch = m_nHeightMapPitch;
	MRectF tRect(pRect->left*GetXScale(), pRect->top*GetYScale()
		, pRect->right*GetXScale(), pRect->bottom*GetYScale());

	// 클리핑 실패 (즉 OutSide) 이면 Picking 할 필요가 없음.
	if ( !MMath::LineClipping2D(P1, P2,&tRect))
		return false;

	if ( P1.x > P2.x )
		swap(P1, P2);

	GetLineIntersectGrid(m_vecTriangleArray, P1, P2, GetXScale(), GetYScale());

	for ( size_t si = 0; si<m_vecTriangleArray.size(); ++si )
	{
		const int &h = m_vecTriangleArray[si].second;
		const int &w = m_vecTriangleArray[si].first;

		//_ASSERT(w>=pRect->left);
		//_ASSERT(h>=pRect->top);
		if ( w >= pRect->right )
			continue;
		if ( h >= pRect->bottom )
			continue;

		float x = w*m_vScale.x;
		float y = h*m_vScale.y;

		_ASSERT(x<=GetWidthSize());
		_ASSERT(y<=GetHeightSize());

		MVector3 v0, v1, v2, v3;

		GetPositionAtIndexXY(&v0, w, h);
		GetPositionAtIndexXY(&v1, w, h+1);
		GetPositionAtIndexXY(&v2, w+1, h);
		GetPositionAtIndexXY(&v3, w+1, h+1);

		// count clock wise
		if ( MMath::IsIntersect(vP,vDir,v0,v2,v3,&kOut))
		{
			MVector3 V = kOut - vP;
			if ( vDir.DotProduct(V) > 0.0f )
			{
				float len = (kOut - vP).Length();
				if ( t>len && !GetAttribute(kOut.x, kOut.y, NonPickingAttrib))
				{
					t = len;
					if ( pTriangle )
					{
						pTriangle[0] = v0;
						pTriangle[1] = v2;
						pTriangle[2] = v3;
					}
				}
			}
		}
		if ( MMath::IsIntersect(vP,vDir,v0,v3,v1,&kOut))
		{
			MVector3 V = kOut - vP;
			if ( vDir.DotProduct(V) > 0.0f )
			{
				float len = (kOut - vP).Length();
				if ( t>len && !GetAttribute(kOut.x, kOut.y, NonPickingAttrib))
				{
					t = len;
					if ( pTriangle )
					{
						pTriangle[0] = v0;
						pTriangle[1] = v3;
						pTriangle[2] = v1;
					}
				}
			}
		}
	}

	if ( pOut )
		*pOut = vP + vDir*t;

	return t;
}

bool RHeightField::IsIntersect(const MBox &bbox)
{
	if ( !m_aabb.ContainsAABB(bbox))
		return false;
	
	// clipping x, y
	MBox box = bbox;
	if ( box.vmin.x < 0.0f )	box.vmin.x = 0.0f;
	if ( box.vmin.y < 0.0f )	box.vmin.y = 0.0f;
	if ( box.vmax.x > GetWidthSize())	box.vmax.x = GetWidthSize();
	if ( box.vmax.y > GetHeightSize())	box.vmax.y = GetHeightSize();

	int nX1 = (int)(box.vmin.x/GetXScale());
	int nY1 = (int)(box.vmin.y/GetYScale());
	int nX2 = (int)(box.vmax.x/GetXScale());
	int nY2 = (int)(box.vmax.y/GetYScale());
	
	_ASSERT(nX1<=nX2);
	_ASSERT(nY1<=nY2);

	int nCount = 0;
	const int &nPitch = GetHeightMapPitch();
	const float *pHeightMap = GetPtr();

	// 해당 영역을 순회하면서 AABB 에 충돌 가능성이 있는 위치를 검사한다.
	for ( int y = nY1; y <=nY2; ++y )
		for ( int x = nX1; x <=nX2; ++x )
		{
			MVector3 vLeftTop	= MVector3(x*GetXScale(), y*GetYScale(), pHeightMap[x + y*nPitch]);
			MVector3 vRightTop	= MVector3((x+1)*GetXScale(), y*GetYScale(), pHeightMap[x+1 + y*nPitch]);
			MVector3 vLeftBottom	= MVector3(x*GetXScale(),(y+1)*GetYScale(), pHeightMap[x + (y+1)*nPitch]);
			MVector3 vRightBottom	= MVector3((x+1)*GetXScale(),(y+1)*GetYScale(), pHeightMap[x+1 + (y+1)*nPitch]);
			++nCount;

			if (vLeftTop.z < box.vmin.z && vRightTop.z < box.vmin.z && vLeftBottom.z < box.vmin.z && vRightBottom.z < box.vmin.z )
				continue;	// 현재 섹터는 충돌 가능성이 없음.
			
			if ( box.Contains(vLeftTop) || box.Contains(vRightTop) || box.Contains(vLeftBottom) || box.Contains(vRightBottom))
				return true;	// 현제 섹터는 충돌

			// 애매한 상황, AABB vs Tri-Polygon 충돌을 검사해야 한다.
			if (box.ContainsPolygon(vLeftTop,vRightTop, vLeftBottom))
				return true;
			if (box.ContainsPolygon(vRightTop, vRightBottom, vLeftBottom))
				return true;
		}

	_ASSERT(((nX2-nX1 +1 )*(nY2-nY1 +1))==nCount);
	return false;
}
