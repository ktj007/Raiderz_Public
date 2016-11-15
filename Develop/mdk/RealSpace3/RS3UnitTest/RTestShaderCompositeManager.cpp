#include "stdafx.h"
#include "RMaterial.h" // TODO: RMaterial.h를 인클루드 하지 않아도 컴파일 에러 나지 않게 수정해야 함.
#include "RShaderCompositeManager.h"
#include "RShaderComposite.h"

using namespace rs3;

const int TEST_ID0 = 0;
const int TEST_ID1 = RSC_COUNT - 1;

TEST( TestShaderCompositeManager, GetComponentsNameList )
{
	vector<string> rscComponentNames;

	RSCID rscNullID;
	RShaderCompositeManager::GetComponentsNameList( rscNullID, rscComponentNames );
	EXPECT_EQ( 0, rscComponentNames.size() );

	RSCID rscTestID = RSCID( TEST_ID0 ) | RSCID( TEST_ID1 );
	RShaderCompositeManager::GetComponentsNameList( rscTestID, rscComponentNames );
	EXPECT_EQ( 2, rscComponentNames.size() );
	EXPECT_STREQ( componentNames[TEST_ID0].name, rscComponentNames[0].c_str() );
	EXPECT_STREQ( componentNames[TEST_ID1].name, rscComponentNames[1].c_str() );
};

TEST( TestShaderCompositeManager, GetShaderComponentIDByName )
{
	RSCID rscTestID;
	rscTestID = RShaderCompositeManager::GetShaderComponentIDByName( componentNames[TEST_ID0].name );
	EXPECT_TRUE( componentNames[TEST_ID0].flag == rscTestID );

	rscTestID = RShaderCompositeManager::GetShaderComponentIDByName( componentNames[TEST_ID1].name );
	EXPECT_TRUE( componentNames[TEST_ID1].flag == rscTestID );
};

TEST( TestShaderCompositeManager, ShaderComposite )
{
	size_t count = sizeof(componentNames)/sizeof(ShaderComponentPair);
	EXPECT_EQ( count, RSC_COUNT );

	class MockShaderComponent : public RShaderComponent
	{
	public:
		MockShaderComponent( const string& strString ) { Create(strString); }
	};

	class MockShaderComposite : public RShaderComposite
	{
	public:
		MockShaderComposite( RSCID rscComponents ) 
			: RShaderComposite(rscComponents) { }
		
		using RShaderComposite::Composite;
	};

	RSCID MCS_1( 0, 1 );
	RSCID MCS_2( 0, 1<<1 );
	RSCID MCS_3( 0, 1<<2 );

	class MockCompositeManager : public RShaderCompositeManager
	{
	public:
		virtual bool Create()
		{
			m_shaderComponents.resize(3);
			m_shaderComponents[0] = new MockShaderComponent("interface()\n{\n$define = test1\n}");	// test1을 디파인한 조각
			m_shaderComponents[1] = new MockShaderComponent("interface()\n{\n$name = test2\n}");
			m_shaderComponents[2] = new MockShaderComponent("interface()\n{\n$dependency = test1\n}");
			return true;
		}
		virtual	RShaderComposite*	NewShaderComposite(RSCID rscComponents)
		{
			return new MockShaderComposite(rscComponents);
		}

	};

	MockCompositeManager aManager;
	aManager.Create();

	// 두번째 콤포넌트 이름이 잘 들어가 있는지 확인
	EXPECT_STREQ( aManager.GetShaderComponent( 1 )->GetName().c_str(), "test2" );

	MockShaderComposite composite( MCS_1 | MCS_2 | MCS_3 );
	bool bSuccess = composite.Composite( &aManager );
	EXPECT_TRUE( bSuccess );

	// 1 이 없으면 에러
	MockShaderComposite composite2( MCS_2 | MCS_3 );
	bSuccess = composite2.Composite( &aManager );
	EXPECT_FALSE( bSuccess );

	// 4번 콤포넌트는 없기 때문에 요청한것과 만든것이 달라도 에러
	MockShaderComposite invalidComposite( MCS_1 | MCS_2 | MCS_3 | RSCID( 0, 1<<3 ) );
	bSuccess = invalidComposite.Composite( &aManager );
	EXPECT_FALSE( bSuccess );

	// TODO: 에러코드를 정의하여 엄밀하게 확인하면 더 좋겠다.

	aManager.Destroy();
}
