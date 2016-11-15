#pragma once

#include <list>
#include <string>
#include <map>
#include "MThread2.h"
#include "RResourceID.h"

#define DECLARE_RES_MGR_ID(x)		enum { RES_MGR_ID = x };
#define DECLARE_USING_RES_MGR(x)	typedef x T_RES_MGR;

using namespace std;

namespace rs3 {

class RResourceManager;
class RSceneManager;
class RDevice;
class RDeviceD3D;
class RHandleResourceManager;
class RResourceLoader;
class RResourceLoadingGroup;
class RBackgroundWorker;
class RWorldSceneNode;

////////////////////////////////////////////////////////////////////////////////////////////////////
// 리소스 상태
//
enum RRSTATE
{
	RRS_EMPTY	= 0,		///< 처음생성되어 비어있는 상태
	RRS_FILLED,				///< IO로부터 리소스 로드정보를 읽어들인 상태
	RRS_LOADED,				///< 디바이스가 쓸 수 있도록 로딩된 상태
	RRS_LOST,				///< 로드는 되었으나 디바이스에서 쓸 수 없는 상태

	RRS_WRONG				///< 잘못된 리소스
};

class RResourceProvider;
class RAbstractResourceCreationInfo;
class RLinker;
class RESOURCELINKTYPE_RESOURCECREATIONINFO;
class RResourceWorkUnit;
////////////////////////////////////////////////////////////////////////////////////////////////////
//  리소스 클래스
//  레퍼런스 카운트 및 아이디와 리소스 매니저를 통한 리소스 생성
//
class RS_API RResource
{
	friend RResourceManager;
	friend RDevice;
	friend RResourceLoader;
	friend RBackgroundWorker;
	friend RResourceProvider;
	friend RDeviceD3D;
	friend RResourceWorkUnit;

public:
	class RS_API LoadingController
	{
		friend RResource;

	public:
		virtual ~LoadingController();

		virtual int		CalcPriority()					{ return 0; }	// RBGWK_NORMAL
		virtual void	OnLoadingComplete(RResource*)	{}

		size_t	GetResourceNotifyCount() { return m_mapResourceNotify.size(); }
		int		GetNotifyCount(RResource* pResource )
		{
			map<RResource*,int>::iterator itr = m_mapResourceNotify.find(pResource);
			if(itr!=m_mapResourceNotify.end())
				return itr->second;
			return 0;
		}
	private:
		map<RResource*,int>		m_mapResourceNotify;	///< 내가 완료통지를 받고자 하는 리소스들, second : 통지받을 회수
		
		void AddNotify( RResource* pResource )
		{
			map<RResource*,int>::iterator itr = m_mapResourceNotify.find(pResource);
			if(itr!=m_mapResourceNotify.end())
			{
				(itr->second)++;
				return;
			}

			m_mapResourceNotify.insert( map<RResource*,int>::value_type( pResource, 1) );
		}

