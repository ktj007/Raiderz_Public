#include "stdafx.h"
#include "RMaterial.h"
#include "RShaderCompositeManager.h"
#include "RActor.h"
#include "RSceneManager.h"
using namespace rs3;



TEST( TestReflectMap, TestApplyReflectMap_HasReflect_ShouldHasReflect )
{
	//설치
	RSCID ReflectRC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	//실행
	manager.AddReflectRSC( ReflectRC );
	RSCID ResultSC = ReflectRC;

	//검증
	EXPECT_TRUE( ResultSC.HasComponent(RSC_REFLECTMAP) );
	EXPECT_TRUE( ResultSC.HasComponent(RSC_PPL_REFLECT) );
}


// 트랜스폼 테스트, 
TEST( TestReflectMap, TestApplyReflectMap_HasTransform_ShouldHasTransform )
{
	//설치
	RSCID ReflectRC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	//실행
	manager.ApplyTransformComponent( ReflectRC, false, true );
	RSCID ResultSC = ReflectRC;

	//검증
	EXPECT_TRUE( ResultSC.HasComponent(RSC_TRANSFORM_CONSTANT) );
	EXPECT_TRUE( ResultSC.HasComponent(RSC_TRANSFORM) );
	EXPECT_TRUE( ResultSC.HasComponent(RSC_NORMAL) );
	EXPECT_TRUE( ResultSC.HasComponent(RSC_POSITION) );
}


// Texcoord Tranform 테스트
TEST( TestReflectMap, TestApplyReflectMap_HasTexcoordTransform_ShouldHasTexcoordTransform )
{
	//설치
	RSCID ReflectRC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	mtrl.SetFlag(RM_FLAG_TEXTURE_TRANSFORM);
	//실행
	manager.ApplyTexcoordComponent( ReflectRC, &mtrl, true, false );
	RSCID ResultSC = ReflectRC;

	//검증
	EXPECT_TRUE( ResultSC.HasComponent(RSC_TEXCOORD_TRANSFORM) );
}


// Texcoord 테스트
TEST( TestReflectMap, TestApplyReflectMap_HasTexcoord_ShouldHasTexcoord )
{
	//설치
	RSCID ReflectRC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	//실행
	manager.ApplyTexcoordComponent( ReflectRC, &mtrl, true, false );
	RSCID ResultSC = ReflectRC;

	//검증
	EXPECT_TRUE( ResultSC.HasComponent(RSC_TEXCOORD) );
}


// 노말맵 테스트
TEST( TestReflectMap, TestApplyReflectMap_HasNormalMap_ShouldHasNormalMap )
{
	//설치
	RSCID ReflectRC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	mtrl.GetTextureInfo(0,0)->SetTextureType(RTT_NORMAL);

	//실행
	manager.ApplyNormalMapComponent( ReflectRC, &mtrl, true );
	RSCID ResultSC = ReflectRC;

	//검증
	EXPECT_TRUE( ResultSC.HasComponent(RSC_PPL_G_NORMALMAP) );
}


// 스페큘러 맵 테스트
TEST( TestReflectMap, TestApplyReflectMap_HasSpecularMap_ShouldHasSpecularMap )
{
	//설치
	RSCID ReflectRC;
	RMaterial mtrl;
	RShaderCompositeManager manager;

	mtrl.GetTextureInfo(0,0)->SetTextureType(RTT_SPECULAR);

	//실행
	ReflectRC = manager.GetTextureLayerReadComponent( &mtrl, RTT_SPECULAR );
	RSCID ResultSC = ReflectRC;

	//검증
	EXPECT_TRUE( ResultSC.HasComponent(RSC_SPECULARMAP) );
}