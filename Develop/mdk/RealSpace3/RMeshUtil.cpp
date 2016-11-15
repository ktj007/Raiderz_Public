#include "stdafx.h"
#include "RMeshUtil.h"
#include "RDeviceD3D.h"
#include "MDebug.h"
#include "RFont.h"

/////////////////////////////////////////////////////////////////////////
// RIBuffer

namespace rs3 {
/////////////////////////////////////////////////////////////////////////
// RVBuffer

RMeshIndexBuffer::RMeshIndexBuffer()
{
	m_size	= 0;
	m_hIB	= R_NONE;
	m_i		= NULL;

	m_pIndex = NULL;

	m_dwUsage	 = D3DUSAGE_WRITEONLY;
	m_dwPool	 = D3DPOOL_MANAGED;
	m_dwLockFlag = 0;

	m_bUseSWVertex = false;
	m_bUseHWVertex = false;

	m_PrimitiveType = RPT_TRIANGLELIST;
}

RMeshIndexBuffer::~RMeshIndexBuffer() 
{
	if(REngine::GetDevicePtr())
		REngine::GetDevice().DeleteIndexBuffer(m_hIB);
	SAFE_DELETE_ARRAY(m_pIndex);
}

void RMeshIndexBuffer::Lock() 
{
	m_i = (WORD*)REngine::GetDevice().LockIndexBuffer(m_hIB,m_dwLockFlag);
}

void RMeshIndexBuffer::Unlock() 
{
	REngine::GetDevice().UnlockIndexBuffer(m_hIB);
}

int RMeshIndexBuffer::GetFaceCnt() 
{
	if(m_size)
		return m_size / 3;
	return 0;
}

void RMeshIndexBuffer::Update(int size,WORD* pData) 
{
	if(size!=m_size) 
		return;

	if(m_bUseHWVertex && m_hIB != R_NONE) {

		Lock();

		memcpy(m_i,pData,sizeof(WORD)*m_size);

		Unlock();
	}

	if(m_bUseSWVertex && m_pIndex)
		memcpy(m_pIndex,pData,sizeof(WORD)*m_size);
}

bool RMeshIndexBuffer::Create(int size,WORD* pData,DWORD flag,DWORD Usage,D3DPOOL Pool)
{
	m_size = size;

	if(flag & USE_VERTEX_HW) m_bUseHWVertex = true;
	if(flag & USE_VERTEX_SW) m_bUseSWVertex = true;

	if(m_bUseHWVertex) {

		m_hIB = REngine::GetDevice().CreateIndexBuffer(size,RFMT_INDEX16,NULL,Usage );

		if(m_hIB==R_NONE) {	// soft 로 돌수도 있다..
			mlog("RMeshIndexBuffer::Create Error : use soft index buffer\n");
		}
	}

	if(m_bUseSWVertex)
		m_pIndex = new WORD[size];

	if(pData)
		Update(size,pData);

	return true;
}

void RMeshIndexBuffer::SetIndices()
{
	REngine::GetDevice().SetIndexBuffer(m_hIB);
}

//////////////////////////////////////////////////////////////////

RMeshVertexBuffer::RMeshVertexBuffer()
{
	Init();
}

RMeshVertexBuffer::~RMeshVertexBuffer()
{
	Clear();
}

void RMeshVertexBuffer::Init() 
{
	m_is_init = false;
	m_hVB = R_NONE;
	m_nPosOffset = 0;
	m_nNormalOffset = 0;
	m_nVertexSize = 0;
	m_nVertexCnt = 0;
	m_nBufferSize = 0;
	m_nRealBufferSize = 0;
	m_v = NULL;
	m_hVF = R_NONE;
	m_hVFOnlyStream0 = R_NONE;

	m_dwUsage = D3DUSAGE_WRITEONLY;
	m_dwPool = D3DPOOL_MANAGED;
	m_dwLockFlag = 0;

//	m_PrimitiveType = RPT_TRIANGLELIST;

	m_nRenderCnt = 0;
	m_pVert = NULL;
	m_bUseSWVertex = false; // 기본은 둘다사용, 옵션에서 바꿀수도 있으므로.
	m_bUseHWVertex = false;
}

void RMeshVertexBuffer::Clear() 
{
	if(REngine::GetDevicePtr())
		REngine::GetDevice().DeleteVertexBuffer(m_hVB);

	SAFE_DELETE(m_pVert);

	Init();
}

bool RMeshVertexBuffer::Create(void *pVertex, HVERTEXFORMAT hvf, HVERTEXFORMAT hvfOnlyStream0, int nPosOffset, int nNormalOffset, int nVertexSize, int nVertexCnt, DWORD flag, DWORD Usage, D3DPOOL pool)
{
	Clear();

	m_nPosOffset = nPosOffset;
	m_nNormalOffset = nNormalOffset;
	m_nVertexSize = nVertexSize;
	m_nVertexCnt = nVertexCnt;
	m_nBufferSize = m_nVertexSize * m_nVertexCnt;
	m_nRealBufferSize = m_nBufferSize;

	m_nRenderCnt = m_nVertexCnt/3;	// triangle list

	m_hVF = hvf;
	m_hVFOnlyStream0 = hvfOnlyStream0;
	m_dwUsage = Usage;
	m_dwPool = pool;

	if(flag & USE_VERTEX_SW) m_bUseSWVertex = true;
	if(flag & USE_VERTEX_HW) m_bUseHWVertex = true;

	// 버퍼는 필요에 의해서 채우고

	if(m_bUseHWVertex) {
		m_hVB = REngine::GetDevice().CreateVertexBuffer(m_nBufferSize, m_nVertexSize);

		if(m_hVB==R_NONE) // tnt 등의 경우 soft 로 돌수도 있다..
			mlog("RMeshVertexBuffer::Create Error : use soft vertex buffer\n");

	}

	if(m_bUseSWVertex) {
		m_pVert = new char[m_nBufferSize];
	}

	m_is_init = true;

	if(pVertex)
		return Update((char*)pVertex, m_nVertexSize, m_nVertexCnt);

	return true;
}

bool RMeshVertexBuffer::UpdateDataSW(char* pVertex)
{
	if(m_bUseSWVertex && m_pVert) {
		memcpy(m_pVert,pVertex,m_nBufferSize);
	}

	return true;
}

bool RMeshVertexBuffer::UpdateDataHW(char* pVertex)
{
	if(m_bUseHWVertex && m_hVB != -1) {

		Lock();

		memcpy( m_v, pVertex, m_nBufferSize );

		Unlock();
	}

	return true;
}

bool RMeshVertexBuffer::UpdateData(char* pVertex)
{
	UpdateDataHW(pVertex);
	UpdateDataSW(pVertex);

	return true;
}

// 범용처리는 아닌..
bool RMeshVertexBuffer::UpdateData(RVector* pVec)
{
	if( !m_is_init )
		return false;

	if( m_hVB != -1 )
	{
		Lock();
		UpdateDataSub( m_v, pVec );
		Unlock();
	}

	if( m_bUseSWVertex && m_pVert )
	{
		UpdateDataSub( m_pVert, pVec );
	}

	return true;
}

void RMeshVertexBuffer::UpdateDataSub( char* pDest, RVector* pVec )
{
	int cursor = m_nPosOffset;
	int vecCursor = 0;
	for( int i = 0; i < m_nVertexCnt; ++i )
	{
		memcpy( &pDest[cursor], &pVec[vecCursor], sizeof(RVector) );
		cursor += m_nVertexSize;
		vecCursor += sizeof(RVector);
	}
}

bool RMeshVertexBuffer::Update(char* pVertex,int VertexSize,int VertexCnt) 
{
	if(!m_is_init) 	return false;

	if(m_nVertexSize != VertexSize)
		return false;

	int BufferSize = VertexSize * VertexCnt;

	if(m_nBufferSize != BufferSize) {
		if(m_nRealBufferSize < BufferSize) { // 진짜 버퍼보다 작다면 그냥 쓰자.
			if(Create(pVertex, m_hVF, m_hVFOnlyStream0, m_nPosOffset, VertexSize, VertexCnt, m_dwUsage, m_dwPool)==false)
				return false;
		}
		else 
		{
			m_nBufferSize = BufferSize; // m_nRealBufferSize 는 따로다..
			m_nVertexCnt = VertexCnt;
		}
	}

	return UpdateData( pVertex );
}

void RMeshVertexBuffer::Lock() 
{
	m_v = (char*)REngine::GetDevice().LockVertexBuffer(m_hVB,m_dwLockFlag);
}

void RMeshVertexBuffer::Unlock() 
{
	REngine::GetDevice().UnlockVertexBuffer(m_hVB);
}

void RMeshVertexBuffer::SetStreamSource()
{
	REngine::GetDevice().SetVertexBuffer(m_hVB,0,0);
}

void RMeshVertexBuffer::Render() {

	if(!m_is_init) 	return;

	SetStreamSource();
	REngine::GetDevice().DrawPrimitive( m_PrimitiveType, 0, m_nVertexCnt/3);
}

void RMeshVertexBuffer::RenderSoft()
{
	if(!m_is_init) 	return;

	REngine::GetDevice().SetVertexFormat( m_hVFOnlyStream0 );
	REngine::GetDevice().DrawPrimitiveUP(m_PrimitiveType, m_nVertexCnt/3, (LPVOID) m_pVert, m_nVertexSize);
}

// soft 버퍼만 지원한다..  Vertex Type 인경우..lvert blendvert
// 우선테스트해본다..괜찮으면 버퍼를 만들던가 한다..
void RMeshVertexBuffer::ConvertSilhouetteBuffer(float fLineWidth)
{
	if( !m_pVert )
		return;

	// normal 이 없으면 의미 없다..
	if( m_nNormalOffset )
	{
		RVector vPos;
		RVector vNormal;

		int cursorPos = m_nPosOffset;
		int cursorNormal = m_nNormalOffset;
		for( int i = 0; i < m_nVertexCnt; ++i )
		{
			memcpy( &vPos, &m_pVert[cursorPos], sizeof(RVector) );
			memcpy( &vNormal, &m_pVert[cursorNormal], sizeof(RVector) );
			RVector vResult = vPos + vNormal * fLineWidth;
			memcpy( &m_pVert[cursorPos], &vResult, sizeof(RVector) );

			cursorPos += m_nVertexSize;
			cursorNormal += m_nVertexSize;
		}
	}
}

void RMeshVertexBuffer::ReConvertSilhouetteBuffer(float fLineWidth)
{
	if(!m_pVert) return;

	// normal 이 없으면 의미 없다..
	if( m_nNormalOffset )
	{
		RVector vPos;
		RVector vNormal;

		int cursorPos = m_nPosOffset;
		int cursorNormal = m_nNormalOffset;
		for( int i = 0; i < m_nVertexCnt; ++i )
		{
			memcpy( &vPos, &m_pVert[cursorPos], sizeof(RVector) );
			memcpy( &vNormal, &m_pVert[cursorNormal], sizeof(RVector) );
			RVector vResult = vPos - vNormal * fLineWidth;
			memcpy( &m_pVert[cursorPos], &vResult, sizeof(RVector) );

			cursorPos += m_nVertexSize;
			cursorNormal += m_nVertexSize;
		}
	}
}

void RDrawLine(RVector &v1,RVector &v2,DWORD dwColor)
{
	struct LVERTEX
	{ 
		float x, y, z; 
		DWORD dwColor; 
	};

	LVERTEX ver[2] =
	{
		{v1.x,v1.y,v1.z,dwColor},
		{v2.x,v2.y,v2.z,dwColor}
	};

	HRESULT hr = REngine::GetDevice().DrawPrimitiveUP(RPT_LINELIST,1,ver,sizeof(LVERTEX));
}

void RMeshVertexBuffer::RenderVertexNormal()
{
	RVector vPos;
	RVector vNormal;

	int cursorPos = m_nPosOffset;
	int cursorNormal = m_nNormalOffset;
	for( int i = 0; i < m_nVertexCnt; ++i )
	{
		memcpy( &vPos, &m_pVert[cursorPos], sizeof(RVector) );
		memcpy( &vNormal, &m_pVert[cursorNormal], sizeof(RVector) );

		RDrawLine( vPos, vPos + vNormal * 3.f, 0xffff0000 );

		cursorPos += m_nVertexSize;
		cursorNormal += m_nVertexSize;
	}
}

void RMeshVertexBuffer::RenderIndexSoft(WORD* pIndex,int nCount,RPRIMITIVETYPE type)
{
	RPFC_THISFUNC;
	if(!m_is_init) 	return;

	RDevice *pDevice = REngine::GetDevicePtr();

	if(pDevice==NULL) return;

	pDevice->SetVertexFormat( m_hVFOnlyStream0 );
	pDevice->DrawIndexedPrimitiveUP(type,0,
		m_nVertexCnt,
		nCount,
		pIndex,
		(LPVOID) m_pVert,m_nVertexSize);
}

void RMeshVertexBuffer::RenderIndexBuffer(int nOffset,int nCount,RPRIMITIVETYPE type)
{
	if(!nCount) return;

	REngine::GetDevice().DrawIndexedPrimitive(type,0, 0,m_nVertexCnt,nOffset,nCount );
}

void RMeshVertexBuffer::Render(RMeshIndexBuffer* ib )
{
	if(!m_is_init) 	return;
	if(!ib)			return;

	SetStreamSource();
	ib->SetIndices();
//	dev->SetStreamSource( 0, m_vb, 0, m_nVertexSize );
//	dev->SetIndices(ib->m_ib);
	REngine::GetDevice().DrawIndexedPrimitive(m_PrimitiveType,0, 0,m_nVertexCnt,0,ib->GetFaceCnt() );
}

void RMeshVertexBuffer::SetVertexBuffer()
{
	REngine::GetDevice().SetVertexFormat(m_hVF);
	SetStreamSource();
}

char* RBaseObject::GetName()
{
	return (char*)m_Name.c_str();
}

void RBaseObject::SetName(const char* name)
{
	m_Name = name;
}

bool RBaseObject::CheckName(string& name)
{
	if(m_Name==name)
		return true;
	return false;
}

bool RBaseObject::CheckName(const char* name) 
{
	return CheckName(string(name));
}

void SubCalcBBox(RVector* max,RVector* min,RVector* v)
{
	if ( (max==NULL) || (min==NULL) || (v==NULL) ) return;

	min->x = min(min->x, v->x);
	min->y = min(min->y, v->y);
	min->z = min(min->z, v->z);

	max->x = max(max->x, v->x);
	max->y = max(max->y, v->y);
	max->z = max(max->z, v->z);
}

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

}