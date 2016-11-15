#pragma once

namespace rs3
{
	template<typename KEY_TYPE>
	class RRenderChainIterator;//전방선언

	template<typename KEY_TYPE>
	class RRenderChain
	{
		friend RRenderChainIterator<KEY_TYPE>;

		typedef std::map<KEY_TYPE,SCENE_SET*>	KEY_TO_SCENESET;
		typedef std::multimap<RSceneNode*,SCENE_SET*>	MULTIMAP_SCENENODE_TO_SET;

	public:
		RRenderChain(void);
		~RRenderChain(void);

	public:
		void	AddSceneNode(RSceneNode *pNode,KEY_TYPE& key);
		void	RemoveSceneNode(RSceneNode *pNode);

		bool	IsEmpty(void);
		void	Clear(void);
		bool	Equal(RRenderChain<KEY_TYPE> &rhs);

		typename KEY_TO_SCENESET::iterator	Begin(void)	{	return m_mapRenderChain.begin();	}
		typename KEY_TO_SCENESET::iterator	End(void)	{	return m_mapRenderChain.end();		}

	private:
		SCENE_SET*	FindSceneNodeSet(KEY_TYPE& key);
		SCENE_SET*	CreateSceneNodeSet(KEY_TYPE& key);
		void		RemoveSceneNodeSet(SCENE_SET* pSet);

	private:
		KEY_TO_SCENESET				m_mapRenderChain;	
		MULTIMAP_SCENENODE_TO_SET	m_multimapSceneNodeToSet;
	};

	
	template<typename KEY_TYPE>
	class RRenderChainIterator
	{
		friend RRenderChain<KEY_TYPE>;
	public:
		RRenderChainIterator(void)	{}
		~RRenderChainIterator(void)	{}
	public:
		
		
		typename RRenderChain<KEY_TYPE>::KEY_TO_SCENESET::iterator&	operator= (typename RRenderChain<KEY_TYPE>::KEY_TO_SCENESET::iterator& iter)
		{	
			m_Iter = iter;	
			return m_Iter;
		}
		
		
		bool	operator!=(typename RRenderChain<KEY_TYPE>::KEY_TO_SCENESET::iterator& iter)	{	return m_Iter != iter;	}

		void	Next(void)	{	++m_Iter;	}

		KEY_TYPE		GetKey(void)		{	return m_Iter->first;	}
		SCENE_SET*		GetSceneSet(void)	{	return m_Iter->second;	}
	private:
		typename RRenderChain<KEY_TYPE>::KEY_TO_SCENESET::iterator	m_Iter;
	};


	template<typename KEY_TYPE>
	RRenderChain<KEY_TYPE>::RRenderChain( void )
	{
	}

	template<typename KEY_TYPE>
	RRenderChain<KEY_TYPE>::~RRenderChain( void )
	{
	}

	template<typename KEY_TYPE>
	void RRenderChain<KEY_TYPE>::AddSceneNode( RSceneNode *pNode,KEY_TYPE& key )
	{
		_ASSERT(NULL != key.m_pMaterial);
		SCENE_SET* pSet = FindSceneNodeSet(key);
		if( NULL == pSet ) pSet = CreateSceneNodeSet(key);

		_ASSERT( pSet->end() == pSet->find(pNode) );
		pSet->insert(pNode);
		m_multimapSceneNodeToSet.insert(PAIR_ACTORNODE_TO_SET(pNode,pSet));
	}

	template<typename KEY_TYPE>
	void RRenderChain<KEY_TYPE>::RemoveSceneNode( RSceneNode *pNode )
	{
		PAIR_ITER_ACTORNODE_TO_SET pairIter;
		MULTIMAP_ACTORNODE_TO_SET::iterator iter;

		pairIter = m_multimapSceneNodeToSet.equal_range(pNode);
		if( pairIter.first != pairIter.second )
		{
			for( iter = pairIter.first ; iter != pairIter.second ; iter++ )
			{
				SCENE_SET* pSet = iter->second;
				_ASSERT( pSet->end() != pSet->find(pNode) );
				pSet->erase(pNode);
				if( pSet->empty() )	RemoveSceneNodeSet(pSet);
			}

			m_multimapSceneNodeToSet.erase(pNode);
		}
	}

