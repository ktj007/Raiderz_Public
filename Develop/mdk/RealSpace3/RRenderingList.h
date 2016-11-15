#pragma once

#include "RRenderChain.h"
namespace rs3
{

	class RTreeResource;
	class RTreeSceneNode;
	class RActorNode;
	class RMeshNode;

	class RActorNodeKey		
	{
	public:
		RActorNodeKey(void);
		~RActorNodeKey(void);

	public:
		bool operator<( const RActorNodeKey& Right) const;	//std::map에 키로 들어가기 때문에 비교연산자를 정의 해주어야함

	public:
		RMaterial*		m_pMaterial;
		int				m_nSubMaterial;
		int				m_nNodeType;
		// 랜더 순회 시 VF셋팅의 변화가 적도록 하기 위해
		HVERTEXFORMAT	m_hVF;
		// 인스턴싱 구축 시 한 메쉬 종류로만 이루어 지도록.
		RMeshNode*		m_pMeshNode;
	};

	class RShadowActorNodeKey : public RActorNodeKey
	{
	public:
		bool operator<( const RShadowActorNodeKey& Right) const;	//std::map에 키로 들어가기 때문에 비교연산자를 정의 해주어야함
	};

	typedef std::map<RActorNodeKey,SCENE_SET*>		MAP_ACTORNODE;
	typedef std::pair<RActorNodeKey,SCENE_SET*>		PAIR_ACTORNODE;

	typedef std::map<RTreeResource*,SCENE_SET*>		MAP_TREECHAIN;
	typedef std::pair<RTreeResource*,SCENE_SET*>	PAIR_TREECHAIN;

	typedef std::multimap<RSceneNode*,SCENE_SET*>	MULTIMAP_ACTORNODE_TO_SET;
	typedef std::pair<RSceneNode*,SCENE_SET*>		PAIR_ACTORNODE_TO_SET;
	typedef std::pair<MULTIMAP_ACTORNODE_TO_SET::iterator,MULTIMAP_ACTORNODE_TO_SET::iterator> PAIR_ITER_ACTORNODE_TO_SET;

	

	class RRenderingList
	{
	public:
		RRenderingList(void);
		~RRenderingList(void);

	public://유틸리티
		void	Clear(void);
		bool	IsEmpty(void);
		void	Update(void);
		bool	Equal(RRenderingList& rhs);

	//-------------------------------------------------------------------------------[ 일반노드용 ]
	public:
		void				Add(RSceneNode *pNode,RRENDERPASS pass);
		void				Remove(RSceneNode *pNode,RRENDERPASS pass);
		SCENE_SET*			GetRenderingList(RRENDERPASS pass)	{	return &m_Set[pass];			}
	private:
		SCENE_SET		m_Set[RRP_MAX];

	//-------------------------------------------------------------------------------[ 액터용 ]
	//리소스가 아직 로딩되지 않은 액터용
	public:
		void				AddNullActor(RSceneNode* pNode);
		void				RemoveNullActor(RSceneNode* pNode);
		bool				IsInNullActor(RSceneNode* pNode);
	private:
		std::set<RSceneNode*>		m_setNullActor;				//리소스가 아직 로딩되지 않은 액터들

	public:
		//알파2패스 액터용
		void				AddAlpha2PassActor(RSceneNode* pNode);
		void				RemoveAlpha2PassActor(RSceneNode* pNode);

		SCENE_SET*			GetAlpha2PassActor(void)	{	return &m_Alpha2PassActorSet;	}

	private:
		SCENE_SET			m_Alpha2PassActorSet;


	//-------------------------------------------------------------------------------[ 액터노드용 ]
	public:
		void				RemoveActorNode(RSceneNode* pNode);
		virtual void		AddActorNode(RSceneNode* pNode,RActorNodeKey& key)					{	m_ActorNodeRenderChain.AddSceneNode(pNode,key);					}
		virtual void		AddAlphaAdditiveActorNode(RSceneNode* pNode,RActorNodeKey& key)		{	m_AlphaAdditiveActorNodeRenderChain.AddSceneNode(pNode,key);	}
		virtual void		AddAlphaActorNode(RSceneNode* pNode,RActorNodeKey& key)				{	m_mapAlphaActorNodeRenderChain.AddSceneNode(pNode,key);			}
		virtual void		AddShadowActorNode(RSceneNode* pNode,RShadowActorNodeKey& key)		{}
		virtual void		AddDoubleShadowActorNode(RSceneNode* pNode,RShadowActorNodeKey& key)	{}
		
		RRenderChain<RActorNodeKey>*	GetActorNodeRenderChain(void)				{	return &m_ActorNodeRenderChain;					}
		RRenderChain<RActorNodeKey>*	GetAlphaAdditiveActorNodeRenderChain(void)	{	return &m_AlphaAdditiveActorNodeRenderChain;	}
		RRenderChain<RActorNodeKey>*	GetAlphaActorNodeRenderChain(void)			{	return &m_mapAlphaActorNodeRenderChain;			}
		RRenderChain<RShadowActorNodeKey>*	GetShadowActorNodeRenderChain(void)			{	return &m_mapShadowActorNodeRenderChain;			}
		RRenderChain<RShadowActorNodeKey>*	GetDoubleShadowActorNodeRenderChain(void)			{	return &m_mapDoubleShadowActorNodeRenderChain;			}

	private:
		RRenderChain<RActorNodeKey>	m_ActorNodeRenderChain;
		RRenderChain<RActorNodeKey>	m_AlphaAdditiveActorNodeRenderChain;
		RRenderChain<RActorNodeKey>	m_mapAlphaActorNodeRenderChain;
	protected:
		RRenderChain<RShadowActorNodeKey>	m_mapShadowActorNodeRenderChain;
		RRenderChain<RShadowActorNodeKey>	m_mapDoubleShadowActorNodeRenderChain;

	//-------------------------------------------------------------------------------[ 트리용 ]
	public:
		void	AddTreeChain(RTreeSceneNode *pNode);
		void	RemoveTreeChain(RTreeSceneNode* pNode);
		MAP_TREECHAIN*		GetTreeRenderChain(void)			{	return &m_mapTreeChain;			}
		
	private:
		SCENE_SET*	FindTreeChainSet(RTreeResource* pResource);
		SCENE_SET*  CreateTreeChainSet(RTreeResource* pResource);
		void		ClearTreeChain(void);
	private:
		MAP_TREECHAIN				m_mapTreeChain;				
	};

	class RShadowRenderingList : public RRenderingList
	{
	public:
		virtual void		AddActorNode(RSceneNode* pNode,RActorNodeKey& key)					{}
		virtual void		AddAlphaAdditiveActorNode(RSceneNode* pNode,RActorNodeKey& key)		{}
		virtual void		AddAlphaActorNode(RSceneNode* pNode,RActorNodeKey& key)				{}
		virtual void		AddShadowActorNode(RSceneNode* pNode,RShadowActorNodeKey& key)		{ m_mapShadowActorNodeRenderChain.AddSceneNode(pNode,key); }
		virtual void		AddDoubleShadowActorNode(RSceneNode* pNode,RShadowActorNodeKey& key)		{ m_mapDoubleShadowActorNodeRenderChain.AddSceneNode(pNode,key); }
	};
}
