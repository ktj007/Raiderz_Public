#pragma once

#include <string>
#include "RTypes.h"

using namespace rs3;
namespace RU3
{
	//-------------------------------------------------------------------------[
	//-------------------------------------------------------------------------[ 씬노드의 공통 데이타 ]
	class RUSceneNodeCommonDataDesc
	{
	public:
		std::string		m_NodeName;
		RVector			m_Position;
		RVector			m_Direction;
		RVector			m_Up;
		RVector			m_Sacle;
	};

	//-------------------------------------------------------------------------[ 콘트롤러 데이타 ] 
	class RUControllerDataDesc
	{
	};

	class RUActorNodeConstraintControllerDataDesc : public RUControllerDataDesc
	{
	public:
		std::string		m_TargetActorNodeName;

	};

	//-------------------------------------------------------------------------[ 프로퍼티 데이타 ]
	class RUPropertyDataDesc
	{
	};


	//-------------------------------------------------------------------------[ RActor ]
	class RUDistortionDataDesc
	{
	public:
		float	m_Dir0X,m_Dir0Y;
		float	m_Dir1X,m_Dir1Y;
		float	m_Scale;
		float	m_Speed0;
		float	m_Speed1;
		bool	m_bRenderable;
		std::string		m_NormalMapFileName;
		float	m_Range;
		float	m_AttenuationBegin;
	};

	class RULodDataDesc
	{
	public:
		bool	m_bEnableFarFogCulling;
		bool	m_bEnableDistanceCulling;
	};

	class RUActorPropertyDataDesc : public RUPropertyDataDesc
	{
	public:
		std::string		m_MeshFileName;
		bool			m_bExistCollisionTree;
		bool			m_bEnableAutoUpdate;
		bool			m_bISShadowReceiver;
	};

	//-------------------------------------------------------------------------[ Light ]
	class RUDirectionalLightPropertyDataDesc
	{
	public:
		float	m_ShadowLuminosity;
		float	m_HalfLambertFactor;
		float	m_Power;
		RVector	m_Ambient;
		RVector m_Diffuse;
		RVector	m_Specular;
	};


	class RUPointLightPropertyDataDesc
	{
	public:
		RVector	m_Color;
		float	m_Intensity;
		float	m_Radius;
		float	m_AttenuationStart;
		DWORD	m_Flags;
		RVector	m_AreaRange;
	};

	//-------------------------------------------------------------------------[ Water ]
	class RUWaterPropertyDataDesc
	{
	public:
		std::string	m_MeshFileName;
		float		m_DepthLimit;
		float		m_BumpUVScale1;
		float		m_BumpUVScale2;
		float		m_BumpUVScale3;
		float		m_BumpUVScale4;
		RVector2	m_BumpMove1;
		RVector2	m_BumpMove2;
		RVector2	m_BumpMove3;
		RVector2	m_BumpMove4;
		float		m_BumpAdjustFactor;
		float		m_BumpWeight1;
		float		m_BumpWeight2;
		float		m_BumpWeight3;
		float		m_BumpWeight4;
		std::string	m_BumpTex1FileName;
		std::string	m_BumpTex2FileName;
		bool		m_bUseCustomFog;
		float		m_FogNear;
		float		m_FogFar;
		float		m_UnderWaterFogNear;
		float		m_UnderWaterFogFar;
		float		m_UnderWaterDepth;
		RVector		m_FogColor;
		RVector		m_WaterColor;
		RVector		m_WaterRGBHalfLife;
		float		m_FresnelConstant;
		float		m_ReflectAdjust;
		float		m_VertexWaveScale;
		float		m_VertexWaveAmplitude1;
		float		m_VertexWavePhase1;
		float		m_Blending;
		float		m_RefractAttenuationDistance;
		float		m_RefractAttenuationFactor;
		std::string	m_ShoreTextureFileName;
		float		m_ShoreIntensity;
		RVector2	m_ShoreUVScale;


		
	};

	//-------------------------------------------------------------------------[ Terrain ]
	class RUTerrainLayerInfoDataDesc
	{
	public:
		int				m_Index;
		std::string		m_AlphaMapFileName;
		RVector			m_TileUV;
		bool			m_bUseSpecularTest;
		std::string		m_NormalMapFileName;


	};

