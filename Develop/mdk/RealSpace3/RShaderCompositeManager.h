#pragma once

#include "RShaderConstant.h"
#include "RShaderComponentID.h"
#include "RTechnique.h"


namespace rs3 {

static int RSC_COUNT = 0;

// 액터 쉐이더 조합 콤포넌트 ID
// dependency에 따라 순서를 맞추어줘야 합니다
const RSCID RSC_POSITION						( RSC_COUNT++ );
const RSCID RSC_POSITIONW						( RSC_COUNT++ );
const RSCID RSC_NORMAL							( RSC_COUNT++ );
const RSCID RSC_TANGENT							( RSC_COUNT++ );
const RSCID RSC_PHYSIQUE						( RSC_COUNT++ );
const RSCID RSC_PHYSIQUE_VBLUR					( RSC_COUNT++ );
const RSCID RSC_TRANSFORM_CONSTANT				( RSC_COUNT++ );
const RSCID RSC_INSTANCING						( RSC_COUNT++ );
const RSCID RSC_TRANSFORM						( RSC_COUNT++ );
const RSCID RSC_TRANSFORM_VBLUR					( RSC_COUNT++ );
const RSCID RSC_TRANSFORM_EDGE					( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_VERTEX					( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_TRANSFORM				( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_WORLD					( RSC_COUNT++ );
const RSCID RSC_TEXCOORD						( RSC_COUNT++ );
const RSCID RSC_TEXCOORD2						( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_DIFFUSE_MAPCHANNEL		( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_DIFFUSE_TRANS			( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_OPACITY_MAPCHANNEL		( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_OPACITY_TRANS			( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_NORMAL_MAPCHANNEL		( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_NORMAL_TRANS			( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_SPECULAR_MAPCHANNEL	( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_SPECULAR_TRANS			( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_SI_MAPCHANNEL			( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_SI_TRANS				( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_ENV_MAPCHANNEL			( RSC_COUNT++ );
const RSCID RSC_TEXCOORD_ENV_TRANS				( RSC_COUNT++ );
const RSCID RSC_DIFFUSEMAP						( RSC_COUNT++ );

//Diffuse Multi Layer
const RSCID RSC_LAYER_1STBLEND_NORMAL			( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_ADD				( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_SUBTRACT			( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_MULTIPLY			( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_DARKEN			( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_LIGHTEN			( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_AVERAGE			( RSC_COUNT++ );
const RSCID RSC_LAYER_1STBLEND_COLORBURN		( RSC_COUNT++ );
const RSCID RSC_DIFFUSEMAPLAYER2				( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_ADD				( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_SUBTRACT			( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_MULTIPLY			( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_DARKEN			( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_LIGHTEN			( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_AVERAGE			( RSC_COUNT++ );
const RSCID RSC_LAYER_2NDBLEND_COLORBURN		( RSC_COUNT++ );
const RSCID RSC_DIFFUSEMAPLAYER3				( RSC_COUNT++ );
const RSCID RSC_LAYER_RESULT_EXPORT				( RSC_COUNT++ );


const RSCID RSC_DIFFUSEMAP_BLEND				( RSC_COUNT++ );
const RSCID RSC_DIFFUSE_PARTSCOLOR_CONSTANT		( RSC_COUNT++ );
const RSCID RSC_INSTANCE_PARTSCOLOR				( RSC_COUNT++ );
const RSCID RSC_DIFFUSE_PARTSCOLOR				( RSC_COUNT++ );
const RSCID RSC_DIFFUSE_NOISEFADE				( RSC_COUNT++ );
const RSCID RSC_FACTOR							( RSC_COUNT++ );
const RSCID RSC_SPECULARMAP						( RSC_COUNT++ );
const RSCID RSC_NORMALMAP						( RSC_COUNT++ );
const RSCID RSC_DUAL_NORMALMAP_OVERAY			( RSC_COUNT++ );
const RSCID RSC_DUAL_NORMALMAP_AVERAGE			( RSC_COUNT++ );
const RSCID RSC_OPACITYMAP						( RSC_COUNT++ );
const RSCID RSC_SELFILLUMINATIONMAP				( RSC_COUNT++ );
const RSCID RSC_DUAL_SPECULARMAP_COLORBURN		( RSC_COUNT++ );
const RSCID RSC_DUAL_SPECULARMAP_COLORDODGE		( RSC_COUNT++ );
const RSCID RSC_VCOLOR							( RSC_COUNT++ );	/// 버텍스 컬러를 아웃풋에 반영
const RSCID RSC_PPL_VALPHA_TO_VCOLOR			( RSC_COUNT++ );	/// 버텍스의 컬러에 버텍스의 알파를 곱한다. 알파 애디티브인데 버텍스의 컬러를 반영 하고 싶을 시
const RSCID RSC_EXPORT_NORMAL					( RSC_COUNT++ );
const RSCID RSC_PPL_VIEW_DIR					( RSC_COUNT++ );
const RSCID RSC_SHADOW_WRITE					( RSC_COUNT++ );
const RSCID RSC_DEPTH_WRITE_ESM					( RSC_COUNT++ );

const RSCID RSC_M_CONSTANT						( RSC_COUNT++ );
const RSCID RSC_LIGHTBUFFER_READ				( RSC_COUNT++ );	// M-Stage에서 L버퍼의 내용을 읽어서 라이팅 및 스페큘라를 셋팅하므로 이들이 사용 되기 전에 위치 해야 함.
const RSCID RSC_LIGHTBUFFER_READ_DSF			( RSC_COUNT++ );	// M-Stage에서 L버퍼의 내용을 DSF 필터를 사용 하여 읽어서 라이팅 및 스페큘라를 셋팅
const RSCID RSC_NORMALBUFFER_READ				( RSC_COUNT++ );

const RSCID RSC_PPL_SELFILLUMINATION			( RSC_COUNT++ );
const RSCID RSC_PPL_COS_VIEW					( RSC_COUNT++ );

const RSCID RSC_PPL_SPECULAR_CONSTANT			( RSC_COUNT++ );
const RSCID RSC_PPL_SPECULAR_PLAIN				( RSC_COUNT++ );
const RSCID RSC_PPL_SPECULAR_FROM_SPECULARMAP	( RSC_COUNT++ );
const RSCID RSC_PPL_SPECULAR_FROM_DIFFUSE_A		( RSC_COUNT++ );
const RSCID RSC_PPL_SPECULAR_FROM_NORMAL_A		( RSC_COUNT++ );
const RSCID RSC_PPL_SPECULAR_FROM_SI_A			( RSC_COUNT++ );
const RSCID RSC_PPL_SPECULAR_FROM_OPACITY_A		( RSC_COUNT++ );

const RSCID RSC_PPL_ALPHA_FROM_VCOLOR_A			( RSC_COUNT++ );
const RSCID RSC_PPL_ALPHA_FROM_DIFFUSE_A		( RSC_COUNT++ );
const RSCID RSC_PPL_ALPHA_FROM_SPECULAR_A		( RSC_COUNT++ );
const RSCID RSC_PPL_ALPHA_FROM_NORMAL_A			( RSC_COUNT++ );
const RSCID RSC_PPL_ALPHA_FROM_SI_A				( RSC_COUNT++ );
const RSCID RSC_PPL_ALPHA_FROM_OPACITY_A		( RSC_COUNT++ );

const RSCID RSC_DECAL							( RSC_COUNT++ );
const RSCID RSC_PPL_LIGHTING					( RSC_COUNT++ );
const RSCID RSC_PPL_WHITE						( RSC_COUNT++ );
const RSCID RSC_PPL_EDGE_COLOR					( RSC_COUNT++ );	// 외곽선 색깔
const RSCID RSC_PPL_PE_EDGE_MASK				( RSC_COUNT++ );
const RSCID RSC_PPL_GHOST						( RSC_COUNT++ );
const RSCID RSC_PPL_GLOW						( RSC_COUNT++ );
const RSCID RSC_ADD_SPECIFIC_COLOR				( RSC_COUNT++ );
const RSCID RSC_ADD_COLOR						( RSC_COUNT++ );
const RSCID RSC_PPL_FOG							( RSC_COUNT++ );
const RSCID RSC_PPL_VBLUR						( RSC_COUNT++ );
const RSCID RSC_PPL_VELOCITY					( RSC_COUNT++ );

// clip by alpha
const RSCID RSC_PPL_VISIBLITY					( RSC_COUNT++ );
const RSCID RSC_PPL_CLIP_ALPHA					( RSC_COUNT++ );

const RSCID RSC_GSTAGE_CONSTANT								( RSC_COUNT++ );
const RSCID RSC_PPL_G_NORMALMAP								( RSC_COUNT++ );
const RSCID RSC_REFLECTMAP								( RSC_COUNT++ );
const RSCID RSC_PPL_REFLECT								( RSC_COUNT++ );
const RSCID RSC_PPL_G_DEPTH									( RSC_COUNT++ );
const RSCID RSC_PPL_G_RIM									( RSC_COUNT++ );
const RSCID RSC_INSTANCE_AO									( RSC_COUNT++ );
const RSCID RSC_MASK_AO										( RSC_COUNT++ );
const RSCID RSC_PPL_G_WRITE_DIFFUSE_AO_RT2					( RSC_COUNT++ );	/// color0를 사용하기 때문에 RSC_PPL_G_WRITE_NORMAL_DEPTH_SPECULAR보다 앞에 있어야 함. A채널에는 AO 마스크를 씌움.
const RSCID RSC_PPL_G_WRITE_DSF_RT2							( RSC_COUNT++ );	/// 굳이 RSC_PPL_G_WRITE_NORMAL_DEPTH_SPECULAR보다 앞에 올 필요는 없지만 RSC_PPL_G_WRITE_DIFFUSE와의 짝을 맞추어 주기 위해
const RSCID RSC_PPL_G_WRITE_NORMAL_DEPTH_SPECULAR_RIM_RT01	( RSC_COUNT++ );


struct ShaderComponentPair
{
	const RSCID& flag;
	char* name;
};
// 위의 상수와 순서가 맞아야 합니다
static const ShaderComponentPair componentNames[] =
{
	{ RSC_POSITION, "position.rsc" },
	{ RSC_POSITIONW, "positionw.rsc" },
	{ RSC_NORMAL, "normal.rsc" },
	{ RSC_TANGENT, "tangent.rsc" },
#ifdef USING_RSC_PHYSIQUE_DQ	
	{ RSC_PHYSIQUE, "physique_dq.rsc" },
#else
 #ifdef USING_RSC_PHYSIQUE_QT
	{ RSC_PHYSIQUE, "physique_qt.rsc" },
 #else
	{ RSC_PHYSIQUE, "physique.rsc" },
 #endif
#endif
#ifdef USING_RSC_PHYSIQUE_QT
	{ RSC_PHYSIQUE_VBLUR, "physique_qt_vblur.rsc" },	
#else
	{ RSC_PHYSIQUE_VBLUR, "physique.rsc" },	
#endif
	{ RSC_TRANSFORM_CONSTANT, "transform_constant.rsc" },
	{ RSC_INSTANCING, "instancing.rsc" },
	{ RSC_TRANSFORM, "transform.rsc" },
	{ RSC_TRANSFORM_VBLUR, "transform_vblur.rsc" },
	{ RSC_TRANSFORM_EDGE, "transform_edge.rsc" },
	{ RSC_TEXCOORD_VERTEX, "texcoord_vertex.rsc" },
	{ RSC_TEXCOORD_TRANSFORM, "texcoord_transform.rsc" },
	{ RSC_TEXCOORD_WORLD, "texcoord_world.rsc" },
	{ RSC_TEXCOORD, "texcoord.rsc" },
	{ RSC_TEXCOORD2, "texcoord2.rsc" },
	{ RSC_TEXCOORD_DIFFUSE_MAPCHANNEL, "texcoord_diffuse_mapchannel.rsc" },
	{ RSC_TEXCOORD_DIFFUSE_TRANS, "texcoord_diffuse_trans.rsc" },
	{ RSC_TEXCOORD_OPACITY_MAPCHANNEL, "texcoord_opacity_mapchannel.rsc" },
	{ RSC_TEXCOORD_OPACITY_TRANS, "texcoord_opacity_trans.rsc" },
	{ RSC_TEXCOORD_NORMAL_MAPCHANNEL, "texcoord_normal_mapchannel.rsc" },
	{ RSC_TEXCOORD_NORMAL_TRANS, "texcoord_normal_trans.rsc" },
	{ RSC_TEXCOORD_SPECULAR_MAPCHANNEL, "texcoord_specular_mapchannel.rsc" },
	{ RSC_TEXCOORD_SPECULAR_TRANS, "texcoord_specular_trans.rsc" },
	{ RSC_TEXCOORD_SI_MAPCHANNEL, "texcoord_si_mapchannel.rsc" },
	{ RSC_TEXCOORD_SI_TRANS, "texcoord_si_trans.rsc" },
	{ RSC_TEXCOORD_ENV_MAPCHANNEL, "texcoord_env_mapchannel.rsc" },
	{ RSC_TEXCOORD_ENV_TRANS, "texcoord_env_trans.rsc" },

	
	{ RSC_DIFFUSEMAP, "diffusemap.rsc" },

	//Diffuse Multi Layer
	{ RSC_LAYER_1STBLEND_NORMAL, "layer_1stblend_normal.rsc" },
	{ RSC_LAYER_1STBLEND_ADD, "layer_1stblend_add.rsc" },
	{ RSC_LAYER_1STBLEND_SUBTRACT, "layer_1stblend_subtract.rsc" },
	{ RSC_LAYER_1STBLEND_MULTIPLY, "layer_1stblend_multiply.rsc" },
	{ RSC_LAYER_1STBLEND_DARKEN, "layer_1stblend_darken.rsc" },
	{ RSC_LAYER_1STBLEND_LIGHTEN, "layer_1stblend_lighten.rsc" },
	{ RSC_LAYER_1STBLEND_AVERAGE, "layer_1stblend_average.rsc" },
	{ RSC_LAYER_1STBLEND_COLORBURN, "layer_1stblend_colorburn.rsc" },
	{ RSC_DIFFUSEMAPLAYER2, "diffusemaplayer2.rsc" },
	{ RSC_LAYER_2NDBLEND_ADD, "layer_2ndblend_add.rsc" },
	{ RSC_LAYER_2NDBLEND_SUBTRACT, "layer_2ndblend_subtract.rsc" },
	{ RSC_LAYER_2NDBLEND_MULTIPLY, "layer_2ndblend_multiply.rsc" },
	{ RSC_LAYER_2NDBLEND_DARKEN, "layer_2ndblend_darken.rsc" },
	{ RSC_LAYER_2NDBLEND_LIGHTEN, "layer_2ndblend_lighten.rsc" },
	{ RSC_LAYER_2NDBLEND_AVERAGE, "layer_2ndblend_average.rsc" },
	{ RSC_LAYER_2NDBLEND_COLORBURN, "layer_2ndblend_colorburn.rsc" },
	{ RSC_DIFFUSEMAPLAYER3, "diffusemaplayer3.rsc" },
	{ RSC_LAYER_RESULT_EXPORT, "layer_result_export.rsc" },	


	{ RSC_DIFFUSEMAP_BLEND, "diffusemap_blend.rsc" },
	{ RSC_DIFFUSE_PARTSCOLOR_CONSTANT, "diffusemap_partscolor_constant.rsc" },
	{ RSC_INSTANCE_PARTSCOLOR, "instance_partscolor.rsc" },
	{ RSC_DIFFUSE_PARTSCOLOR, "diffusemap_partscolor.rsc" },
	{ RSC_DIFFUSE_NOISEFADE, "diffusemap_noisefade.rsc" },
	{ RSC_FACTOR, "factor.rsc" },

	{ RSC_SPECULARMAP, "specularmap.rsc" },
	{ RSC_NORMALMAP, "normalmap.rsc" },
	{ RSC_DUAL_NORMALMAP_OVERAY, "dual_normalmap_overay.rsc" },
	{ RSC_DUAL_NORMALMAP_AVERAGE, "dual_normalmap_average.rsc" },
	{ RSC_OPACITYMAP, "opacitymap.rsc" },
	{ RSC_SELFILLUMINATIONMAP, "selfilluminationmap.rsc" },
	{ RSC_DUAL_SPECULARMAP_COLORBURN, "dual_specularmap_colorburn.rsc" },
	{ RSC_DUAL_SPECULARMAP_COLORDODGE, "dual_specularmap_colordodge.rsc" },
	{ RSC_VCOLOR, "vcolor.rsc" },
	{ RSC_PPL_VALPHA_TO_VCOLOR, "ppl_valpha_to_vcolor.rsc" },
	{ RSC_EXPORT_NORMAL, "export_normal.rsc" },
	{ RSC_PPL_VIEW_DIR, "ppl_view_direction.rsc" },

	{ RSC_SHADOW_WRITE, "shadow_write.rsc" },
	{ RSC_DEPTH_WRITE_ESM, "depth_write_esm.rsc" },
	
	{ RSC_M_CONSTANT, "mstage_constant.rsc" },
	{ RSC_LIGHTBUFFER_READ, "lightbuffer_read.rsc" },
	{ RSC_LIGHTBUFFER_READ_DSF, "lightbuffer_read_dsf.rsc" },
	{ RSC_NORMALBUFFER_READ, "normalbuffer_read.rsc" },

	{ RSC_PPL_SELFILLUMINATION, "ppl_self_illumination.rsc" }, 
	{ RSC_PPL_COS_VIEW, "ppl_cos_view.rsc" },

	{ RSC_PPL_SPECULAR_CONSTANT, "ppl_specular_constant.rsc" },
	{ RSC_PPL_SPECULAR_PLAIN, "ppl_specular_plain.rsc" },
	{ RSC_PPL_SPECULAR_FROM_SPECULARMAP, "ppl_specular_from_specular_map.rsc" },
	{ RSC_PPL_SPECULAR_FROM_DIFFUSE_A, "ppl_specular_from_diffuse_a.rsc" },
	{ RSC_PPL_SPECULAR_FROM_NORMAL_A, "ppl_specular_from_normal_a.rsc" },
	{ RSC_PPL_SPECULAR_FROM_SI_A, "ppl_specular_from_si_a.rsc" },
	{ RSC_PPL_SPECULAR_FROM_OPACITY_A, "ppl_specular_from_opacity_a.rsc" },

	{ RSC_PPL_ALPHA_FROM_VCOLOR_A, "ppl_alpha_from_vcolor_a.rsc" },
	{ RSC_PPL_ALPHA_FROM_DIFFUSE_A, "ppl_alpha_from_diffuse_a.rsc" },
	{ RSC_PPL_ALPHA_FROM_SPECULAR_A, "ppl_alpha_from_specular_a.rsc" },
	{ RSC_PPL_ALPHA_FROM_NORMAL_A, "ppl_alpha_from_normal_a.rsc" },
	{ RSC_PPL_ALPHA_FROM_SI_A, "ppl_alpha_from_si_a.rsc" },
	{ RSC_PPL_ALPHA_FROM_OPACITY_A, "ppl_alpha_from_opacity_a.rsc" },

	{ RSC_DECAL, "decalmap.rsc" },
	{ RSC_PPL_LIGHTING, "ppl_lighting.rsc" },
	{ RSC_PPL_WHITE, "ppl_white.rsc" },
	{ RSC_PPL_EDGE_COLOR, "ppl_EdgeColor.rsc" },
	{ RSC_PPL_PE_EDGE_MASK, "ppl_PE_Edge_Mask.rsc" },
	{ RSC_PPL_GHOST, "ppl_ghost.rsc" },
	{ RSC_PPL_GLOW, "ppl_glow.rsc" },
	{ RSC_ADD_SPECIFIC_COLOR, "add_specific_color.rsc" },
	{ RSC_ADD_COLOR, "add_color.rsc" },
	{ RSC_PPL_FOG, "ppl_fog.rsc" },
	// about blur
	{ RSC_PPL_VBLUR, "ppl_vblur.rsc" },
	{ RSC_PPL_VELOCITY, "ppl_velocity.rsc" },
	// 모든 처리의 맨 마지막에 오되 clip 이전에 되어야 한다.
	{ RSC_PPL_VISIBLITY, "ppl_visiblity.rsc" },
	// clip by alpha
	{ RSC_PPL_CLIP_ALPHA, "ppl_clip_alpha.rsc" },
	// G-Stage
	{ RSC_GSTAGE_CONSTANT, "gstage_constant.rsc" },
	{ RSC_PPL_G_NORMALMAP, "ppl_g_normalmap.rsc" },
	{ RSC_REFLECTMAP, "reflectmap.rsc" },
	{ RSC_PPL_REFLECT, "ppl_reflect_map.rsc" },
	{ RSC_PPL_G_DEPTH, "ppl_g_depth.rsc" },
	{ RSC_PPL_G_RIM, "ppl_g_rim.rsc" },
	{ RSC_INSTANCE_AO,"instance_ao.rsc"},
	{ RSC_MASK_AO, "mask_AO.rsc" },
	{ RSC_PPL_G_WRITE_DIFFUSE_AO_RT2, "ppl_g_write_diffuse_ao_rt2.rsc" },
	{ RSC_PPL_G_WRITE_DSF_RT2, "ppl_g_write_dsf_rt2.rsc" },
	{ RSC_PPL_G_WRITE_NORMAL_DEPTH_SPECULAR_RIM_RT01, "ppl_g_write_normal_depth_specular_rim_rt01.rsc" },
};

class RShaderFXPool;
class RShaderFX;
class RShaderComponent;
class RShaderComposite;
class REnvironmentSet;
class RLightSceneNode;
class RCameraSceneNode;
class RMaterialDecal;
enum RSHADOWFILTERMETHOD;
enum RRENDERINGMETHOD;

// Samplers
enum ACTOR_SAMPLER 
{
	AS_DIFFUSE					= 0,
	AS_DECALMAP					= 0,	// 다음 Pass로 그려지기 때문에 0번에 할당한다.
	AS_TREE_COMPOSITE			= 0,
	AS_BLENDING_MATERIALMAP		= 1,	// 재질 교체에 사용할 텍스쳐 레이어
	AS_TREE_BRANCH_TEXTURE		= 1,
	AS_DEPTHMAP					= 2,
	AS_VIEWSPACE_DEPTHMAP		= 2,
	AS_REFLECT					= 2,
	AS_CLOUDMAP					= 3,
	AS_TREE_BILLBOARD_TEXTURE	= 3,
	AS_NOISEMAP					= 3,
	AS_NORMALMAP				= 4,
	AS_TREE_BILLBOARD_NORMAL	= 4,
	AS_SELFILLUMINATION			= 5,
	AS_TREE_BRANCH_DETAIL		= 5,
	AS_SPECULARMAP				= 6,
	AS_SPOTMAP					= 7,
	AS_OPACITYMAP				= 7,
	AS_VIEWSPACE_NORMALMAP		= 7,	// G-Stage에서 만들어 진 뷰 스페이스 기준 노말 버퍼.
	AS_VIEWSPACE_LIGHTMAP		= 8,	// L-Stage에서 만들어진 뷰 스페이스 기준 라이트 버퍼 정보
	AS_VIEWSPACE_DSFID			= 9,	// G-Stage에서 만들어 진 뷰 스페이스 기준 DSFID 버퍼.

	AS_GNORMAL					= 4,
	AS_GDEPTH					= 5,
	AS_GDIFFUSE					= 0,

	//Diffuse Layer
	AS_DIFFUSELAYER0MASK		 =2,
	AS_DIFFUSELAYER1			 =10,
	AS_DIFFUSELAYER1MASK		 =11,
	AS_DIFFUSELAYER2			 =12,
	AS_DIFFUSELAYER2MASK		 =13,

	AS_SECOND_NORMALMAP			 =14,
	AS_SECOND_SPECULARMAP		 =15
};



struct ComponentDesc_GStage
{
	RMaterial* pMtrl;
	bool bPhysique;
	bool bTexCoord;
	bool bExtraTexCoord;
	bool bVertexColor;
	bool bNoiseRef;
	bool bInstancing;
	bool bMultiplyColor;
	bool bAO;
	bool bBlendingMaterial;
};

struct ComponentDesc_MStage
{
	RMaterial* pMtrl;
	bool bAddColorPass;
	bool bPhysique;
	bool bVertexColor;
	bool bTexCoord;
	bool bExtraTexCoord;
	bool bUseShade;
	bool bBlendingMaterial;
	float fVisibility;
	bool bNoiseRef;
	bool bInstancing;
	bool bMultiplyColor;
};
 


class RS_API RShaderCompositeManager
{
public:
	// public shader constant
	static RShaderConstant		m_cWorldViewProj;
	static RShaderConstant		m_cWorldView;
	static RShaderConstant		m_cViewProj;
	static RShaderConstant		m_cView;
	static RShaderConstant		m_cViewInv;
	static RShaderConstant		m_cWorld;
	static RShaderConstant		m_cWorldPrev;
	static RShaderConstant		m_cPartsColor;		///< 파츠 컬러
	static RShaderConstant		m_cVisiblity;		///< 투명도
	static RShaderConstant		m_cBlendFactor;

	static RShaderConstant		m_cFogFactor;
	static RShaderConstant		m_cFogColor;

	static RShaderConstant		m_cEyePos;
	static RShaderConstant		m_cEyeDir;
	static RShaderConstant		m_cTextureTransform;
	static RShaderConstant		m_cDecalCenterPos;
	static RShaderConstant		m_cTextureFactor;
	static RShaderConstant		m_cGlossiness;
	static RShaderConstant		m_cDSFID;

	//////////////////////////////////////////////////////////////////////////
	// 스키닝 관련
	static RShaderConstant		m_cBlendMatrices;
#ifdef USING_RSC_PHYSIQUE_DQ	
	static RShaderConstant		m_cBlendDQ;				
#endif
	static RShaderConstant		m_cBlendQuats;			
	static RShaderConstant		m_cBlendTrans;			
	static RShaderConstant		m_cBlendQuatsPrev;		
	static RShaderConstant		m_cBlendTransPrev;		
	static RShaderConstant		m_cBlendScale;			

	// ETC
	static RShaderConstant		m_cGlowColorAndDiffuseRate;
	static RShaderConstant		m_cIlluminationScale;
	//static RShaderConstant		m_cRimScale;
	static RShaderConstant		m_cScreenSpaceTransform;	//M-Stage에서 사용 하는 정점 위치를 스크린 공간의 좌표로 변환해주는 행렬
	static RShaderConstant		m_cScreenSizeInv;
	static RShaderConstant		m_cMotionBlurFactorFront;		//오브젝트 픽셀 모션 블러에서 블러 방향 앞쪽의 블러 정도.
	static RShaderConstant		m_cAddColor;
	static RShaderConstant		m_cAlphaRef;
	// 인스턴싱 관련
	static RShaderConstant		m_cInstancingMatrices;
	static RShaderConstant		m_cEdgeColor;
	static RShaderConstant		m_cTickNess;
	static RShaderConstant		m_cPEEdgeColor;
	static RShaderConstant		m_cUseEnvShininessAlpha;

	static RShaderConstant		m_cUVTransform;
	static RShaderConstant		m_cMapChannel;

	//Multy Layer 관련
	static RShaderConstant		m_cDiffuseLayerOpacity;
	static RShaderConstant		m_cNormalLayerOpacity;
	static RShaderConstant		m_cSpecularLayerOpacity;
	static RShaderConstant		m_cUseMaskMap;
	
	static RShaderConstant		m_scFarDist;

	static RShaderConstant		m_cNoiseFadeParam;
	static RShaderConstant		m_cNoiseFadeColor1;
	static RShaderConstant		m_cNoiseFadeColor2;

	static RShaderConstant		m_cShadowValue;

	static RVector4				m_UVTransform[MAX_PIXELSAMPLER];
	static int					m_MapChannel[MAX_PIXELSAMPLER];
public:
	RShaderCompositeManager(void);
	~RShaderCompositeManager(void);

	void				Destroy();
	bool				Create();
	RShaderFX*			GetShader(RSCID rscComponents, bool bBackgroundLoading);

	unsigned int		GetShaderComponentCount();
	RShaderComponent*	GetShaderComponent(unsigned int nComponentID);
	RShaderFXPool*		GetShaderEffectPool();
	
	/// 해당 콤포넌트들의 이름을 목록으로 얻는다
	static void			GetComponentsNameList( RSCID rscComponents, vector<string>& outComponents );

	/// 콤포넌트ID를 이름으로 얻는다
	static RSCID		GetShaderComponentIDByName( const char* pComponentName );

	DWORD				GetCheckSumComponentCRCs();

	//--------------------------------------------------------------------------
	//Render State 설정 관련
	static void			SetGlobalRenderState( RCameraSceneNode* pCurrentCamera);

	void				SetMaterialRenderState( RMaterial* pMaterial, bool bUseNormalmap, bool bUseSpecularValue, bool bAlphaMesh);

	static void			SetCompositeLayerRenderState(RCompositeLayer* CompositeLayer);

	static void			SetTexture(int nStage, RMtrlTexInfo* pTexInfo, RTEXTUREFILTERTYPE filter =RTF_BILINEAR, int nMaxAnisotropy = 1);
	static void			SetTextureByType(RTextureType type, RMaterial* pMaterial, RTEXTUREFILTERTYPE filter = RTF_COUNT, int nMaxAnisotropy = -1);
	static void			SetCullMode(bool bTwoSide);
	static void			SetBlending(DWORD dwMaterialFlag, bool bAlphaMesh);
	static void			SetMaterialTexture(RMaterial* pMaterial, bool bUseNormalmap);
	void				SetMaterialShaderConstant(RMaterial* pMaterial, bool bUseSpecularValue, float fVisibility, float fSelfilluminationScale);
	static void			SetMaterialDecalRenderState(COMPOSITE_LAYER_INFO* pDecalLayer);
	//--------------------------------------------------------------------------



	///--------------------------------------------------------------------------
	/// Shader 조합 관련 
	void				ApplyNormalMapComponent( RSCID& rscComponent, RMaterial* pMtrl , bool _bUseNormalMap );				/// 노말맵 조합
	void				ApplyAlphaComponent( RSCID& rscComponent, RMaterial* pMtrl, bool bVColor );	/// 알파 관련 컴포넌트 조합
	void				ApplySpecularComponent( RSCID& rscComponents, RMaterial* pMtrl, bool bGstage);	/// 스페큘라 관련 컴포넌트 조합
	void				ApplyMaterialDecalComponents(RSCID& rscComponents);
	void				ApplyReflectComponent( RSCID& rscComponents, RMaterial* pMtrl );
	void				AddReflectRSC( RSCID& rscComponents );
	void				ApplyVertexColorComponent( RSCID& rscComponent, bool bVertexColor);
	void				ApplyDiffuseComponent( RSCID& rscComponent, RMaterial* pMtrl, bool bBlendingMaterial, bool bTexCoord);
	void				ApplyTexcoordComponent( RSCID& rscComponent, RMaterial* pMtrl, bool bTexCoord, bool bExtraTexCoord);
	void				ApplyMultiplyColorComponent( RSCID& rscComponents, bool bMultiplyColor);
	void				ApplyPartsColorComponent( RSCID& rscComponents, RMaterial* pMtrl, bool bTexCoord);
	void				ApplyNoiseFadeComponent( RSCID& rscComponents, bool bTexCoord, bool bNoiseFade);
	void				ApplyTransformComponent( RSCID& rscComponents, bool bPhysique, bool bHaveNormal);
	void				ApplyAOMaskComponent( RSCID& rscComponents, bool bAO);
	void				CheckAndApplyInstancingComponent( RSCID& rscComponents, bool bInstancing);

	RSCID				MakeComponentBitflagForGStage(RRENDERINGMETHOD renderMethod, ComponentDesc_GStage desc);
	RSCID				GetGStageOutputComponents( RRENDERINGMETHOD renderMethod);
	RSCID				MakeComponentBitflagForMStage(ComponentDesc_MStage desc);

	RSCID				GetDiffuseComponent(RMaterial* pMtrl);
	RSCID				GetDiffuseLayerBlendingComponent(int BlendingOrder, COMPOSITE_LAYER_BLEND_MODE mode);
	RSCID				GetTextureLayerReadComponent(RMaterial* pMtrl, RTextureType type);
	RSCID				GetSpecularMapComponent(RMaterial* pMtrl);


	RSCID				GetMaterialDecalDependentComponent();
	RSCID				GetIncompatibleComponentWithMaterialDecal();
	RSCID				GetMaterialDecalComponent();

	RSCID				GetVertexTexcoordComponent();
	RSCID				GetUVAniTexcoordComponent();
	RSCID				GetProjectionTexcoordComponent();
	//--------------------------------------------------------------------------



	//--------------------------------------------------------------------------
	//Fog 관련
	static void			FogOn(float fNear, float fFar, float fOffset, DWORD dwColor);
	static void			FogOff();

	static void			GetFogInfo(RVector4& outFogFactor, DWORD& outColor);

	static void			ClearFogColor();
	static void			RestoreFogColor();

	static void			SetFogColorForBlending(bool bAdditive);
	//--------------------------------------------------------------------------



	//////////////////////////////////////////////////////////////////////////
	// 툴 또는 디버깅을 위한 기능들
	bool				GetViewLightingOnly() const { return m_bViewLightingOnly; }
	void				SetViewLightingOnly(bool val);
	bool				GetSpecularEnable() const { return m_bSpecularEnable; }
	void				SetSpecularEnable(bool val);
	bool				GetDSF() const { return m_bUseDSF; }
	void				SetDSF(bool val);
	bool				GetUseNormalMap() const { return m_bUseNormalMap; }
	void				SetUseNormalMap(bool val);


	//-------------------------------------------------------
	//쉐이더 상수 설정 함수
	static void SetTransformConstant(const RMatrix& matWorld, const RMatrix& matView, const RMatrix& matViewProj);
	static void SetNoiseFadeConstant(RVector4 vParam, RVector4 vColor1, RVector4 vColor2);
	static void SetMultiLayerConstant(RTextureType type, RVector3 vOpacity, RVector3 vUseMaskMap);



protected:
	virtual	RShaderComposite*	NewShaderComposite(RSCID rscComponents);
	RShaderComposite*	GetExistShader(RSCID rscComponents);

	typedef map<RSCID, RShaderComposite*> RSCIDMAP;
	void				DestroyEffects( RSCIDMAP& effectMap );

	static void			SetFogColor(DWORD dwColor);
	static void			SetFogFactor(float fNear, float fFar, float fOffset);

	static RShaderFXPool*		m_pEffectPool;
	
	RSCIDMAP					m_effectMap;
	vector<RShaderComponent*>	m_shaderComponents;
	DWORD						m_dwCheckSumShaderComponentCRCs;	///< 쉐이더 조각의 CRC

	
	bool						m_bViewLightingOnly;	///< 라이팅만 보여주기
	bool						m_bSpecularEnable;		///< 스페큘러 사용 여부
	bool						m_bUseDSF;
	bool						m_bUseNormalMap;

	RSCID						m_defaultRSCID;
	RTechnique					m_defaultTechnique;

	static DWORD				m_dwFogColor;
	static RVector4				m_vFogFactor;

};

//////////////////////////////////////////////////////////////////////////
inline RShaderFXPool* RShaderCompositeManager::GetShaderEffectPool()					{	return m_pEffectPool;	}
inline unsigned int RShaderCompositeManager::GetShaderComponentCount()					{ return m_shaderComponents.size(); }
inline DWORD RShaderCompositeManager::GetCheckSumComponentCRCs()						{ return m_dwCheckSumShaderComponentCRCs; }

inline void RShaderCompositeManager::GetComponentsNameList( RSCID rscComponents, vector<string>& outComponents )
{
	outComponents.clear();

	for(int i=0; i< RSC_COUNT; i++)
	{
		if( (rscComponents & componentNames[i].flag).IsNonZero() )
		{
			outComponents.push_back( componentNames[i].name );
		}
	}
}
inline RSCID RShaderCompositeManager::GetShaderComponentIDByName( const char* pComponentName )
{
	for( int i = 0; i < RSC_COUNT; ++i )
	{
		if( _stricmp( pComponentName, componentNames[i].name ) == 0 )
			return componentNames[i].flag;
	}

	return RSCID();
}

}