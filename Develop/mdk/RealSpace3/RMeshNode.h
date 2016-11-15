#ifndef _RMeshNode_h
#define _RMeshNode_h

#include "RPreDefine.h"
#include "RMeshNodeData.h"
#include "RSkeleton.h"
#include "RMaterial.h"
#include "MMemPool.h"

namespace rs3 {




class RActorNodeInstancingBuffer
{
#define MAX_MESH_INSTANCE	20		// instancing.rsc의 것과 맞추3~

private:
	HVERTEXBUFFER			m_hInstancingBuffer;
	RDevice*				m_pDevice;

public:
	RActorNodeInstancingBuffer() :
	  m_hInstancingBuffer(R_NONE), m_pDevice(NULL)
	  {
	  }
	  ~RActorNodeInstancingBuffer()
	  {
		  Destroy();
	  }

public:
	void	Destroy()
	{
		if( m_hInstancingBuffer != R_NONE && m_pDevice != NULL)
		{
			m_pDevice->DeleteVertexBuffer(m_hInstancingBuffer);
			m_hInstancingBuffer = R_NONE;
			m_pDevice = NULL;
		}
	}

	void	Create( RDevice* pDevice)
	{
		m_pDevice = pDevice;
		if(!m_hInstancingBuffer)
		{
			int nInstancingCnt = MAX_MESH_INSTANCE;
			int nStructSize = sizeof(float);	// 인덱스만을 가짐.
			int bufferSize = nInstancingCnt * nStructSize;
			// 데이터 채움. 오로지 순차적인 인덱스만 있으면 됨.
			float* pData = new float[ nInstancingCnt];
			for( int i=0; i<nInstancingCnt; i++)
				pData[i] = (float)i;
			// 생성시 인덱스로만 한번만 채워주고 그 후로는 바뀔 일이 없으므로 다이나믹으로 하지 않는다.
			m_hInstancingBuffer = m_pDevice->CreateVertexBuffer( bufferSize, nStructSize, pData);
			// 데이터 지워줌,
			SAFE_DELETE_ARRAY( pData);
		}
	}

	HVERTEXBUFFER	GetBuffer()					{ return m_hInstancingBuffer;	}
	int				GetMaxSize()				{ return MAX_MESH_INSTANCE;	}
};

/// LOD 정보를 관리하는 Class
class RS_API RMeshLODNode
{
public:
	RMeshLODNode() : m_nLODLevel(0)	{}
	virtual ~RMeshLODNode()			{}

private:
	int	m_nLODLevel;

public:
	void	SetLODLevel( int nLv)	{ m_nLODLevel = nLv;	}
	int		GetLODLevel()			{ return m_nLODLevel;	}
};	



enum RMESH_ALIGN : int {
	RMA_NONE = 0,
	RMA_NORMAL = 1,
	RMA_Z_FIXED = 2
};

//////////////////////////////////////////////////////////
// 노드가 가지는 속성을 관리하는 Class

class RS_API RMeshNodeInfo
{
public:
	RMeshNodeInfo();
	virtual ~RMeshNodeInfo();

	bool	isVisibleMesh()		{ return !GetFlag(RM_FLAG_HIDE); }
	bool	isCollisionMesh()	{ return GetFlag(RM_FLAG_COLLISION_MESH); }
	bool	isDummyMesh()		{ return GetFlag(RM_FLAG_DUMMY_MESH); }
	bool	isClothMesh()		{ return GetFlag(RM_FLAG_CLOTH_MESH); }
	bool	isBone()			{ return GetFlag(RM_FLAG_BONE_MESH); }
	bool	isLODMesh()			{ return GetFlag(RM_FLAG_LOD_MESH); }

	void	SetFlag(DWORD dwFlag) { m_dwFlag = dwFlag; }
	bool	GetFlag(DWORD dwFlag) { return (m_dwFlag&dwFlag) ? true:false; }
	void	AddFlag(DWORD dwFlag) { m_dwFlag |= dwFlag; }

public:

	DWORD					m_dwFlag;				
	RMESH_ALIGN				m_nAlign;				// Align Type

	//////////////////////////////////////////////////////////////////////////
	// CLOTH // TODO
	//////////////////////////////////////////////////////////////////////////
	int						m_nClothWidth;			// 옷감 크기
	int						m_nClothHeight;			//
	int						m_nSeam[4];				// Seam 영역 Vertex의 Index // Max의 인덱스 -1 로 변환할것!

	float					cCloth_fGravity;		// 중력
	bool					cCloth_bShear;			// 대각선 노드
	float					cCloth_fRelax;			// 유연성
	float					cCloth_fWindForce;		// 바람세기
	float					cCloth_fWindHead;		// 바람의 방향(Y축 Degree)
	float					cCloth_fWindAmp;		// 바람의 진폭(Noise Texture * Amp)

	void					SetWidth(int nWidth){ m_nClothWidth = nWidth;}
	void					SetHeight(int nHeight){ m_nClothHeight = nHeight;}
	int						GetWidth(){ return m_nClothWidth;}
	int						GetHeight(){ return m_nClothHeight;}

};

////////////////////////////////////////////////////////////////////////
// 노드의 버퍼 관리를 담당한다. ( 인덱스 버퍼와 , 버텍스 버퍼 )
// TODO: 삭제후 RMeshNode 로 통합
class RS_API RBatch
{
public:
	RBatch();
	virtual ~RBatch();