		void RemoveNotify( RResource* pResource	)
		{
			map<RResource*,int>::iterator itr = m_mapResourceNotify.find(pResource);
			if(itr!=m_mapResourceNotify.end())
			{
				if(itr->second == 1 )
					m_mapResourceNotify.erase( itr );
				else
					itr->second--;
				return;
			}
			_ASSERT( FALSE);
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성자 / 소멸자
	//
	RResource(void);
	RResource( const char* _pSzResourceName, int nResID );
	virtual ~RResource(void);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 리소스 아이디 / 이름 / 레퍼런스 카운트
	//
	int							GetResID();
	const std::string&			GetName() const;
	virtual const char*			GetDebugName() { return GetName().c_str(); }

	int							AddRef();
	int							GetRefCount() const;
	void						SetRefCount(int RefCount) { m_nRefCount = RefCount; }


	//////////////////////////////////////////////////////////////////////////
	// State
	RRSTATE						GetState() const;

	/// 로딩이 끝났는가 여부, state 가 loaded 이거나 wrong 인경우이다
	bool						IsLoadingCompleted() const;

	/// 로딩이 성공했는가?
	bool						IsLoadingSucceeded() const;

	// TODO: 이 함수가 필요한가 ?
	void						ForceSetState( RRSTATE _eState )	{ m_state = _eState; }

	void						SetResourceLoadingGroup( RResourceLoadingGroup* pResLoadingGroup ) { m_pResLoadingGroup = pResLoadingGroup; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 전체 생성 / 소멸
	//
	virtual bool				BuildAll( bool bBackground = false, LoadingController* pController = NULL );
	virtual int					Release();		// Safe Release. RefCount가 0이되면 할 행동들을 설정해주어야한다.

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 자원
	//
	virtual int					GetUsingSystemMemory()	{ return 0; }
	virtual int					GetUsingVideoMemory()	{ return 0; }

	// 캐쉬
	enum CACHE_PRIORITY
	{
		RRCP_NOT_USING	= -1,	///< 캐싱 활성화 상태라도 캐싱되지 않는 리소스.
		RRCP_ALWAYS		= 0,	///< update때 지워지지 않고 항상 Caching 되어 있음. ClearCache 때 같이 지워줌

		RRCP_HIGH,
		RRCP_MIDDLE,
		RRCP_LOW,

		RRCP_MAX
	};

	enum CACHE_TYPE
	{
		SYSTEM_MEMORY = 0,
		VIDEO_MEMORY
	};

	void						SetCachePriority( CACHE_PRIORITY _eCachePriority );
	CACHE_PRIORITY				GetCachePriority();

	//////////////////////////////////////////////////////////////////////////
	// 백그라운드 로딩 관련
	int							GetLoadingPriority();				///< 백그라운드 로딩의 우선순위
	virtual void				SetLoadingPriority(int nPriority);
	RResourceLoader*			GetLoader()			{ return m_pLoader; }

	bool						IsBackgroundCreated();	///< 백그라운드 로딩으로 로딩이 시도되었는지 여부
	bool						IsBuildLater();			///< 빌드를 ResourceLoadingGroup으로 함

	void						AddLoadingController(LoadingController* pListener);
	void						RemoveLoadingController(LoadingController* pListener);

public:
	virtual bool				Invalidate()	{ return true; }	// device에 밀접한 리소스일경우 장치 reset시에
	virtual bool				Restore()		{ return true; }

protected:

	enum LOAD_RESULT {
		WRONG = 0,
		LOADED,
		WAITING,
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 상태별 리소스 처리 / 생성
	//
	virtual bool				Fill()			{ return true; }	// 리소스의 내용을 채운다 // 실제로 데이터가 로딩되는 부분이다. XML 파일에서 파일 이름이 지정 되는 곳과 떨어져 있어 난해했네 - COMMENT OZ
	virtual bool				Erase()			{ return true; }	// 리소스의 내용을 비운다 : 현재 RMesh만 구현
	virtual LOAD_RESULT			Load()			{ return LOADED; }	// 디바이스로 로딩

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 이름 / 아이디 / 레퍼런스 카운트
	//
	int							m_nRefCount;
	int							m_nResID;
	CACHE_PRIORITY				m_cachePriority;
	int							m_nLoadingPriority;
	std::string					m_strName;

	RResourceLoader*			m_pLoader;

	typedef list<LoadingController*>	LOADINGCONTROLLERLIST;
	list<LoadingController*>			m_loadingControllers;

	RResourceLoadingGroup*		m_pResLoadingGroup;

//	void						RegisterNotify();			///< 완료 통지
	
	bool						SendNotify1();		///< 하나의 리스너에게 통지, 통지할 리스너가 없으면 false를 리턴한다
	void						SendNotifyAll();	///< 모든 리스너들에게 통지
	void						SendNotifyResLoadingGroup(); ///< 리소스 로딩 그룹에 완료 통지


private:
	RRSTATE						m_state;
};


//----------------------------------------------------------------------------------------------------
inline int RResource::AddRef()			{ return ++m_nRefCount; }
//----------------------------------------------------------------------------------------------------
inline int RResource::GetRefCount() const { return m_nRefCount; }
//----------------------------------------------------------------------------------------------------
inline const string& RResource::GetName() const { return m_strName; }
//----------------------------------------------------------------------------------------------------
inline int RResource::GetResID()		{ return m_nResID; }
//----------------------------------------------------------------------------------------------------
inline RRSTATE RResource::GetState() const	{ return m_state; }
//----------------------------------------------------------------------------------------------------
inline bool RResource::IsBuildLater() { return (m_pResLoadingGroup != NULL); }
//----------------------------------------------------------------------------------------------------
inline int RResource::GetLoadingPriority()	{ return m_nLoadingPriority; }
// //----------------------------------------------------------------------------------------------------
inline void RResource::SetLoadingPriority(int nPriority)	{ m_nLoadingPriority = nPriority; }
//----------------------------------------------------------------------------------------------------
inline void RResource::SetCachePriority( CACHE_PRIORITY _eCachePriority )
{
	if (GetRefCount() <= 0 )
	{
		_ASSERT( !"캐싱중이거나 레퍼런스 카운터가 올바르지 않습니다." );
		return;
	}
	_ASSERT( _eCachePriority < RRCP_MAX );
	m_cachePriority = _eCachePriority;
}
//----------------------------------------------------------------------------------------------------
inline RResource::CACHE_PRIORITY RResource::GetCachePriority(){ return m_cachePriority; }
//----------------------------------------------------------------------------------------------------
inline bool RResource::IsLoadingCompleted() const { return (m_state==RRS_LOADED || m_state==RRS_WRONG); }
//----------------------------------------------------------------------------------------------------
inline bool RResource::IsLoadingSucceeded() const { return (m_state==RRS_LOADED); }
//----------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////
// 핸들기반 리소스
//
class RS_API RHandleResource : public RResource
{
	friend RHandleResourceManager;

protected:
	RHANDLE			m_handle;

public:
	RHandleResource(void);
	virtual ~RHandleResource(void);

	RHANDLE		GetHandle();
};
//----------------------------------------------------------------------------------------------------
inline RHANDLE RHandleResource::GetHandle()	{ return m_handle; }

////////////////////////////////////////////////////////////////////////////////////////////////////
// 일반적인 용도로 사용가능한 리소스
// 이 클래스를 상속 받으면 RGeneralResourceManager를 사용하여 관리됩니다.
//
class RGeneralResourceManager;
class RS_API RGeneralResource : public RResource
{
public:
	DECLARE_USING_RES_MGR(RGeneralResourceManager);
	RGeneralResource(const char* _pSzResourceName, RSceneManager* pSceneManager) : RResource(_pSzResourceName,RRESOURCETYPE::RR_GENERAL)
	{
	}
};


class RS_API RAbstractResourceCreationInfo
{
public:
	RAbstractResourceCreationInfo(void) : m_Name(""){}
	RAbstractResourceCreationInfo(std::string ResourceName) : m_Name(ResourceName){}

public:
	virtual int GetResourceType(void) = 0;
	RAbstractResourceCreationInfo* Clone(void);

	bool	HasResourceName(void)			{	return m_Name=="" ? false : true;	}
	bool	IsMakedResourceSharable(void)	{	return HasResourceName();	}

protected:
	virtual	RAbstractResourceCreationInfo*	AllocateCreationInfo(void) = 0;
	virtual void					OnClone(RAbstractResourceCreationInfo* pIdentifierToFill) = 0;

public:
	std::string m_Name;
};



}
