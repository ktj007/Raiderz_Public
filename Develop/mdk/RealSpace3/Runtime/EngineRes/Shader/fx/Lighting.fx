//Sampler


sampler GIMap : register(s0);	// 현재는 AO
sampler LightMap : register(s1);
sampler DepthMap : register(s2);
sampler CloudMap : register(s3);
sampler GNormal : register(s4);
sampler GDepth : register(s5);
sampler SpotMap : register(s7);



// Transform
shared matrix	g_matWorldViewProj;
shared matrix	g_matWorldView;

//Screen-Quad의 View-공간 정점 위치 정보
shared float3 	g_vFarClipCornerInView;

shared float4	g_vResolution;

// Light Param
shared float3	g_vLightVec;
shared float3	g_vLightColor;
shared float3	g_vLightAmbient;
shared float4	g_vSkySpecularTerm; //xyz: view-space sky direction, w:intensity

// Shadow
shared float 	g_shadowLuminosity;
shared matrix  	g_lightTransform;
shared float4  	g_lightViewTransform;
shared float	g_fDepthTexelSize;
const float		g_fFilterSize = 1.5;
const float 	esm_k = 30 * 0.01;	// 우리는 cm단위를 쓰기때문에 0.01 을 곱함

// PSSM Shadow
#define MAX_SPLIT_COUNT 4
shared float4 	g_vCSMSplitInfo[MAX_SPLIT_COUNT];	// xyz = center, w = radius
shared float 	g_nSplitCount;
shared float4 	g_vDepthBias;
shared matrix  	g_lightTransformArray[MAX_SPLIT_COUNT];
shared matrix  	g_lightViewTransformArray;

// Cloud Shadow
shared matrix  	g_cloudTransform;
shared float	g_fCloudFactor;
shared matrix  	g_cloudTransform2;
shared float	g_fCloudFactor2;

// Spot Light
shared float3	g_vLightPosition;
shared float2	g_vAttenuationInfo;		//x: Radius, y: AttenuationStart	
shared float	g_fVisibility;
shared float4	g_vLightDirectionFOV;

// Spot Light Instancing
#define MAX_SPOT_INSTANCE 10
shared float3	g_vLightPositionArray[MAX_SPOT_INSTANCE];
shared float3	g_vLightInfoArray[MAX_SPOT_INSTANCE];		//x: Radius, y: AttenuationStart, z: Visibility
shared float3	g_vLightColorArray[MAX_SPOT_INSTANCE];
shared float4	g_vLightDirectionFOVArray[MAX_SPOT_INSTANCE];




float4 ReconstructPosFromDepth(float2 vScreencood, float fDepth)
{
	float3 vRay;
	vRay.xy = lerp(-g_vFarClipCornerInView.xy, g_vFarClipCornerInView.xy, vScreencood);
	vRay.z = g_vFarClipCornerInView.z;
	return float4(vRay*fDepth, 1);
}

float3 ReconstructNormal(float2 vScreencood, int nZDirection)
{
	float3 vViewNormal =0;
	vViewNormal.xy = tex2D(GNormal, vScreencood).xy;
	vViewNormal.z = sqrt(1.0f - vViewNormal.x*vViewNormal.x - vViewNormal.y*vViewNormal.y)*nZDirection;

	return vViewNormal;
}



//#ifdef CALC_SHADOW

/*
#define POISSON_COUNT 16

  float2 poissonDisk[POISSON_COUNT] = { 
  float2( -0.94201624, -0.39906216 ), 
  float2( 0.94558609, -0.76890725 ), 
  float2( -0.094184101, -0.92938870 ), 
  float2( 0.34495938, 0.29387760 ), 
  float2( -0.91588581, 0.45771432 ), 
  float2( -0.81544232, -0.87912464 ), 
  float2( -0.38277543, 0.27676845 ), 
  float2( 0.97484398, 0.75648379 ), 
  float2( 0.44323325, -0.97511554 ), 
  float2( 0.53742981, -0.47373420 ), 
  float2( -0.26496911, -0.41893023 ), 
  float2( 0.79197514, 0.19090188 ), 
  float2( -0.24188840, 0.99706507 ), 
  float2( -0.81409955, 0.91437590 ), 
  float2( 0.19984126, 0.78641367 ), 
  float2( 0.14383161, -0.14100790 ) 
};
*/
#define POISSON_COUNT 12
#define POISSON_HALF_COUNT 7

const float2 poissonDisk[POISSON_COUNT] = {
   -0.326212, -0.405805,
    0.962340, -0.194983,
   -0.840144, -0.073580,
    0.519456,  0.767022,
   -0.695914,  0.457137,
    0.185461, -0.893124,
    0.507431,  0.064425,
   -0.321940, -0.932615,
   -0.203345,  0.620716,
    0.473434, -0.480026,
   -0.791559, -0.597705,
    0.896420,  0.412458,
};


