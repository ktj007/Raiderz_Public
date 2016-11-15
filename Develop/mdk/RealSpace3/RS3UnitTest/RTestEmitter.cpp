#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "RDevice.h" // TODO: 인클루드 안해도 되게 수정 하자.
#include "REmitter.h"
#include "RUtil.h"

using namespace rs3;

class TestREmitter : public REmitter
{
public:
	using REmitter::m_sEmitterDesc;
	using REmitter::Update;
	using REmitter::GetVisibilityAppliedColor;
};

class TestEmitter : public NullDeviceFixture
{
public:
	virtual ~TestEmitter()
	{
		Clear();
	}

	/**
		emitterLife : 이미터 라이프(ms)
		amount : 초당 파티클 생성 수
	*/
	TestREmitter* CreateEmitter( float emitterLife, unsigned int amount )
	{
		TestREmitter* pEmitter = new TestREmitter;
		_ASSERT( pEmitter );
		pEmitter->CreateNew();

		REmitterDescription& desc = pEmitter->m_sEmitterDesc;
		desc.fEmitterLife = emitterLife;
		desc.fParticleLife = CMinMax<float>( 10.f, 10.f ); // 파티클이 생성 될 수 있게 라이프를 적당히 준다.
		for( size_t i = 0; i < desc.vecEmitterVar.size(); ++i )
		{
			desc.vecEmitterVar[i].uAmount = amount;
		}
		pEmitter->Init();

		m_emitterList.push_back( pEmitter );
		return pEmitter;
	}

private:
	void Clear()
	{
		for( size_t i = 0; i < m_emitterList.size(); ++i )
		{
			delete m_emitterList[i];
		}
		m_emitterList.clear();
	}

	vector<TestREmitter*> m_emitterList;
};

const float EMITTER_LIFE = 100.f;
const unsigned int PARTICLE_AMOUNT = 100;

// 이미터 라이프 타임이 0일때의 상태 테스트
TEST_F( TestEmitter, EmitterStateAtLifeTimeZero )
{
	TestREmitter* pEmitter = CreateEmitter( 0.f, 0 );

	pEmitter->Start();
	EXPECT_EQ( REmitter::NOT_PLAY, pEmitter->GetCurrentPlayState() ) << "라이프 타임이 0일 때 Play되면 안된다.";

	DWORD dwCreatedParticleNum = pEmitter->Update( 1000.f );
	EXPECT_EQ( 0, dwCreatedParticleNum ) << "라이프 타임이 0일 때 파티클이 생성되면 안된다.";
}

TEST_F( TestEmitter, ParticleCountAtStart )
{
	TestREmitter* pEmitter = CreateEmitter( EMITTER_LIFE, PARTICLE_AMOUNT );

	pEmitter->Start();
	EXPECT_EQ( 0, pEmitter->GetCurrentCount() ) << "Start 하자마자 파티클이 생성되어서는 안된다.";
}

TEST_F( TestEmitter, ParticleCountAtUpdateZeroTime )
{
	TestREmitter* pEmitter = CreateEmitter( EMITTER_LIFE, PARTICLE_AMOUNT );

	pEmitter->Start();

	int prevParticleCount = pEmitter->GetCurrentCount();
	pEmitter->Update( 0.f );
	int currentParticleCount = pEmitter->GetCurrentCount() - prevParticleCount;
	EXPECT_EQ( 0, currentParticleCount ) << "Update 시간이 0이면 파티클이 생성되어서는 안된다.";
}

TEST_F( TestEmitter, ParticleCountAtUpdate )
{
	TestREmitter* pEmitter = CreateEmitter( EMITTER_LIFE, PARTICLE_AMOUNT );

	pEmitter->Start();

	int prevParticleCount = pEmitter->GetCurrentCount();
	pEmitter->Update( 1.f );
	int currentParticleCount = pEmitter->GetCurrentCount() - prevParticleCount;
	EXPECT_EQ( PARTICLE_AMOUNT, currentParticleCount ) << "1초에 Amount만큼의 파티클이 생성되어야 한다.";
}