	class RUTerrainLayerDataDesc
	{
	public:
		int	m_Size;
		RUTerrainLayerInfoDataDesc* m_pLayerInfoDataDescs;
	};

	class RUTerrainPropertyDataDesc
	{
	public:
		bool			m_bMakeNaveMesh;
		int				m_Mapwidth;
		int				m_MapHeight;
		RVector			m_MapSize;
		std::string		m_MapName;
		std::string		m_GrassFileName;
	};

	//-------------------------------------------------------------------------[ Tree ]
	class RUTreeSceneNodePropertyDataDesc
	{
	public:
		std::string		m_FileName;
	};

	//-------------------------------------------------------------------------[ Camera ]
	class RUCameraSceneNodePropertyDataDesc
	{
	public:
		float	m_NearZ;
		float	m_FarZ;
		float	m_FovY;
		float	m_AspectRatio;
	};

	//-------------------------------------------------------------------------[ Effect ]
	class RUEffectActorRotateDataDesc
	{
	public:
		float	m_RotateMin;
		float	m_RotateMax;

		enum RUROTATION_AXIS
		{
			X = 0,
			Y,
			Z
		};

		RUROTATION_AXIS	m_RotationAxis;
	};

	class RUEffectActorPropertyDataDesc : public RUActorPropertyDataDesc
	{
	public:
		enum RUEFFECTACOTOR_ALIGN_TYPE
		{
			NONE = 0,
			ALIGN,
			ALIGN_Z,
			ROTATE
		};
		RUEFFECTACOTOR_ALIGN_TYPE	m_AlignType;
		AnimationLoopType			m_AniLoopType;	
		float		m_StartTime;
		RVector		m_Color;
		bool		m_bInvisible;
	};

	class RUEffectInstancePropertyDataDesc
	{
	public:
		std::string		m_FileName;
		bool			m_bAutoStart;
	};

	class RUEmitterVariableDataDesc
	{
	public:
		float	m_fEventTime;
		UINT	m_Amount;
		RVector m_Range;
		RVector	m_Pos;
		float	m_Angle;
		float	m_MinVelocity;
		float	m_MaxVelocity;
	};

	class RUEmitterTextureDataDesc
	{
	public:
		std::string	m_TextureFileName;
		RBLEND		m_SrcBlend;
		RBLEND		m_DstBlend;
		bool		m_bAlphaTest;
		float		m_AlphaRef;
		RVector4	m_TexUV;
		RVector2	m_TexUVPivot;

	};

	class RUEmitterUVAniDataDesc
	{
		int		m_CountX;
		int		m_CountY;
		int		m_MaxFrame;
		float	m_FrameInterval;
		bool	m_bLoop;
	};

	class RUEmitterPropertyDataDesc
	{
	public:
		float				m_StartTime;
		EMITTER_TYPE		m_EmitterType;
		EMITTER_UVMOD_TYPE	m_UVModifierType;

		float			m_EmitterLife;
		bool			m_bLooping;
		bool			m_bSorting;
		bool			m_bRotate;
		bool			m_bDistortion;
		BILLBOARD_TYPE	m_BillboardType;
		CMinMax<float>	m_RotateRad;
		bool			m_bColorRand;
		bool			m_bColorRandRange;
		bool			m_bVelRand;
		bool			m_bGahter;
		bool			m_bExplosion;
		bool			m_bLocalTransform;
		bool			m_bFollowParentRotation;
		bool			m_bFixAmount;
		CMinMax<float>	m_ParticleLife;
		float			m_ParticleScale;

		bool			m_bInvisible;

		int				m_VariableArraySize;


	};
	//-------------------------------------------------------------------------[ Trail ]
	class RUTrailPropertyDataDesc
	{
	public:
		std::string		m_NodeName;
		float			m_Life;
		float			m_Width;

		bool			m_bIsBlendMode_Add;
		bool			m_bIsBlendMode_Alpha;
		bool			m_bIsSrcBlend;
		int				m_SrcBlend;
		int				m_DestBlend;

		bool			m_bAlign;
		int				m_Segment;
		bool			m_bLoop;

		RVector4		m_TexUV;
		//TEXUVTYPE		m_TexUVType;
		std::string		m_TextureFileName;
		bool			m_bInvisible;
		bool			m_bEnableDistortion;
		RUDistortionDataDesc	m_DistortionDataDesc;
	}
}