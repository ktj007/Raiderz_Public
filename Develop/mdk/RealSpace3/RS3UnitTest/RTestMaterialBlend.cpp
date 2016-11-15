#include "stdafx.h"
#include "RMaterial.h"
#include "RShaderCompositeManager.h"
#include "RActor.h"

using namespace rs3;


class MockMaterialWithDiffuseTexture : public RMaterial
{
public:
	MockMaterialWithDiffuseTexture() : RMaterial()
	{
		MakeDiffuseTextureInfo();
	}

	virtual ~MockMaterialWithDiffuseTexture()
	{
	}

	void MakeDiffuseTextureInfo()
	{
		CreateTexInfo( 1);
		RMtrlTexInfo MtrlTexInfo;
		MtrlTexInfo.SetTextureType(RTT_DIFFUSE);

		AddMtrlTexInfo(0, MtrlTexInfo);
	}
};

//							테스트 하는 기능	  _ 선조건    _    기대 결과값
TEST( TestMaterialBlend, TestApplyDiffuseComponent_NoDiffuseSC_ShouldNotThrowDiffuseMapBlendSC)
{
	//설치
	RSCID NoDiffuseSC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	//실행
	manager.ApplyDiffuseComponent( NoDiffuseSC, &mtrl, true, true);
	RSCID ResultSC = NoDiffuseSC;

	//검증
	EXPECT_FALSE( ResultSC.HasComponent(RSC_DIFFUSEMAP_BLEND) );
}

TEST( TestMaterialBlend,  TestApplyDiffuseComponent_NoTextureCoordSC_ShouldNotThrowDiffuseMapBlendSC)
{
	//설치
	RSCID NoTextureCoordSC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	//실행
	manager.ApplyDiffuseComponent( NoTextureCoordSC, &mtrl, true, false);
	RSCID ResultSC = NoTextureCoordSC;

	//검증
	EXPECT_FALSE( ResultSC.HasComponent(RSC_DIFFUSEMAP_BLEND) );
}


TEST( TestMaterialBlend, TestApplyDiffuseComponent_MaterialWithDiffuseTexture_ShouldThrowDiffuseMapBlendSC)
{
	//설치
	RSCID ShaderComponent;
	MockMaterialWithDiffuseTexture MaterialWithDiffuseTexture;
	RShaderCompositeManager manager;

	//실행
	manager.ApplyDiffuseComponent( ShaderComponent, &MaterialWithDiffuseTexture, true, true);

	//검증
	EXPECT_TRUE( ShaderComponent.HasComponent(RSC_DIFFUSEMAP_BLEND) );
}