TEST_F( TestEmitter, EmitterPositionUpdate )
{
	const RVector EMITTER_POS_START( 100.f, 200.f, 300.f );
	const RVector EMITTER_POS_END( 400.f, 500.f, 600.f );
	const float START_TIME = 3.f;
	const float CHECK_LIFE_TIME = 0.5f;
	const RVector EXPECT_POS( EMITTER_POS_START + ( EMITTER_POS_END - EMITTER_POS_START ) * CHECK_LIFE_TIME );

	TestREmitter* pEmitter = CreateEmitter( EMITTER_LIFE, 0 );
	REmitterDescription& desc = pEmitter->m_sEmitterDesc;
	ASSERT_GE( 2, (int)desc.vecEmitterVar.size() ) << "이미터 키 파라메터는 최소 2개가 있어야 한다.";
	desc.vecEmitterVar[0].vPos = EMITTER_POS_START;
	desc.vecEmitterVar[1].vPos = EMITTER_POS_END;
	pEmitter->Init();

	// Start시 위치 테스트
	pEmitter->SetPosition( RVector::ZERO );
	pEmitter->Start();
	EXPECT_EQ( EMITTER_POS_START, pEmitter->GetPosition() ) << "이미터가 Start되면 vecEmitterVar의 첫번째 키의 위치값이 설정되어야 한다.";

	// 중간 라이프 타임 위치 테스트
	pEmitter->Update( EMITTER_LIFE * CHECK_LIFE_TIME );
	RVector vr = pEmitter->GetPosition();
	EXPECT_EQ( EXPECT_POS, pEmitter->GetPosition() ) << "이미터 중간 프레임 위치값이 기대값과 다릅니다.";

	// 마지막 라이프 타임 위치 테스트
	pEmitter->Update( EMITTER_LIFE * ( 1.f - CHECK_LIFE_TIME ) );
	EXPECT_NE( EMITTER_POS_END, pEmitter->GetPosition() ) << "이미터 Life가 끝났을 때 위치 업데이트를 하지 않는다."; // 이게 맞는 동작일까?

	// 재시작시 위치 테스트
	pEmitter->SetStartTime( START_TIME ); // StartTime이 0이 아니여도 제대로 동작해야 함.
	pEmitter->Start();
	EXPECT_EQ( EMITTER_POS_START, pEmitter->GetPosition() ) << "이미터가 Start되면 vecEmitterVar의 첫번째 키의 위치값이 설정되어야 한다.";
}

TEST_F( TestEmitter, GetVisibilityAppliedColor )
{
	TestREmitter* pEmitter = CreateEmitter( EMITTER_LIFE, PARTICLE_AMOUNT );

	const float VISIBILITY = 0.5f;
	const RVector4 vTestColor( 1.f, 0.5f, 0.2f, 1.f );
	const RVector4 vExpectedColor = RVector4( vTestColor.r, vTestColor.g, vTestColor.b, vTestColor.a * VISIBILITY );
	const DWORD dwExpectedResultColor = RVector4ToDWORD( vExpectedColor );

	DWORD dwVisibilityAppliedColor = pEmitter->GetVisibilityAppliedColor( vTestColor, VISIBILITY );
	EXPECT_EQ( dwExpectedResultColor, dwVisibilityAppliedColor ) << "이미터의 VisibilityResult값이 색상 알파값에 제대로 반영되지 않았다.";
}

// 라이프 타임이 0일때 IsAlive()체크.
TEST_F( TestEmitter, IsAliveAtLifeTimeZero )
{
	TestREmitter* pEmitter = CreateEmitter( 0, 0 );
	bool bAlive = pEmitter->IsAlive();
	EXPECT_FALSE( bAlive ) << "이미터 라이프가 0이면 파괴되여야 한다.";

	pEmitter->Start();
	bAlive = pEmitter->IsAlive();
	EXPECT_FALSE( bAlive ) << "이미터 라이프가 0이면 파괴되여야 한다.";

	RSceneNodeUpdateVisitor v;
	pEmitter->Traverse( &v );

	bAlive = pEmitter->IsAlive();
	EXPECT_FALSE( bAlive ) << "이미터 라이프가 0이면 파괴되여야 한다.";
}

TEST_F( TestEmitter, IsAlive )
{
	TestREmitter* pEmitter = CreateEmitter( EMITTER_LIFE, 0 );
	bool bAlive = pEmitter->IsAlive();
	EXPECT_TRUE( bAlive ) << "이미터가 한번도 업데이트 되지 않으면 살아있어야 한다.";

	pEmitter->Start();
	bAlive = pEmitter->IsAlive();
	EXPECT_TRUE( bAlive ) << "이미터가 한번도 업데이트 되지 않으면 살아있어야 한다.";

	pEmitter->Update( EMITTER_LIFE );
	bAlive = pEmitter->IsAlive();
	EXPECT_FALSE( bAlive ) << "이미터 라이프 만큼 업데이트 되고 파티클이 없으면 파괴되어야 한다";
}
