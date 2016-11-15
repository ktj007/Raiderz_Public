#include "stdafx.h"
#include "RDevice.h" // TODO: 인클루드 안해도 되게 수정 하자.
#include "REmitter.h"
#include "RParticle.h"

using namespace rs3;

TEST( TestParticle, UpdateColorAndAlpha )
{
	class CTestParticle : public RParticle
	{
	public:
		void	UpdateColor( const RVector4& vColorFirst, const RVector4& vColorSecond, float fWeight ) 
		{ 
			RParticle::UpdateColor(vColorFirst, vColorSecond, fWeight); 
		}
		
		void	UpdateAlpha( const RVector4& vColorFirst, const RVector4& vColorSecond, float fWeight )
		{ 
			RParticle::UpdateAlpha(vColorFirst, vColorSecond, fWeight); 
		}
	};

	CTestParticle particle;
	particle.m_vColor.Set( 1.f, 1.f, 1.f, 1.f );
	REmitterDescription emitterDesc;
	emitterDesc.reset();

	const RVector4 vColorFirst( 1.f, 0.f, 1.f, 1.f );
	const RVector4 vColorSecond( 0.f, 1.f, 0.f, 0.f );
	const RVector4 vExpectedBlendingColor( 0.5f, 0.5f, 0.5f, 0.5f );

	particle.UpdateColor( vColorFirst, vColorSecond, 0.5f );
	EXPECT_EQ( vExpectedBlendingColor.r, particle.m_vColor.r );
	EXPECT_EQ( vExpectedBlendingColor.g, particle.m_vColor.g );
	EXPECT_EQ( vExpectedBlendingColor.b, particle.m_vColor.b );

	particle.UpdateAlpha( vColorFirst, vColorSecond, 0.5f );
	EXPECT_EQ( vExpectedBlendingColor.a, particle.m_vColor.a );
}

TEST( TestParticle, InitParticleColor )
{
	REmitterDescription desc;
	desc.reset();

	ASSERT_TRUE( desc.vecParticleVar.size() == 2 );
	const RVector4 vTestColor( 1.f, 0.5f, 0.2f, 0.4f );
	desc.vecParticleVar[0].vColor = vTestColor;

	RParticle particle;
	particle.Clear(); // TODO: Clear하지 않으면 메모리 참조 오류 남.
	particle.Init( &desc );

	EXPECT_EQ( particle.m_vColor.r, vTestColor.r );
	EXPECT_EQ( particle.m_vColor.g, vTestColor.g );
	EXPECT_EQ( particle.m_vColor.b, vTestColor.b );
	EXPECT_EQ( particle.m_vColor.a, vTestColor.a );
}