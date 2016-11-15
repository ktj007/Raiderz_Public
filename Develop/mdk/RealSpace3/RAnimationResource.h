#pragma once

#include "RPreDefine.h"
#include "RAnimationDef.h"
#include "RResource.h"
#include <hash_map>
#include "RFile.h"

namespace rs3 {

class RAnimationNode;
class RAnimationManager;
class RMemoryFile;

class RS_API RAnimationResource : public RResource
{
public:
	DECLARE_USING_RES_MGR(RAnimationManager);

	RAnimationResource(const char* _pSzResourceName, RSceneManager* pSceneManager);
	~RAnimationResource();

	AnimationType GetAnimationType();

	int GetMaxFrame();
	int GetAniNodeCount();

	RAnimationNode* GetAniNode(int i);
	RAnimationNode* GetBipRootNode();

	RAnimationNode* GetNode(const string& strName);
	int				GetNodeIndex(const string& strName);	///< 이름으로 인덱스를 얻는다, 없으면 -1 을 리턴

	bool SetMaxAniBoundingBox(RBoundingBox& box);

	RBoundingBox* GetMaxAniBoundingBox() { return m_pMaxAniBoundingBox; }

	DWORD GetFileVersion() { return m_header.ver; }

protected:
	virtual bool		Fill() override;

	bool LoadAniHeader(RFile& rf, const char* filename);
	bool LoadAniNode(RFile& rf, const char* filename);
	bool LoadAni(const char* filename);
	bool SaveAni();
	bool LoadVisibilityKey(RFile& rf, RAnimationNode* pANode);

	ex_ani_t			m_header;
	bool				m_bheaderLoadComplete;

	RAnimationNode**	m_pAniNodeTable;
	int					m_nAniNodeCnt;
	AnimationType		m_AniType;

	RAnimationNode*		m_pBipRootNode;

	int					m_nMaxFrame;
	RBoundingBox*		m_pMaxAniBoundingBox;

	stdext::hash_map<string,int>		m_nodeIndexMap;		///< 이름 -> index

};

//////////////////////////////////////////////////////////////////////////
// inlines

inline AnimationType RAnimationResource::GetAnimationType()		{ return m_AniType;		}

inline int RAnimationResource::GetMaxFrame()					{ return m_nMaxFrame;	}

inline int RAnimationResource::GetAniNodeCount()				{ return m_nAniNodeCnt;	}

inline RAnimationNode* RAnimationResource::GetAniNode(int i)	{ return m_pAniNodeTable[i]; }

inline RAnimationNode* RAnimationResource::GetBipRootNode()		{ return m_pBipRootNode; }

}