	bool CreateVertexBuffer(char* pVert, const RVertexOffsetInfo& info, HVERTEXFORMAT hvf, HVERTEXFORMAT hvfOnlyStream0, int vert_num, DWORD flag);
	
	bool UpdateVertexBuffer(char* pVert);
	bool UpdateVertexBufferSoft(char* pVert);

	bool CreateIndexBuffer(WORD* pIndex,int _size);

	// Vertex 상태 정하기
	inline void SetVertexStream()
	{
		m_pVb->SetVertexBuffer();
		m_pIb->SetIndices();
	}

	int GetUsingSystemMemory();
	int GetUsingVideoMemory();

public:

	RMeshIndexBuffer*		m_pIb;
	RMeshVertexBuffer*		m_pVb;

#ifdef MESH_STRIP_TEST
	RMeshIndexBuffer*		m_pIbDefault;
	RMeshIndexBuffer*		m_pIbOptimized;
	RMeshIndexBuffer*		m_pIbStrips;
#endif
	bool				m_bIsMakeVertexBuffer;

	static bool			m_pUseHWShader;
	static bool			m_bisToolApp;
	static RVector3		m_vToolLightPos;
};
//----------------------------------------------------------------------------------------------------
inline int RBatch::GetUsingSystemMemory()
{
	int nSize = 0;
	if (m_pIb)
		nSize += m_pIb->GetSize();

	if (m_pVb)
		nSize += m_pVb->GetUsingSystemMemory();

	return nSize;
}
//----------------------------------------------------------------------------------------------------
inline int RBatch::GetUsingVideoMemory()
{
	int nSize = 0;
	if (m_pIb)
		nSize += m_pIb->GetSize();

	if (m_pVb)
		nSize += m_pVb->GetUsingVideoMemory();

	return nSize;
}
//----------------------------------------------------------------------------------------------------

typedef std::map< string, HVERTEXFORMAT > MAP_VERTEX_FORMAT;

class RS_API RMeshNode :public RMeshNodeData , public RMeshNodeMtrl, public RMeshNodeInfo , public RBatch, public RMeshLODNode
{
	friend RMesh;

public:
	RMeshNode();
	virtual ~RMeshNode();

	bool MakeNodeBuffer(DWORD flag);					// 버텍스 버퍼의 생성.

private:
	bool ConnectMtrl();									// 재질과 관리용 포인터 연결.

	void MakeVertexInfo(RVertexOffsetInfo& info, HVERTEXFORMAT* pVF, HVERTEXFORMAT* pVFOnlyStream0);

	static MAP_VERTEX_FORMAT	m_mapVertexFormat;

public:
	bool isRenderAble();								// 그려져야하는 노드인가?
	bool isPhysique();									// physique 메쉬인가 ?
	bool isHWPhysique();								// 하드웨어 physique 인가..
	bool hasAlphaMaterial();							// 알파 (additive 및 blending) material을 포함하는가
	bool hasAdditiveMaterial();							// additive material 을 포함하는가
	bool hasTranslucentMaterial();						// 반투명 material 을 포함하는가

	bool CheckMaterialFlag(DWORD dwFlag);				//해당 flag를 가지고 있는 매테리얼이 있는지 검사

	void UpdateNodeBuffer();							// 버텍스 버퍼의 갱신.

	///////////////////////////////////////////
	// reference pointer

	RMesh*			m_pParentMesh;						// 자신의 부모 모델 파일...
	RMesh*			m_pBaseMesh;						// 계층형 기본 모델 파일...
	RMeshNode*		m_pParent;							// 부모 노드..

	RMatrix			m_matBase;		// local-> max상의 world로의 변환행렬 (부모의 변환 포함)
	RMatrix			m_matInvBase;		// matBase 의 inverse

	RBIPID			GetBipId();

	int				GetUserPropertyCount();
	const string&	GetUserPropertyString(int nIndex);
	bool			FindUserProperty(const std::string& value);

public:
	RBIPID			m_bipID;

protected:
	vector<string>	m_userPropertyString;
};

/////////////////////////////////////////////////
inline RBIPID RMeshNode::GetBipId() { return m_bipID; }
inline bool RMeshNode::isPhysique() { return (m_nPhysiqueCnt>0);}
inline bool RMeshNode::isHWPhysique() { return ( isPhysique() && RBatch::m_pUseHWShader); }
inline bool RMeshNode::isRenderAble() { return ( !isDummyMesh() || (m_nFaceCnt>0)); }

inline int	RMeshNode::GetUserPropertyCount()	{ return (int)m_userPropertyString.size(); }
inline const string& RMeshNode::GetUserPropertyString(int nIndex) { return m_userPropertyString[nIndex]; }
inline bool RMeshNode::FindUserProperty(const std::string& value)
{
	for ( vector<string>::iterator itr = m_userPropertyString.begin(); itr != m_userPropertyString.end(); ++itr )
	{
		if ( *itr == value)
			return true;
	}

	return false;
}


class RS_API RMeshNodeMLeaf : public RMeshNode, public MMemPool<RMeshNodeMLeaf>
{
	friend RMesh;
public:
	RMeshNodeMLeaf() : RMeshNode() {}
	virtual ~RMeshNodeMLeaf() {}
};

}

#endif//_RMeshNode_h
