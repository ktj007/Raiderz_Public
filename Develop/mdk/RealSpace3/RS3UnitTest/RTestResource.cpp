#include "stdafx.h"
#include "RTypes.h"
#include "RResourceID.h"
#include "RResource.h"

using namespace rs3;

TEST( TestResource, GetResourceIDString )
{
	EXPECT_EQ( RRESOURCETYPE::RR_UNKNOWN, RRESOURCETYPE::RR_MAX );

	// GetResourceIDString 개수 맞는지 검사
	EXPECT_STREQ( RRESOURCETYPE::GetResoruceIDString(RRESOURCETYPE::RR_MAX), "_managed_max" );
	EXPECT_STREQ( RRESOURCETYPE::GetResoruceIDString(RRESOURCETYPE::RR_UNMANAGED_MAX), "_unmanaged_max" );
}

TEST( TestResource, ResourceLoader )
{
	class TestLoadingController : public RResource::LoadingController
	{
	public:
		int m_nCount;

		TestLoadingController() : m_nCount(0) {}

		virtual void	OnLoadingComplete(RResource*)	{  m_nCount++; }

	};

	TestLoadingController* testLoadingController = new TestLoadingController;

	class TestResource : public RResource
	{
	public:
		size_t GetLoadingControllerCount() { return m_loadingControllers.size(); }
	};

	TestResource* aResource = new TestResource;

	aResource->BuildAll( false, testLoadingController);

	// 완료 통지가 오는지 확인
	EXPECT_EQ( 1, testLoadingController->m_nCount );
	EXPECT_EQ( 0, aResource->GetLoadingControllerCount() );

	// 두번 더하면 통지가 두번 오는지 확인
	aResource->AddLoadingController( testLoadingController ) ;
	aResource->AddLoadingController( testLoadingController ) ;
	aResource->BuildAll( false );
	EXPECT_EQ( 3, testLoadingController->m_nCount );

	// 중복해서 controller가 더해지는지 확인
	aResource->AddLoadingController( testLoadingController ) ;
	aResource->AddLoadingController( testLoadingController ) ;
	EXPECT_EQ( 2, aResource->GetLoadingControllerCount() );

	// LoadingController 가 삭제되어도 이상없는지 확인
	delete testLoadingController;
	EXPECT_EQ( 0, aResource->GetLoadingControllerCount() );

	testLoadingController = new TestLoadingController;

	aResource->AddLoadingController( testLoadingController ) ;
	aResource->AddLoadingController( testLoadingController ) ;
	
	// 통지 받을 리소스 수가 1개인지 확인
	EXPECT_EQ( 1, testLoadingController->GetResourceNotifyCount() );
	// 통지 받을 회수가 2개 인지 확인
	EXPECT_EQ( 2, testLoadingController->GetNotifyCount(aResource) );

	// Resource 가 삭제되어도 이상없는지 확인
	delete aResource;

	EXPECT_EQ( 0, testLoadingController->GetResourceNotifyCount() );
	delete testLoadingController;
}