// 뿌아송은 탭 수가 적으면 밉다. 실질적으로는 균일한 원을 그리는 탭으로 지정.
const float2 poissonDiskHalf[POISSON_HALF_COUNT] = {
//  0,       1,
//  0.8659,  0.9988,
//  0.8662, -0.4996,
//  0,      -1,
// -0.8659, -0.9988,
// -0.8662,  0.4996, 

   -0.326212, -0.405805,
    0.962340, -0.194983,
   -0.840144, -0.073580,
    0.519456,  0.767022,
   -0.695914,  0.457137,
    0.185461, -0.893124,
    0.507431,  0.064425,

//  0.929592 , -0.796997 ,
//  0.150820 , -0.847052 ,
//  0.628401 , -0.205821 ,
//  0.494960 , 0.371327 ,
//  -0.123634 , -0.055148 ,
//  -0.460626 , 0.843394 ,

//  0.544224 , 0.507634 ,
//  0.043764 , 0.718759 ,
//  0.443738 , -0.699437 ,
//  -0.672118 , -0.036101 ,
//  0.387358 , 0.982086 ,
//  -0.999705 , -0.564346 ,
//  0.439794 , -0.308316 ,
};

#define DEPTHBIAS_FACTOR	2.0f

void IsShadow(float2 vecCoord, float d, out float2 outfShadow)
{
	float esm_k = 0.3f;

	outfShadow =1.0f;

 	float2 z = tex2Dlod( DepthMap, float4( vecCoord, 0,0) ).xy;
 	outfShadow = saturate( exp( esm_k * (z - d ) ) );
}

void IsShadow_Poisson(float2 vecCoord, float d, out float2 outfShadow)
{
	float2 fTexOffset = {g_fDepthTexelSize, g_fDepthTexelSize
#ifdef SHADOW_PSSM
		/g_nSplitCount
#endif
	};
	fTexOffset *= g_fFilterSize;

	outfShadow = 0.0f;
	
    for(int i = 0; i < POISSON_COUNT; ++i)
    {
		float2 shadowMapUV = vecCoord + fTexOffset * poissonDisk[i];
	
		float2 fShadow =1;
		IsShadow( shadowMapUV , d, fShadow);
		
		outfShadow +=fShadow;
    }
    
	outfShadow /= POISSON_COUNT;
}

void IsShadow_Poisson_Half(float2 vecCoord, float d, out float2 outfShadow)
{
	float2 fTexOffset = {g_fDepthTexelSize, g_fDepthTexelSize
#ifdef SHADOW_PSSM
		/g_nSplitCount
#endif
	};
	fTexOffset *= g_fFilterSize;

	outfShadow = 0.0f;
	
    for(int i = 0; i < POISSON_HALF_COUNT; ++i)
    {
		float2 shadowMapUV = vecCoord + fTexOffset * poissonDiskHalf[i];
	
		float2 fShadow =1;
		IsShadow( shadowMapUV , d, fShadow);
		
		outfShadow +=fShadow;
    }
    
	outfShadow /= POISSON_HALF_COUNT;
}
//#endif // #ifdef CALC_SHADOW




#define SUB_LIGHT_COUNT_MAX 4

shared float3 g_vSubLightColor[SUB_LIGHT_COUNT_MAX];
shared float3 g_vSubLightDir[SUB_LIGHT_COUNT_MAX];
shared float g_fSubLightIntensity;

#ifdef SUB_LIGHT_COUNT_4
	#define SUB_LIGHT_COUNT 4
#elif SUB_LIGHT_COUNT_3
	#define SUB_LIGHT_COUNT 3
#elif SUB_LIGHT_COUNT_2
	#define SUB_LIGHT_COUNT 2
#elif SUB_LIGHT_COUNT_1
	#define SUB_LIGHT_COUNT 1
#else
	#define SUB_LIGHT_COUNT 0
#endif

float3 GetSubLighting(float3 vViewNormal, float3 vDir, float3 vColor)
{
	float3 vLighting = dot(vDir, vViewNormal)*0.5f+0.5f;
	vLighting *= vLighting*vColor;
	
	return vLighting;
}

float3 GetSubLightingResult(float3 vViewNormal)
{
	float3 vResult =0;

	for(int i=0; i<SUB_LIGHT_COUNT; ++i)
	{
		vResult +=GetSubLighting(vViewNormal, g_vSubLightDir[i], g_vSubLightColor[i]);
	}

	return vResult *g_fSubLightIntensity;
}


float GetSkySpecular(float3 vReflect, float fPower, float fScale)
{
	return pow( max(dot(vReflect, g_vSkySpecularTerm.xyz),0), fPower) *fScale *g_vSkySpecularTerm.w;
}

