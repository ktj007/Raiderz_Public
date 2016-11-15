#include "stdafx.h"
#include "REffectManager.h"
#include "REffectSceneNode.h"

namespace rs3 {

REffectSharedBuffer::REffectSharedBuffer( int nVertexCount, int nIndexCount ) :
  m_hVertexBuffer(R_NONE)
, m_nVertexCount(nVertexCount)
, m_nVertexWriteCount(0)
, m_nVertexBufferSize(0)
, m_nNextVertexData(0)
, m_nCurrentVertexData(0)

, m_hIndexBuffer(R_NONE)
, m_nIndexCount(nIndexCount)
, m_pVertexArray(NULL)
, m_pIndexArray(NULL)
, m_nIndexWriteCount(0)
{
	ReCreateVertexBuffer();
	ReCreateIndexBuffer();
}

REffectSharedBuffer::~REffectSharedBuffer()
{
	DeleteVertexBuffer();
	DeleteIndexBuffer();
}

void REffectSharedBuffer::ReCreateVertexBuffer()
{
	DeleteVertexBuffer();

	m_pVertexArray = new PVERTEXFORMAT[m_nVertexCount];
	_ASSERT( m_pVertexArray );

	// create VB
	m_nVertexBufferSize	= m_nVertexCount * sizeof(PVERTEXFORMAT);
	// D3DUSAGE_DYNAMIC in DX DOC :		As a result, D3DLOCK_DISCARD and D3DLOCK_NOOVERWRITE are valid only on vertex buffers created with D3DUSAGE_DYNAMIC. 
	// D3DUSAGE_WRITEONLY in DX DOC :	Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty. D3DUSAGE_WRITEONLY only affects the performance of D3DPOOL_DEFAULT buffers.
	m_hVertexBuffer = REngine::GetDevice().CreateVertexBuffer( m_nVertexBufferSize, sizeof(PVERTEXFORMAT), NULL, (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY) );
	_ASSERT( m_hVertexBuffer );
}

void REffectSharedBuffer::ReCreateIndexBuffer()
{
	DeleteIndexBuffer();

	m_pIndexArray = new WORD[m_nIndexCount];

	// create IB
	m_hIndexBuffer = REngine::GetDevice().CreateIndexBuffer( m_nIndexCount, RFMT_INDEX16, NULL, (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY));
	_ASSERT( m_hIndexBuffer );
}

void REffectSharedBuffer::DeleteVertexBuffer()
{
	if( m_hVertexBuffer != R_NONE )
	{
		REngine::GetDevice().DeleteVertexBuffer( m_hVertexBuffer );
		m_hVertexBuffer = R_NONE;
	}
	SAFE_DELETE_ARRAY( m_pVertexArray );
}

void REffectSharedBuffer::DeleteIndexBuffer()
{
	if( m_hIndexBuffer != R_NONE )
	{
		REngine::GetDevice().DeleteIndexBuffer( m_hIndexBuffer );
		m_hIndexBuffer = R_NONE;
	}
	SAFE_DELETE_ARRAY( m_pIndexArray );
}

//nCount : 버텍스 갯수
bool REffectSharedBuffer::WriteVertex( unsigned int nCount )
{
	if( nCount == 0) return true;

	int nSizeOfData = nCount * sizeof(PVERTEXFORMAT);	//데이타 크기
	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;

	if( m_nNextVertexData + nSizeOfData >= m_nVertexBufferSize  )
	{
		// No space remains. Start over from the beginning 
		//   of the vertex buffer.
		dwLockFlags = D3DLOCK_DISCARD;
		m_nNextVertexData = 0;
	}

	void *pVertex = REngine::GetDevice().LockVertexBuffer( m_hVertexBuffer, dwLockFlags, m_nNextVertexData / sizeof(PVERTEXFORMAT), nCount );
	if( pVertex )
	{
		memcpy( pVertex, m_pVertexArray, nCount * sizeof(PVERTEXFORMAT) );
		REngine::GetDevice().UnlockVertexBuffer(m_hVertexBuffer);
		m_nVertexWriteCount = nCount;

		m_nCurrentVertexData = m_nNextVertexData;	// 락걸어서 쓰여진 버퍼의 주소. 렌더링시에 사용됨
		m_nNextVertexData+=nSizeOfData;				// 미사용된 버퍼의 시작주소
		return true;
	}

	m_nVertexWriteCount = 0;
	return false;
}

bool REffectSharedBuffer::WriteIndex( unsigned int nCount )
{
	if( nCount == 0) return true;

	void *pIndex = REngine::GetDevice().LockIndexBuffer( m_hIndexBuffer, D3DLOCK_DISCARD, 0, nCount );
	if( pIndex )
	{
		memcpy( pIndex, m_pIndexArray, nCount * sizeof(WORD) );
		REngine::GetDevice().UnlockIndexBuffer( m_hIndexBuffer );
		m_nIndexWriteCount = nCount;
		return true;
	}

	m_nIndexWriteCount = 0;
	return false;
}

PVERTEXFORMAT* REffectSharedBuffer::GetVertexArray( unsigned int nCount )
{
	if( m_nVertexCount < nCount )
	{
		while( m_nVertexCount < nCount )
			m_nVertexCount = m_nVertexCount * 2;

		ReCreateVertexBuffer();
	}

	return m_pVertexArray;
}

WORD* REffectSharedBuffer::GetIndexArray( unsigned int nCount )
{
	if( m_nIndexCount < nCount )
	{
		while( m_nIndexCount < nCount )
			m_nIndexCount = m_nIndexCount * 2;

		ReCreateIndexBuffer();
	}

	return m_pIndexArray;
}

int REffectSharedBuffer::DrawPrimitiveTriangleList()
{
	RPFC_THISFUNC;

	int primitiveCount = m_nVertexWriteCount / 3;
	if( primitiveCount <= 0 ) return 0;

	REngine::GetDevice().SetVertexBuffer( m_hVertexBuffer, 0, 0 );

	if( REngine::GetDevice().DrawPrimitive( RPT_TRIANGLELIST, m_nCurrentVertexData / sizeof(PVERTEXFORMAT) , primitiveCount ) )
		return primitiveCount;

	return 0;
}

int REffectSharedBuffer::DrawIndexedPrimitiveTriangleStrip()
{
	RPFC_THISFUNC;

	int primitiveCount = m_nIndexWriteCount - 2;
	if( primitiveCount <= 0 ) return 0;

	REngine::GetDevice().SetVertexBuffer( m_hVertexBuffer, 0, 0 );
	REngine::GetDevice().SetIndexBuffer( m_hIndexBuffer );

	if( REngine::GetDevice().DrawIndexedPrimitive( RPT_TRIANGLESTRIP, m_nCurrentVertexData / sizeof(PVERTEXFORMAT), 0, m_nIndexWriteCount, 0, primitiveCount ) )
		return primitiveCount;

	return 0;
}

RParticlePool* REffectManager::m_pParticlePool = NULL;
REffectSharedBuffer* REffectManager::m_pSharedBuffer = NULL;
float REffectManager::m_fCreateParticleRange = 10000.f;

REffectManager::REffectManager()
{
	m_pParticlePool = new RParticlePool( 2048 );
	m_pSharedBuffer = new REffectSharedBuffer( 4096, 1024 );
}

REffectManager::~REffectManager()
{
	Destroy();
	_ASSERT(m_mapEffectSources.empty());
}

void REffectManager::Destroy()
{
	ClearEffectSource();


	SAFE_DELETE(m_pParticlePool);
	SAFE_DELETE(m_pSharedBuffer);
}

REffectSceneNode* REffectManager::CreateEffectSource(const char* szFileName, bool bBackgroundCreation, bool bAddInfiniteEffectSource)
{
	if( szFileName == NULL )
		return NULL;

	if( !strlen( szFileName ) )
		return NULL;

	// 없으면 만든다
	string strFullPathName;
	strFullPathName += szFileName;
	strFullPathName += ".effect.scene.xml";

	bool bBeing = false;
	if ( MIsExistFile( strFullPathName.c_str()) == true)
		bBeing = true;

	bool bLoaded = false;
	RWorldSceneNode	worldSceneNode;
	if( bBeing)
		bLoaded = REngine::GetSceneManager().LoadTempEfffectWorldSceneNodeFromRootXMLFile( &worldSceneNode, strFullPathName.c_str(), bBackgroundCreation );
	if( bLoaded == false )
		return NULL;

	REffectSceneNode* pEffectSceneNode = NULL;
	const RSceneNodeList& topLevelNodes = worldSceneNode.GetChildren();
	for( RSceneNodeList::const_iterator itr = topLevelNodes.begin(); itr!=topLevelNodes.end(); ++itr)
	{
		RSceneNode* pSceneNode = *itr;
		if( MIsDerivedFromClass(REffectSceneNode, pSceneNode) )
		{
			// 두개 이상있으면 에러
			_ASSERT( NULL == pEffectSceneNode );
			pEffectSceneNode = MDynamicCast(REffectSceneNode, pSceneNode);
		}
	}
	if( NULL == pEffectSceneNode )
		return NULL;

	pEffectSceneNode->m_strFileName = strFullPathName;
	pEffectSceneNode->RemoveFromParent();
	pEffectSceneNode->UpdateRenderableNodesList();

	if (bAddInfiniteEffectSource)
		m_mapEffectSources.insert( EFFECT_SRC_MAP::value_type(szFileName, REF_EFFECT_SRC(-1, pEffectSceneNode)) );

	return pEffectSceneNode;
}

void REffectManager::ClearEffectSource()
{
	for(EFFECT_SRC_MAP::iterator itr = m_mapEffectSources.begin(); itr!= m_mapEffectSources.end(); ++itr)
	{
		_ASSERT(itr->second.first == -1);
		delete itr->second.second;
	}

	m_mapEffectSources.clear();
}

REffectSceneNode* REffectManager::CreateEffect(const char* szFileName, bool bBackgroundCreation)
{
	REffectSceneNode* pExistEffectSceneNode = GetEffect(szFileName);
	if( !pExistEffectSceneNode )
		pExistEffectSceneNode = CreateEffectSource(szFileName, bBackgroundCreation);

	if(!pExistEffectSceneNode)
		return NULL;

	RSceneNode* pClonedNode = CloneEffect( pExistEffectSceneNode );
	REffectSceneNode* pNewEffectSceneNode = MDynamicCast( REffectSceneNode, pClonedNode );
	_ASSERT(pNewEffectSceneNode);

	pNewEffectSceneNode->UpdateRenderableNodesList();

	return pNewEffectSceneNode;
}

RSceneNode* REffectManager::CloneEffect(RSceneNode* pSource)
{
	RSceneNode* pClonedNode = pSource->Clone();
	if( pClonedNode == NULL )
		return NULL;

	CloneChildren( pClonedNode, pSource );
	CloneControllers( pClonedNode, pSource );

	return pClonedNode;
}

REffectSceneNode* REffectManager::GetEffect(const char* szFileName)
{
	EFFECT_SRC_MAP::iterator itr = m_mapEffectSources.find( szFileName );
	if( itr == m_mapEffectSources.end() )
		return NULL;

	return itr->second.second;
}

void REffectManager::CloneControllers(RSceneNode* pDestNode, RSceneNode* pSource)
{
	for( int i = 0; i < RSNCTRL_MAX; ++i )
	{
		RSCENENODECONTROLLERLIST* pControllers = pSource->GetControllers( RSNCTRL_TYPE(i));

		for( RSCENENODECONTROLLERLIST::iterator it = pControllers->begin(); it != pControllers->end(); ++it )
		{
			RSceneNodeController* pController = *it;
			if( pController->QueryAttribute( RCA_STICKY ) ) continue;

			RSceneNodeController* pNewController = pController->Clone();
			pDestNode->AddController( pNewController );
		}
	}
}

void REffectManager::CloneChildren(RSceneNode* pDestNode, RSceneNode* pSource)
{
	for ( RSceneNodeList::const_iterator i = pSource->GetChildren().begin(); i != pSource->GetChildren().end(); ++i) 
	{
		RSceneNode* pChildNode = *i;
		if(pChildNode->QueryAttribute( RSNA_DO_NOT_SAVE )) continue;

		RSceneNode* pNewChildNode = pChildNode->Clone();
		_ASSERT(pNewChildNode);
		if(pNewChildNode)
		{
			pDestNode->AddChild( pNewChildNode );
			pDestNode->AddToSceneNodeGroup( pNewChildNode );

			CloneChildren( pNewChildNode, pChildNode );
			CloneControllers( pNewChildNode, pChildNode );
		}
	}
}

int REffectManager::AddRefEffectSource( const std::string& _strName, REffectSceneNode* _pEffectSceneNode )
{
	EFFECT_SRC_MAP::iterator itr = m_mapEffectSources.find(_strName);
	if (itr == m_mapEffectSources.end())
	{
		m_mapEffectSources.insert( EFFECT_SRC_MAP::value_type(_strName, REF_EFFECT_SRC(1, _pEffectSceneNode) ) );
		return 1;
	}

	if (itr->second.first == -1)
		return -1;

	int& nRefCnt = itr->second.first;
	++nRefCnt;
	return nRefCnt;
}

void REffectManager::RemoveRefEffectSource( const std::string& _strName, REffectSceneNode* _pNotDeleteLastSrc_If_EqualToThis )
{
	EFFECT_SRC_MAP::iterator itr = m_mapEffectSources.find(_strName);
	if (itr != m_mapEffectSources.end() && itr->second.first != -1)
	{
		int& nRefCnt = itr->second.first;
		--nRefCnt;

		_ASSERT(nRefCnt >= 0);
		if(nRefCnt == 0)
		{
			if (_pNotDeleteLastSrc_If_EqualToThis != itr->second.second)
				delete itr->second.second;

			m_mapEffectSources.erase(itr);
		}
	}
}

}