#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "REffectSceneNode.h"

using namespace rs3;

class TestEffectSceneNode : public NullDeviceFixture
{
};

class RTestEffectBase : public REffectBase
{
public:
	RTestEffectBase() : m_bLooping( false ) {}
	~RTestEffectBase() {}

	virtual void Start() override {};
	virtual void Stop() override {};
	virtual void Pause() override {};
	virtual bool IsAlive() override { return false; };

	virtual void SetLooping(bool loop) override { m_bLooping = loop; }
	virtual bool IsLooping() override { return m_bLooping; }

	virtual int GetCurrentCount() override { return 0; };

protected:
	bool m_bLooping;
};

TEST_F( TestEffectSceneNode, IsAlive )
{
	REffectSceneNode effectSceneNode;
	bool bAlive = effectSceneNode.IsAlive();
	EXPECT_FALSE( bAlive ) << "이펙트 씬노드는 IsAlive()가 false를 반환해야 한다."; 
}

TEST_F( TestEffectSceneNode, IsLoopingDefault )
{
	REffectSceneNode effectSceneNode;
	bool bLooping = effectSceneNode.IsLooping();
	EXPECT_FALSE( bLooping );

	// REffectSceneNode 밑에 다른 REffectSceneNode를 자식으로 붙여본다
	REffectSceneNode childEffectSceneNode;
	effectSceneNode.AddChild( &childEffectSceneNode );
	effectSceneNode.AddToSceneNodeGroup( &childEffectSceneNode );
	bLooping = effectSceneNode.IsLooping();
	EXPECT_FALSE( bLooping );

	// 테스트용 이펙트를 자식으로 붙여본다
	RTestEffectBase testEffectFirst;
	childEffectSceneNode.AddChild( &testEffectFirst );
	RTestEffectBase testEffectSecond;
	childEffectSceneNode.AddChild( &testEffectSecond );
	
	testEffectFirst.SetLooping( false );
	testEffectSecond.SetLooping( false );
	bLooping = effectSceneNode.IsLooping();
	EXPECT_FALSE( bLooping ) << "이펙트 씬노드의 자식들 중 모두 Loop 속성이 아니면 false를 반환해야 한다.";

	testEffectFirst.SetLooping( true );
	testEffectSecond.SetLooping( false );
	bLooping = effectSceneNode.IsLooping();
	EXPECT_TRUE( bLooping ) << "이펙트 씬노드의 자식들 중 하나라도 Loop 속성을 가지면 true를 반환해야 한다.";

	testEffectFirst.SetLooping( true );
	testEffectSecond.SetLooping( true );
	bLooping = effectSceneNode.IsLooping();
	EXPECT_TRUE( bLooping ) << "이펙트 씬노드의 자식들 모두 Loop 속성을 가지면 true를 반환해야 한다.";

	testEffectFirst.RemoveFromParent();
	testEffectSecond.RemoveFromParent();
	childEffectSceneNode.RemoveFromParent();
}

TEST_F( TestEffectSceneNode, SetLooping )
{
	REffectSceneNode effectSceneNode;

	// REffectSceneNode 밑에 다른 REffectSceneNode를 자식으로 붙여본다
	REffectSceneNode childEffectSceneNode;
	effectSceneNode.AddChild( &childEffectSceneNode );

	// 테스트용 이펙트를 자식으로 붙여본다
	RTestEffectBase testEffectFirst;
	childEffectSceneNode.AddChild( &testEffectFirst );
	RTestEffectBase testEffectSecond;
	childEffectSceneNode.AddChild( &testEffectSecond );

	// REffectSceneNode에서 SetLooping해주면 자식들도 모두 Loop 속성이 바뀌어야 한다.
	effectSceneNode.SetLooping( false );
	EXPECT_FALSE( effectSceneNode.IsLooping() );
	EXPECT_FALSE( childEffectSceneNode.IsLooping() );
	EXPECT_FALSE( testEffectFirst.IsLooping() );
	EXPECT_FALSE( testEffectSecond.IsLooping() );

	effectSceneNode.SetLooping( true );
	EXPECT_TRUE( effectSceneNode.IsLooping() );
	EXPECT_TRUE( childEffectSceneNode.IsLooping() );
	EXPECT_TRUE( testEffectFirst.IsLooping() );
	EXPECT_TRUE( testEffectSecond.IsLooping() );

	testEffectFirst.RemoveFromParent();
	testEffectSecond.RemoveFromParent();
	childEffectSceneNode.RemoveFromParent();
}