float GetSpecular(float fSpecularCompress, float3 vView, float3 vViewNormal, float3 vLight)
{
	float fSpecularScale =0;
	float fSpecularPower =0;

	//modf(x,i) : x의 정수부분을 i로 리턴하고, 소수점 이하부분을 함수리턴값으로 리턴한다.
	fSpecularScale = modf( fSpecularCompress, fSpecularPower);

	float3 vReflect = reflect(-vView, vViewNormal);

	// 웬만하면 분기문을 사용 안하도록
	float immuneSpecular = saturate(sign(fSpecularScale));
	
	return lerp( 0, (pow ( max(dot(vReflect, vLight), 0), fSpecularPower ) * fSpecularScale), immuneSpecular)
#ifdef SKY_SPECULAR
		+ GetSkySpecular(vReflect, fSpecularPower, fSpecularScale)
#endif
		;
}

float GetRim(float fMask, float3 vViewNormal, float3 vView, float fViewPosZ)
{
// 	// 깊이 RT의 정보 y의 부호를 림 마스크로 사용 된다.		
// 	fSpecularTerm.z = saturate(sign(vDepthSpec.y));	// sign: -1 or 1 -> saturate: 0 or 1
	float cosView = abs(dot(vViewNormal, vView));
	cosView *= cosView; // 더 밝게


	//pow()를 쓰는 것보다 그냥 곱하는게 더 빠르다.
	//float power = 12; // 1이상의 값. 클수록 범위가 작아진다. ( 1 또는 4 권장 )
	float fRim = 1 - cosView;
	fRim *=fRim;			//^2
	fRim *=fRim;			//^4
	fRim = fRim*fRim*fRim	//^12
			*fMask;

	//fRim *= saturate(dot(-vView, vLight));


	// 실제 림 스케일은 깊이 기반으로 산출 필요.
	// 포그 구하는 공식으로 그대로 사용하자. 기존 CommitShaderGStage()에 있던 식 이상함.
	const float fRimFalloffFar		= 2000.f;
	const float fRimFalloffNear		= 1000.f;
	const float fFimReciprocalDist	= 1.0f / ( fRimFalloffFar - fRimFalloffNear);
	
	/// 1: 림가득 0: 림없음
	float rimScale = 1 - saturate( ((-fViewPosZ) - fRimFalloffNear) * fFimReciprocalDist);
	
	return fRim * rimScale;
}


float2 GetShadowESM(float4 vViewPos, float3 vViewNormal)
{
	float4 vDepthCoord	= 0;
	float4 vBiasedPosition = vViewPos;
	vBiasedPosition.xyz += vViewNormal*g_vDepthBias[0]*DEPTHBIAS_FACTOR;

	float4 vShadowRefUV = mul(vBiasedPosition ,g_lightTransform);
	vShadowRefUV /= vShadowRefUV.w;    
	float fShadowCompareD = dot(vBiasedPosition, g_lightViewTransform);

	float2 fShadow=1;
	#ifdef SHADOW_FILTER_POISSON
		IsShadow_Poisson_Half( float4(vShadowRefUV.xy,0,1), fShadowCompareD, fShadow );
	#else
		IsShadow( float4(vShadowRefUV.xy,0,1), fShadowCompareD, fShadow );
	#endif

	return fShadow;
}

float2 GetShadowProjection(float2 vRenderTargetCoord)
{
	return tex2D( DepthMap, vRenderTargetCoord ).xy;
}

float2 GetSplitShadow(float4 vViewPos, float3 vViewNormal, int nSplitPosition)
{
	float4 vBiasedPosition = vViewPos;
	vBiasedPosition.xyz += vViewNormal*g_vDepthBias[nSplitPosition]*DEPTHBIAS_FACTOR;

	float4 vDepthCoord = mul(vBiasedPosition ,g_lightTransformArray[nSplitPosition]);
	vDepthCoord.xyz  /= vDepthCoord.w;    
	vDepthCoord.y = vDepthCoord.y/g_nSplitCount + nSplitPosition/g_nSplitCount;
	vDepthCoord.z = dot(vBiasedPosition, g_lightViewTransformArray[nSplitPosition]);

	float2 refUV = vDepthCoord.xy;
	float d = vDepthCoord.z;
	
	float2 fShadow=1;
	#ifdef SHADOW_FILTER_POISSON
		[branch]
		if( nSplitPosition>0)
			IsShadow_Poisson_Half( refUV, d, fShadow );
		else
			IsShadow_Poisson( refUV, d, fShadow );
	#else
		IsShadow( refUV, d, fShadow );
	#endif // #ifdef SHADOW_FILTER_POISSON

	return fShadow;
}