	template<typename KEY_TYPE>
	SCENE_SET* RRenderChain<KEY_TYPE>::FindSceneNodeSet( KEY_TYPE& key )
	{
		std::map<KEY_TYPE,SCENE_SET*>::iterator iter;
		iter = m_mapRenderChain.find(key);
		if( iter == m_mapRenderChain.end() )	return NULL;
		return iter->second;
	}

	template<typename KEY_TYPE>
	SCENE_SET* RRenderChain<KEY_TYPE>::CreateSceneNodeSet( KEY_TYPE& key )
	{
		_ASSERT(m_mapRenderChain.end() == m_mapRenderChain.find(key));
		SCENE_SET* pSet = new SCENE_SET;
		m_mapRenderChain.insert(std::pair<KEY_TYPE,SCENE_SET*>(key,pSet));
		return pSet;
	}

	template<typename KEY_TYPE>
	void RRenderChain<KEY_TYPE>::RemoveSceneNodeSet( SCENE_SET* pSet )
	{
		_ASSERT(NULL != pSet);

		//지우기
		for(std::map<KEY_TYPE,SCENE_SET*>::iterator iter = m_mapRenderChain.begin() ; iter != m_mapRenderChain.end() ; iter++ )
		{
			if( iter->second == pSet )
			{
				m_mapRenderChain.erase(iter);
				delete pSet;
				pSet = NULL;
				break;
			}
		}

		_ASSERT(NULL == pSet);
	}

	template<typename KEY_TYPE>
	bool RRenderChain<KEY_TYPE>::IsEmpty( void )
	{
		if( false == m_mapRenderChain.empty() )
		{
			return false;
		}

		if( false == m_multimapSceneNodeToSet.empty() )
		{
			return false;
		}

		return true;
	}

	template<typename KEY_TYPE>
	bool RRenderChain<KEY_TYPE>::Equal(RRenderChain<KEY_TYPE> &rhs)
	{
		if( m_mapRenderChain.size() != rhs.m_mapRenderChain.size() )
		{
			return false;
		}

		if( m_multimapSceneNodeToSet.size() != rhs.m_multimapSceneNodeToSet.size() )
		{
			return false;
		}

		for(std::map<KEY_TYPE,SCENE_SET*>::iterator iter = m_mapRenderChain.begin() ; iter != m_mapRenderChain.end() ; iter++ )
		{
			KEY_TYPE key = iter->first;
			SCENE_SET* pSet = iter->second;
			SCENE_SET* pRightSet = rhs.FindSceneNodeSet(key);
			if( NULL == pRightSet )
			{
				return false;
			}
			if( pSet->size() != pRightSet->size() )
			{
				return false;
			}
			for(SCENE_SET::iterator iter = pSet->begin() ; iter != pSet->end() ; iter++ )
			{
				RSceneNode* pSceneNode = (*iter);
				if( pRightSet->end() == pRightSet->find(pSceneNode) )
				{
					return false;
				}
			}
		}

		for( MULTIMAP_SCENENODE_TO_SET::iterator iter = m_multimapSceneNodeToSet.begin(); iter != m_multimapSceneNodeToSet.end() ; iter++ )
		{
			RSceneNode* pSceneNode	= iter->first;
			SCENE_SET* pSet			= iter->second;
			if( rhs.m_multimapSceneNodeToSet.end() == rhs.m_multimapSceneNodeToSet.find(pSceneNode) )
			{
				return false;
			}

		} 

		return true;

	}
	

	template<typename KEY_TYPE>
	void RRenderChain<KEY_TYPE>::Clear( void )
	{
		for(std::map<KEY_TYPE,SCENE_SET*>::iterator iter = m_mapRenderChain.begin() ; iter != m_mapRenderChain.end() ; iter++ )
		{
			SCENE_SET* pSet = iter->second;
			delete pSet;
		}
		m_mapRenderChain.clear();
		m_multimapSceneNodeToSet.clear();
	}
}