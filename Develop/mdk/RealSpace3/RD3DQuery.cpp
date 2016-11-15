#include "StdAfx.h"
#include "RD3DQuery.h"

namespace rs3
{

	RD3DOcclusionQuery::RD3DOcclusionQuery( void )
		:m_pD3DQuery(NULL)
	{
	}

	RD3DOcclusionQuery::~RD3DOcclusionQuery( void )
	{
		Release();
	}

	bool RD3DOcclusionQuery::Create(void* pDevice)
	{
		LPDIRECT3DDEVICE9 pD3DDevice = (LPDIRECT3DDEVICE9)pDevice;
		HRESULT hr;
		LPDIRECT3DQUERY9 pd3dQuery = NULL;
		if( FAILED( hr = pD3DDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pd3dQuery ) ) )
		{
			SAFE_RELEASE(pd3dQuery);
			mlog("can't create d3d query\n");
			return false;
		}

		m_pD3DQuery = pd3dQuery;
		return true;
	}

	void RD3DOcclusionQuery::Release( void )
	{
		if( NULL != m_pD3DQuery )
		{
			m_pD3DQuery->Release();
			m_pD3DQuery = NULL;
		}
	}

	void RD3DOcclusionQuery::BeginIssue( void )
	{
		_ASSERT(NULL != m_pD3DQuery);
		m_pD3DQuery->Issue(D3DISSUE_BEGIN);
	}

	void RD3DOcclusionQuery::EndIssue( void )
	{
		_ASSERT(NULL != m_pD3DQuery);
		m_pD3DQuery->Issue(D3DISSUE_END);
	}

	bool RD3DOcclusionQuery::Data( int* pData )
	{
		_ASSERT(NULL != m_pD3DQuery);

		HRESULT hr = m_pD3DQuery->GetData( (void*)pData,sizeof(int), D3DGETDATA_FLUSH );
		if( hr == S_FALSE ) return false;
		if( hr == S_OK )	return true;

		// http://msdn.microsoft.com/en-us/library/bb205873(v=VS.85).aspx
		// Query에 대한 폴링으로 응답받는 소스코드로 인해 이 경우에도 true로 반환해야 정상작동됨
		if( hr == D3DERR_DEVICELOST ) return true;	

		return true;
	}


	RD3DEventQuery::RD3DEventQuery( void )
		:m_pD3DQuery(NULL)
	{

	}

	RD3DEventQuery::~RD3DEventQuery( void )
	{
		Release();
	}

	void RD3DEventQuery::EndIssue( void )
	{
		_ASSERT(NULL != m_pD3DQuery);
		m_pD3DQuery->Issue(D3DISSUE_END);
	}

	void RD3DEventQuery::EmptyCommandBuffer( void )
	{
		while(S_FALSE == m_pD3DQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ));
	}

	bool RD3DEventQuery::Create( void* pDevice )
	{
		LPDIRECT3DDEVICE9 pD3DDevice = (LPDIRECT3DDEVICE9)pDevice;
		HRESULT hr;
		LPDIRECT3DQUERY9 pd3dQuery = NULL;
		if( FAILED( hr = pD3DDevice->CreateQuery( D3DQUERYTYPE_EVENT, &pd3dQuery ) ) )
		{
			SAFE_RELEASE(pd3dQuery);
			mlog("can't create d3d query\n");
			return false;
		}

		m_pD3DQuery = pd3dQuery;
		return true;
	}

	void RD3DEventQuery::Release( void )
	{
		if( NULL != m_pD3DQuery )
		{
			m_pD3DQuery->Release();
			m_pD3DQuery = NULL;
		}
	}

	RD3DQueryManager::RD3DQueryManager( void )
	{
	}

	RD3DQueryManager::~RD3DQueryManager( void )
	{
	}

	LPDIRECT3DDEVICE9 RD3DQueryManager::m_pD3DDevice = NULL;

	RQuery* RD3DQueryManager::CreateQuery(QUERY_TYPE query_type)
	{
		RQuery *pRQuery = NULL;

		switch(query_type)
		{
			case	QUERY_OCCLUSION:
			{
				pRQuery = new RD3DOcclusionQuery;
			}break;
			case	QUERY_EVENT:
			{
				pRQuery = new RD3DEventQuery;
			}break;
		}

		
		if( false == pRQuery->Create(m_pD3DDevice) )
		{
			delete pRQuery;
			return NULL;
		}

		m_setQuery.insert(pRQuery);

		return pRQuery;
	}

	void RD3DQueryManager::ReleaseQuery( RQuery* pQuery )
	{
		QUERY_SET::iterator iter;
		iter = m_setQuery.find(pQuery);
		if( iter == m_setQuery.end() )	return;

		m_setQuery.erase(pQuery);
		pQuery->Release();
		delete pQuery;
	}

	void RD3DQueryManager::Release( void )
	{
		QUERY_SET::iterator iter;
		for(iter = m_setQuery.begin(); iter != m_setQuery.end() ; ++iter )
		{	
			RQuery* pQuery = (*iter);
			pQuery->Release();
			delete pQuery;
		}

		m_setQuery.clear();
	}

	void RD3DQueryManager::RestoreDevice( void )
	{
		QUERY_SET::iterator iter;
		for(iter = m_setQuery.begin(); iter != m_setQuery.end() ; ++iter )
		{	
			RQuery* pQuery = (*iter);
			pQuery->Create(m_pD3DDevice);
		}
	}	

	void RD3DQueryManager::LostDevice( void )
	{
		QUERY_SET::iterator iter;
		for(iter = m_setQuery.begin(); iter != m_setQuery.end() ; ++iter )
		{	
			RQuery* pQuery = (*iter);
			pQuery->Release();
		}
	}

}