float2 GetShadowPSSM(float4 vViewPos, float3 vViewNormal)
{
	float4 fDist=0;
	fDist.x = distance(vViewPos.xyz, g_vCSMSplitInfo[0].xyz);
	fDist.y = distance(vViewPos.xyz, g_vCSMSplitInfo[1].xyz);
	fDist.z = distance(vViewPos.xyz, g_vCSMSplitInfo[2].xyz);
	fDist.w = distance(vViewPos.xyz, g_vCSMSplitInfo[3].xyz);

	float4 fRadius = {g_vCSMSplitInfo[0].w, g_vCSMSplitInfo[1].w, g_vCSMSplitInfo[2].w, g_vCSMSplitInfo[3].w};
	int4 bInSplit= fRadius>=fDist;

	int4 Mask = bInSplit * float4(4,3,2,1);
	int nPos0 = 4-max(max(max(Mask.x, Mask.y), Mask.z), Mask.w);

	int CheckPos0[5] = {0, 1, 2, 3, -1};
	int CheckPos1[6] = {bInSplit, 0, 0};

	int2 nSplitPosition=-1;
	nSplitPosition.x = CheckPos0[nPos0];
	nSplitPosition.y = nSplitPosition.x +CheckPos1[nPos0+1];

	float fBlendArea = 0.2f;
	float fCSM_LerpTerm =fDist[nSplitPosition.x] / fRadius[nSplitPosition.x]; 	
	fCSM_LerpTerm =fCSM_LerpTerm = saturate(((1.0f-fCSM_LerpTerm)/fBlendArea));

	int2 fSplitPosition = nSplitPosition;

	float2 fShadow =1;

	[branch]
	if(nSplitPosition.x>=0)
	{
		fShadow = GetSplitShadow(vViewPos, vViewNormal, nSplitPosition.x);
	}

	#ifdef SHADOW_LERP_SPLIT
		[branch]
		if(nSplitPosition.x < nSplitPosition.y)
		{
			float2 fShadow2 = GetSplitShadow(vViewPos, vViewNormal, nSplitPosition.y);
			
			fShadow = lerp(fShadow2, fShadow, fCSM_LerpTerm);
		}
	#endif // #ifdef SHADOW_LERP_SPLIT

	// 	//------------------------------------------
	// 	//Dedug 용: 각 구간을 색으로 나타냄.
	// 	float3 DebugColor[8] = 
	// 	{	1, 0, 0,		//빨
	// 		0, 1, 0,		//녹
	// 		0, 0, 1,		//파
	// 		1, 1, 0,		//노
	// 		1, 0, 1,		//자주
	// 		0, 1, 1,		//하늘
	// 		1, 1, 1,		//백
	// 		0, 0, 0,		//흑
	// 	};
	// 	float3 Result=0;
	// 	if(nSplitPosition.x < nSplitPosition.y)
	// 	{
	// 		Result = DebugColor[nSplitPosition.x+4];
	// 	}
	// 	else
	//  	Result = DebugColor[nSplitPosition.x];
	// 
	// 	//fShadow =Result;
	// 	//------------------------------------------

	return fShadow;
}

float3 GetCloudShadow(float4 vViewPosition)
{
	float4 vCloudCoord = mul(vViewPosition, g_cloudTransform);
	float3 vCloudMapColor = tex2D(CloudMap, vCloudCoord.xy).rgb;
	float3 vCloudColor = lerp(vCloudMapColor, float3(1,1,1), g_fCloudFactor);

	#ifdef CLOUD2
	{
		float4 vCloudCoord2 = mul(vViewPosition, g_cloudTransform2);
		float3 vCloudMapColor2 = tex2D(CloudMap, vCloudCoord2.xy).rgb;
		float3 vCloudColor2 = lerp(vCloudMapColor2, float3(1,1,1), g_fCloudFactor2);
		vCloudColor *= vCloudColor2;
	}
	#endif // #ifdef CLOUD2

	return vCloudColor;
}


