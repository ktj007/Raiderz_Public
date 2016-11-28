#ifndef _RMesh_h
#define _RMesh_h

#include "RPreDefine.h"
#include <vector>
#if (_MSC_VER >= 1900)
#include <unordered_map>
#else
#include <hash_map>
#endif

#include "RMeshUtil.h"
#include "RMeshAnimationMgr.h"
#include "RResource.h"
#include "ROccluderContext.h"
#include "RLODMetric.h"

namespace rs3 {

////////////////////////////////////////////////////////////////////////////////////////////////////

//#pragma warning(disable : 4996)

class RSceneManager;
class RMeshMgr;
class RActor;

class RMeshNode;
class RActorNode;
class RActorNodePhysique;

// for collision
class RMeshCollision;

// for m_NodeTable / m_nDataNum
class RMaterialResource;

typedef RHashList<RMeshNode*>			RMeshNodeHashList;
typedef RHashList<RMeshNode*>::iterator	RMeshNodeHashList_Iter;

////////////////////////////////////////////////////////////////////////////////////////////////////




class RS_API RMesh : public RResource, public RLODMetric
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// friend
	//
	friend RMeshMgr;			// resource manager

	friend RActor;
	friend RMeshNode;			// m_hVF 계열, m_nFileVersion

	friend RActorNode;			// for m_NodeTable
	friend RActorNodePhysique;	// for m_NodeTable 

public:

	RMesh(const char* _pSzResourceName, RSceneManager* _pSceneManager);
	virtual ~RMesh();

	DECLARE_USING_RES_MGR(RMeshMgr);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// shared resource : vertex format, default material ...
	//
	static bool					InitSharedResources(RDevice *pDevice);
	static bool					DestroySharedResources();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 리소스 메모리 사용량
	//
	virtual int					GetUsingSystemMemory()	{ return m_nMaterialUsingSystemMemory + m_nVertexUsingSystemMemory; }
	virtual int					GetUsingVideoMemory()	{ return m_nMaterialUsingVideoMemory + m_nVertexUsingVideoMemory; }
	

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메시 노드 리스트
	//
	RMeshNode*					GetNode(const char* name){ return m_NodeHashList.Find(name); }
	RMeshNode*					GetNode(int i) { return m_NodeTable[i]; }
	size_t						GetNodeCount() { return m_NodeTable.size(); }
	void						DelMeshList();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation
	//
	RMeshAnimationMgr*			GetAnimationMgr(void);
	RMeshAnimationMgr			m_AniMgr;											// 모션관리자


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BoundingBox
	//
	bool						GetStaticSkeletonBoundingBox(RBox& box);
	RBoundingBox&				GetStaticBBox(){ return m_staticBBox; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 캐릭터툴에서 편집하는 정보들
	//
	RMeshCollision*				m_pMeshCollision;							// 충돌
	unsigned int				GetMaterialResourceCount();					///< 재질의 수를 얻어냅니다
	const RMaterialResource*	GetMaterialResource(int nIndex)	;			///< 재질을 얻어냅니다
	int							GetFileVersion(){ return m_nFileVersion; }	// 파일버전
	void						SetFileVersion(int nVersion){ m_nFileVersion = nVersion; }
	// boolean

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 툴에서 사용 하기 위해외부에서 데이터를 관여
	// RMesh의 데이터만 재 로딩 한다고 해서 정상적으로 작동되지 않는다.
	// 메쉬노드의 정보도 갱신되야 해서 메쉬노드를 가지는 액터를 다시 갱신 하여야함.
	// bool						ReFill()		{ return ( Erase() && Fill()); };	// 우선은 RMesh만 제기능으로 작동하니 베이스에 안놓고.. - 090608, OZ

public:
	static	RMeshNode*			loadClothMeshNodeELUFile(std::string fileName);	//Fill()함수와 반드시 동일한 형태를 지녀야 한다.
																				//Fill()함수를 수정하면 이 함수도 수정해야 한다.
	bool						IsAlpha2PassRendering() { return m_isAlpha2PassRendering; }
	bool						IsAutoAnimation() { return m_bAutoAnimation; }
	bool						IsLoadedAnimaitonList() { return IsExsitAnimationListFile(); }
	bool						IsExsitAnimationListFile() { return m_bExistAnimationListFile; }
protected:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 리소스 로드
	//
	virtual bool				Fill() override;
	virtual bool				Erase() override;
	virtual LOAD_RESULT			Load() override;

	void						PushNode( RMeshNode* pMeshNode );

	vector<RMaterialResource*>	m_vecMtrlList;								// 재질관리자
protected:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read File(XML)
	//
	bool						ReadDescXml(const char* fname,bool bBackground);
	bool						ReadUserPropertyElements(MXmlElement* pSceneElement);
	bool						ReadClothElements(MXmlElement* pClothListElement);

private:
	//ELU파일
	bool						LoadEluFile(const string& EluFileName);

	//유저프로퍼티
	void						FillMeshNodeUserPropertyInfo(void);

	int							m_nFileVersion;
	vector<pair<string, string>> m_vecUserProperty;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// "fill" (io) 관련
	//
	void						ConnectMatrix();
	void						CalcStaticBoundingBox();

	RBoundingBox				m_staticBBox;		// 기본 모델의 bounding box

	RMeshNodeHashList			m_NodeHashList;
	vector<RMeshNode*>			m_NodeTable;
	int							m_nDataNum;			// m_NodeTable 메시 노드의 개수
	int							m_nID;				// node의 아이디


//	RMeshEffectList				m_meshEffectList;
//	vector<RSceneNode*>			m_meshLightList;
	RPickType					m_PickingType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// "load" (device) 관련
	//
	void						MakeAllNodeVertexBuffer();

	static RMaterialResource*	m_pDefaultMaterial;	// 기본 material
	bool						m_isPhysiqueMesh;
	bool						m_isAlpha2PassRendering; // Alpha2Pass렌더링 여부
	bool						m_bAutoAnimation;
	bool						m_bExistAnimationListFile;	// 애니메이션 리스타가 존재 여부

	RSceneManager*		m_pSceneManager;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메모리 사용
	//
	int							m_nMaterialUsingSystemMemory;
	int							m_nMaterialUsingVideoMemory;
	int							m_nVertexUsingSystemMemory;
	int							m_nVertexUsingVideoMemory;
};

//////////////////////////////////////////////////////////////////////////
inline unsigned int RMesh::GetMaterialResourceCount() { return m_vecMtrlList.size(); }

}



#endif//_RMesh_h