void GetShadowResult(float4 vViewPos, float3 vViewNormal, float2 vRenderTargetCoord, float fLighting, 
					 out float outfShadow, out float outfDuplicatedShadow, out float outShadowExceptCloud)
{
	float2 vShadow = 1;

	#ifdef SHADOW_PSSM
		[branch]
		if( fLighting > 0 )
			vShadow = GetShadowPSSM(vViewPos, vViewNormal);
		else
			vShadow = 0;
	#else
		#ifdef PROJECTION_SHADOW
			vShadow = GetShadowProjection(vRenderTargetCoord);
		#else
			vShadow = GetShadowESM(vViewPos, vViewNormal);
		#endif
	#endif // #ifdef SHADOW_PSSM

	outfShadow =min(vShadow.x, vShadow.y);
	outShadowExceptCloud = outfShadow;	// 구름을 적용 하지 않은 상태의 그림자


	float fDoubleShadow = vShadow.y;
	
	
	//Cloud Shadow
	#ifdef CLOUD
	{
		float3 vCloudColor = GetCloudShadow(vViewPos);
		outfShadow = min( outfShadow, vCloudColor.x);
	}
	#endif


	outfDuplicatedShadow = fLighting * (1-fDoubleShadow) * (bool)(outfShadow<1 && fDoubleShadow<1);	// 원래 값의 inv가 된다. 0이면 그림자가 없는 상태	
}

	
float GetAttenuation(float fStart, float fEnd, float3 vLightLength, float fVisibility)
{
	float fAttenuationRange = fEnd - fStart;
	float fAttenuation = (fEnd - vLightLength) / fAttenuationRange;
	fAttenuation = saturate(fAttenuation) *fVisibility;

	return fAttenuation;
}


void ApplyDoubleShadow(in float fDuplicatedShadow, inout float3 vResultColor)
{
	/// 겹치는 부분은 더 어둡게
	bool bDuplicatedShadow = (bool)(fDuplicatedShadow > 0);
		
	vResultColor -= (fDuplicatedShadow) * 0.15f * bDuplicatedShadow;
	vResultColor = max(vResultColor, 0.0f);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Dir Light
#ifdef DIR_LIGHT
struct VSINPUT
{
	float4 vPosition : POSITION;
	float2 vDiffuseCoord : TEXCOORD0;
};
struct VSOUTPUT
{
	float4 vPosition : POSITION;
	float2 vDiffuseCoord : TEXCOORD0;
};

VSOUTPUT VSMAIN(VSINPUT input)
{
	VSOUTPUT output;

	// Point Light의 경우 w에 Light 정보를 담고 있으므로 1로 설정해준다.
	input.vPosition.w = 1;
	output.vPosition = mul(input.vPosition, g_matWorldViewProj);
	output.vDiffuseCoord = input.vDiffuseCoord;

	return output;
}

float4 PSMAIN(VSOUTPUT input) : COLOR
{
#ifndef LIGHTING
	return float4(1,1,1, 0);
#endif

	float2 vRenderTargetCoord = input.vDiffuseCoord;

	float2 vDepthSpec = tex2D(GDepth, vRenderTargetCoord).xy;
	float fDepth = vDepthSpec.x;

	if( fDepth>=1.0f)
  		return float4(1,1,1,0);

	//DepthBuffer에 저장된 값의 부호는 Normal의 Z축 방향이다.
	int nZDirection = sign(fDepth);
	fDepth = abs(fDepth);

	float4 vPosition = ReconstructPosFromDepth(vRenderTargetCoord, fDepth);
	float3 vViewNormal = ReconstructNormal(vRenderTargetCoord, nZDirection);

//	float3 vLight = normalize(g_vLightVec);
	float3 vLight = g_vLightVec;

	float fLighting = saturate( dot(vLight, vViewNormal) );
	


	//-----------------------------------------------------------------------------------------
	//Specluar
	float fSpecular = 0;
	float fRim = 0;
	
	#ifndef EXPORT_SHADOW_ONLY
		#ifdef SPECULAR
			float3 vView = normalize(-vPosition.xyz);

			fSpecular = GetSpecular(abs(vDepthSpec.y), vView, vViewNormal, vLight);

			#ifdef RIM
				fRim = GetRim(saturate(sign(vDepthSpec.y)), vViewNormal, vView, vPosition.z);
			#endif
		#endif
	#endif
	//-----------------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------------
	//Shadow
	float fShadowFinal = 1;
	float fDuplicatedShadow = 0;
	float fShadowExceptCloud =1;

	#ifdef IMPORT_SHADOW
		float3 vShadowValue = tex2D( LightMap, vRenderTargetCoord).rgb;
		fShadowFinal = vShadowValue.x;
		fDuplicatedShadow = vShadowValue.y;
	#else
		#ifdef CALC_SHADOW
			GetShadowResult(vPosition, vViewNormal, vRenderTargetCoord, fLighting,	//input
				fShadowFinal, fDuplicatedShadow, fShadowExceptCloud);				//output			
		#endif
	#endif

	#ifdef EXPORT_SHADOW_ONLY
		return float4( fShadowFinal, fDuplicatedShadow, fShadowExceptCloud, 1);	
	#endif
	//-----------------------------------------------------
	
	
	float fLightingWithShadowResult = max(fLighting*fShadowFinal, g_shadowLuminosity);	
	
	float3 vResultColor = fLightingWithShadowResult*g_vLightColor +g_vLightAmbient +GetSubLightingResult(vViewNormal) +fRim;
	

	ApplyDoubleShadow(fDuplicatedShadow, vResultColor);


	#ifdef IMPORT_GI
		vResultColor *= tex2D( GIMap, vRenderTargetCoord).r;
	#endif	// #ifdef IMPORT_GI

	return float4(vResultColor, fSpecular);
}

technique MainTechnique
{
    pass P0
    {
		VertexShader = compile vs_3_0 VSMAIN();
		PixelShader  = compile ps_3_0 PSMAIN();
    }
}
#endif // #ifdef DIR_LIGHT

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Point Light
#ifdef POINT_LIGHT
struct VSINPUT_POINTLIGHT
{
	float4 vPosition : POSITION;
	
	float3 vLightPos : NORMAL;
	float4 vColor : COLOR0;
	float3 vLightInfo : TEXCOORD0;	//x: Radius, y: Attenuation Start, z: Visibility
	
	float4 vAxisX : TEXCOORD1;
	float4 vAxisY : TEXCOORD2;
	float4 vAxisZ : TEXCOORD3;
};
struct VSOUTPUT_POINTLIGHT
{
	float4 vPosition : POSITION;
	
	float3 vLightPos : TEXCOORD1;
	float4 vColor : TEXCOORD2;
	float3 vLightInfo : TEXCOORD3;	//x: Radius, y: Attenuation Start, z: Visibility

#ifdef AREA_LIGHT
	float3 vAxisX : TEXCOORD4;
	float3 vAxisY : TEXCOORD5;
	float3 vAxisZ : TEXCOORD6;
	float3 vLightAreaRange : TEXCOORD7;
#endif
};

VSOUTPUT_POINTLIGHT VSMAIN_POINTLIGHT(VSINPUT_POINTLIGHT input)
{
	VSOUTPUT_POINTLIGHT output = (VSOUTPUT_POINTLIGHT)0;

	//Point Light
	float3 vRadius = input.vLightInfo.x;
	float3 vLightAreaRange = { input.vAxisX.w, input.vAxisY.w, input.vAxisZ.w };

	float3 vPosition = input.vPosition.xyz * (vRadius
#ifdef AREA_LIGHT
		+vLightAreaRange*0.5f
#endif
		);

	float4 vLocalPosition = 1;
#ifdef AREA_LIGHT
	vLocalPosition.x = dot(vPosition, input.vAxisX.xyz);
	vLocalPosition.y = dot(vPosition, input.vAxisY.xyz);
	vLocalPosition.z = dot(vPosition, input.vAxisZ.xyz);
#else
	vLocalPosition.xyz = vPosition;
#endif
	vLocalPosition.xyz += input.vLightPos;

	output.vLightPos = mul(float4(input.vLightPos,1), g_matWorldView).xyz;
	output.vLightInfo = input.vLightInfo;	
	output.vColor = input.vColor;
	
	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);

#ifdef AREA_LIGHT
	output.vAxisX =mul(float3(input.vAxisX.x, input.vAxisY.x, input.vAxisZ.x), g_matWorldView).xyz;
	output.vAxisY =mul(float3(input.vAxisX.y, input.vAxisY.y, input.vAxisZ.y), g_matWorldView).xyz;
	output.vAxisZ =mul(float3(input.vAxisX.z, input.vAxisY.z, input.vAxisZ.z), g_matWorldView).xyz;
	output.vLightAreaRange = vLightAreaRange;
#endif // #ifdef AREA_LIGHT

	return output;
}

float4 PSMAIN_POINTLIGHT( VSOUTPUT_POINTLIGHT input, float2 vPos : VPOS) : COLOR
{
	// http://www.gamerendering.com/category/rendering-methods/shaders-rendering-methods/
	float2 vRenderTargetCoord = vPos.xy / g_vResolution.xy + g_vResolution.zw;

	float2 vDepthSpec = tex2D(GDepth, vRenderTargetCoord).xy;
	float fDepth = vDepthSpec.x;

	//DepthBuffer에 저장된 값의 부호는 Normal의 Z축 방향이다.
	int nZDirection = sign(fDepth);
	fDepth = abs(fDepth);

	float4 vPosition = ReconstructPosFromDepth(vRenderTargetCoord, fDepth);
	float3 vViewNormal = ReconstructNormal(vRenderTargetCoord, nZDirection);


	float fRadius = input.vLightInfo.x;
	float fAttenuationStart = input.vLightInfo.y;
	float fVisibility = input.vLightInfo.z;
	float3 vLight = input.vLightPos - vPosition.xyz;
	float3 vLightColor = input.vColor.xyz;

#ifdef AREA_LIGHT
	float3 fLightRange = input.vLightAreaRange;

	float fX = clamp( dot(-vLight,input.vAxisX), - fLightRange.x/2.0f, fLightRange.x/2.0f);
	float fY = clamp( dot(-vLight,input.vAxisY), - fLightRange.y/2.0f, fLightRange.y/2.0f);
	float fZ = clamp( dot(-vLight,input.vAxisZ), - fLightRange.z/2.0f, fLightRange.z/2.0f);

	float3 vPrevLightPosition = vLight+vPosition.xyz;
	float3 vRealLightPosition = vPrevLightPosition + input.vAxisX*fX + input.vAxisY*fY +input.vAxisZ*fZ;

	vLight = vRealLightPosition-vPosition.xyz;

	clip(fRadius - length(vLight));
#endif

	float fAttenuation = GetAttenuation(fAttenuationStart, fRadius, length(vLight), fVisibility);

	vLight = normalize(vLight);
	

	//-----------------------------------------------------------------------------------------
	//Specluar
	float fSpecular = 0;
	
	#ifdef SPECULAR
		fSpecular = GetSpecular(abs(vDepthSpec.y), normalize(-vPosition.xyz), vViewNormal, vLight);
	#endif
	//-----------------------------------------------------------------------------------------

	float fLighting = saturate(dot(vLight, vViewNormal));
	float3 vLightColorFinal = vLightColor * fAttenuation * fLighting;
	float fSpecularFinal = fSpecular * fAttenuation;
	
	return float4(vLightColorFinal, fSpecularFinal);
}

technique PointLightTechnique
{
    pass P0
    {
		VertexShader = compile vs_3_0 VSMAIN_POINTLIGHT();
		PixelShader  = compile ps_3_0 PSMAIN_POINTLIGHT();
    }
}
#endif //#ifdef POINT_LIGHT

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// Spot Light
#ifdef SPOT_LIGHT
struct VSINPUT_SPOTLIGHT
{
	float4 vPosition : POSITION;
};
struct VSOUTPUT_SPOTLIGHT
{
	float4 vPosition : POSITION;
#ifdef SPOT_INSTANCING
	float3 vLightPos : TEXCOORD1;
	float3 vColor : TEXCOORD2;
	float3 vLightInfo : TEXCOORD3;	//x: Radius, y: Attenuation Start, z: Visibility
	float4 vLightDirectionFOV  : TEXCOORD4;
#endif
};

VSOUTPUT_SPOTLIGHT VSMAIN_SPOTLIGHT(VSINPUT_SPOTLIGHT input)
{
	VSOUTPUT_SPOTLIGHT output = (VSOUTPUT_SPOTLIGHT)0;

#ifdef SPOT_INSTANCING
	float iIndex = input.vPosition.w;
	output.vLightPos = g_vLightPositionArray[iIndex];
	output.vColor = g_vLightColorArray[iIndex];
	output.vLightInfo = g_vLightInfoArray[iIndex];
	output.vLightDirectionFOV = g_vLightDirectionFOVArray[iIndex];
#endif

	float4 vLocalPosition = input.vPosition;
	vLocalPosition.w = 1;
	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);
	
	return output;
}

float4 PSMAIN_SPOTLIGHT( VSOUTPUT_SPOTLIGHT input, float2 vPos : VPOS) : COLOR
{
	// http://www.gamerendering.com/category/rendering-methods/shaders-rendering-methods/
	float2 vRenderTargetCoord = vPos.xy / g_vResolution.xy + g_vResolution.zw;

	float2 vDepthSpec = tex2D(GDepth, vRenderTargetCoord).xy;
	float fDepth = vDepthSpec.x;

	//DepthBuffer에 저장된 값의 부호는 Normal의 Z축 방향이다.
	int nZDirection = sign(fDepth);
	fDepth = abs(fDepth);

	float4 vPosition = ReconstructPosFromDepth(vRenderTargetCoord, fDepth);
	float3 vViewNormal = ReconstructNormal(vRenderTargetCoord, nZDirection);


//-----------------------------------------------------------------------------------------
//Attenuation
#ifdef SPOT_INSTANCING
	float fRadius = input.vLightInfo.x;
	float fAttenuationStart = input.vLightInfo.y;
	float fVisibility = input.vLightInfo.z;
	float3 vLight = input.vLightPos - vPosition.xyz;
	float3 vLightColor = input.vColor;
	float4 vLightDirectionFOV = input.vLightDirectionFOV;
#else
	float fRadius = g_vAttenuationInfo.x;
	float fAttenuationStart = g_vAttenuationInfo.y;
	float fVisibility = g_fVisibility;
	float3 vLight = g_vLightPosition - vPosition.xyz;
	float3 vLightColor = g_vLightColor;
	float4 vLightDirectionFOV = g_vLightDirectionFOV;
#endif


	//-----------------------------------------------------------------------------------------
	//Attenuation
	float fAttenuation = GetAttenuation(fAttenuationStart, fRadius, length(vLight), fVisibility);

	vLight = normalize(vLight);

	float fFalloff = dot(vLightDirectionFOV.xyz, -vLight); // 노멀라이즈 된 vLight를 써야 한다.
	float fFOV = vLightDirectionFOV.w;
	float fFOVAttenuation = saturate((fFalloff-fFOV)/(1-fFOV));
	fAttenuation *= fFOVAttenuation;
	//-----------------------------------------------------------------------------------------

	


	//-----------------------------------------------------------------------------------------
	//Specluar
	float fSpecular = 0;
	
	#ifdef SPECULAR
		fSpecular = GetSpecular(abs(vDepthSpec.y), normalize(-vPosition.xyz), vViewNormal, vLight);
	#endif
	//-----------------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------------
	//Lighting
	float fLighting = saturate(dot(vLight, vViewNormal));
	float3 vLightColorFinal = vLightColor * fAttenuation * fLighting;
	float fSpecularFinal = fSpecular * fAttenuation;

	

	
#ifndef SPOT_INSTANCING

	//Shadow Acne 문제를 해결하기 위해 position을 normal방향으로 밀어준다.
	float4 vBiasedPosition = vPosition;
	float fBiasScale = 20+pow(vPosition.z*0.01f, 0.8);
	vBiasedPosition.xyz += vViewNormal*fBiasScale;

	float4 vDepthCoord = mul(vBiasedPosition, g_lightTransform);
	float fLightSpaceZSign = sign(vDepthCoord.z);
	vDepthCoord.xyz  /= vDepthCoord.w;
	float2 vShadowRefUV = vDepthCoord.xy;


	//-----------------------------------------------------------------------------------------
	//Shadow
#ifdef CALC_SHADOW
	float fShadowCompareD = dot(vBiasedPosition, g_lightViewTransform);
	float2 vShadow=1;
	IsShadow_Poisson_Half( float4(vShadowRefUV,0,1), fShadowCompareD, vShadow );
	float fShadowFinal = vShadow.x;//IsShadow_Poisson( float4(vShadowRefUV,0,1), fShadowCompareD );
	
	vLightColorFinal *= fShadowFinal;
	fSpecularFinal *= fShadowFinal;
#endif
	//-----------------------------------------------------------------------------------------


#ifdef SPOT_MAP
	vShadowRefUV.x = 1 - vShadowRefUV.x;
	float4 vSpotMapColor = tex2D(SpotMap, vShadowRefUV);

	//Light 뒤쪽은 영향을 받지 않게 한다.
	vSpotMapColor *= fLightSpaceZSign;
	
	vLightColorFinal *= vSpotMapColor.rgb;
	fSpecularFinal *= vSpotMapColor.a;
#endif // #ifdef SPOT_MAP
#endif // #ifndef SPOT_INSTANCING

	return float4(vLightColorFinal, fSpecularFinal);
}

technique SpotLightTechnique
{
	pass P0
	{
		VertexShader = compile vs_3_0 VSMAIN_SPOTLIGHT();
		PixelShader  = compile ps_3_0 PSMAIN_SPOTLIGHT();
	}
}
#endif // #ifdef SPOT_LIGHT


#ifdef PROJECTION_SHADOW
float4 PSMAIN_SHADOWRECEIVER(float2 vTexcoord : TEXCOORD0) : COLOR
{
	float2 vDepthSpec = tex2D(GDepth, vTexcoord).xy;
	float fDepth = vDepthSpec.x;

	//DepthBuffer에 저장된 값의 부호는 Normal의 Z축 방향이다.
	fDepth = abs(fDepth);

	float4 vPosition = ReconstructPosFromDepth(vTexcoord, fDepth);
	float4 vShadowUV = mul(vPosition ,g_lightTransform);

	return float4(tex2Dproj( DepthMap, vShadowUV ).rg, 0, 1);	
}

technique ShadowReceiverTechnique
{
	pass P0
	{
		PixelShader  = compile ps_3_0 PSMAIN_SHADOWRECEIVER();
	}
}
#endif










#ifdef SUB_LIGHT_ONLY
float4 PSMAIN_SUBLIGHT(float2 vRenderTargetCoord : TEXCOORD0) : COLOR
{
	float2 vDepthSpec = tex2D(GDepth, vRenderTargetCoord).xy;
	int nZDirection = sign(vDepthSpec.x);

	float3 vViewNormal = ReconstructNormal(vRenderTargetCoord, nZDirection);
	
	float3 vLighting = GetSubLightingResult(vViewNormal);

	return float4(vLighting, 0);
}


technique SubLightTechnique
{
	pass P0
	{
		PixelShader  = compile ps_3_0 PSMAIN_SUBLIGHT();
	}
}
#endif
