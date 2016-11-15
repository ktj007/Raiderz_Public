///////////////////////////////////////////////////////////////////////////////
sampler DiffuseMap : register(s0);
sampler DepthBufferMap : register(s2);
sampler IlluminationMap : register(s5);

shared matrix	g_matWorld;
shared matrix	g_matWorldViewProj;

shared float4	g_vResolution;
shared float	g_fFarZ;	

shared float	g_fAlphaRef = 0;

shared float 	g_vIlluminationScale = 1;

shared float 	g_fDepthBiasBlendDist = 20.0f;

shared float 	g_fAttenuationBegin; // Distortion Param

shared float4	g_vFogFactor;   // x : near, y : far, z : dist , w : factor (multiply)
shared float3	g_vFogColor;

///////////////////////////////////////////////////////////////////////////////

struct VSINPUT
{
	float4 vPosition	: POSITION;
	float4 vColor		: COLOR;
	float2 vTexCoord0 	: TEXCOORD0;
	float3 vTexCoord1 	: TEXCOORD1;
};

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
	float4 vColor		: COLOR;
	float2 vTexCoord0 	: TEXCOORD0;
#ifdef MULTI_LAYER_BLENDING
	float3 vTexCoord1 	: TEXCOORD1;
#endif
#ifdef SOFT_PARTICLE
	float2 vDepth		: TEXCOORD3;
#endif
	float  fFog			: FOG;
};

VSOUTPUT VSMAIN( VSINPUT input )
{
	VSOUTPUT output = (VSOUTPUT)0;

	output.vPosition = mul( input.vPosition, g_matWorldViewProj );
	output.vColor = input.vColor;
	output.vTexCoord0 = input.vTexCoord0;
	
#ifdef MULTI_LAYER_BLENDING
	output.vTexCoord1 = input.vTexCoord1;
#endif

#ifdef SOFT_PARTICLE
	output.vDepth.x = output.vPosition.z;
	output.vDepth.y = output.vPosition.w;
#endif
	
	float fFogNear 			= g_vFogFactor.x;
//	float fFogEnd 			= g_vFogFactor.y;
	float fReciprocalfogDist= g_vFogFactor.z;
	float fFactor 			= g_vFogFactor.w;
	// 0:포그없음 1:포그가득
    output.fFog = saturate( ( output.vPosition.z - fFogNear ) * fReciprocalfogDist ) + fFactor;

	return output;
}

///////////////////////////////////////////////////////////////////////////////

struct PSOUTPUT
{
	float4 color0 : COLOR0;
	float4 color1 : COLOR1;
};

PSOUTPUT PSMAIN( 
	VSOUTPUT input 
#ifdef SOFT_PARTICLE
	, float4 vPos : VPOS
#endif
)
{
	PSOUTPUT output = (PSOUTPUT)0;

///////////////////////////////////////////////////////////////////////////////
// First Color
	float4 vDiffuseColor0 = tex2D( DiffuseMap, input.vTexCoord0 );
	vDiffuseColor0 *= input.vColor;

#ifdef SELF_ILLUMINATION
	float4 vSelfIlluminationColor0 = tex2D( IlluminationMap, input.vTexCoord0 );
	vSelfIlluminationColor0 *= g_vIlluminationScale;
	vDiffuseColor0.rgb += vSelfIlluminationColor0.rgb;
#endif


///////////////////////////////////////////////////////////////////////////////
// Second Color
#ifdef MULTI_LAYER_BLENDING
	float4 vDiffuseColor1 = tex2D( DiffuseMap, input.vTexCoord1.xy );
	vDiffuseColor1 *= input.vColor;
	
#ifdef SELF_ILLUMINATION
	float4 vSelfIlluminationColor1 = tex2D( IlluminationMap, input.vTexCoord1.xy );
	vSelfIlluminationColor1 *= g_vIlluminationScale;
	vDiffuseColor1.rgb += vSelfIlluminationColor1.rgb;
#endif

#endif


///////////////////////////////////////////////////////////////////////////////
// Final Color
#ifdef MULTI_LAYER_BLENDING
	float fBlend = input.vTexCoord1.z;
	float fBlendInverse = 1.f - fBlend;
	output.color0 = vDiffuseColor0 * fBlendInverse + vDiffuseColor1 * fBlend;
#else
	output.color0 = vDiffuseColor0;
#endif


///////////////////////////////////////////////////////////////////////////////
// Fog
	output.color0.rgb = lerp( output.color0.rgb, g_vFogColor, input.fFog );
	

///////////////////////////////////////////////////////////////////////////////
// Alpha Test
#ifdef ALPHA_TEST
	if( output.color0.a <= g_fAlphaRef )
		discard;
#endif


///////////////////////////////////////////////////////////////////////////////
// Soft Particle
#ifdef SOFT_PARTICLE
	float fPositionZ = input.vDepth.x;

	// 샘플링 할 점의 깊이를 얻음. 
	float2 vScreenCoords = vPos.xy / g_vResolution.xy + g_vResolution.zw;
	float fDepthValue = tex2D( DepthBufferMap, vScreenCoords).r;
	// DepthBuffer에 저장된 값의 부호는 NormalBuffer에 저장된 normal의 Z축 부호를 나타내므로 부호를 없애준다.
	fDepthValue = abs( fDepthValue );
	float fScreenZ = fDepthValue * g_fFarZ;
	
	float fBlendFactor = saturate( ( ( fScreenZ - fPositionZ ) / g_fDepthBiasBlendDist ) - 0.2f );
	fBlendFactor = pow( fBlendFactor, 2 );
	
#ifdef SOFT_PARTICLE_BLEND_ALPHA
	output.color0.a *= fBlendFactor;
#endif

#ifdef SOFT_PARTICLE_BLEND_COLOR
	output.color0.rgb *= fBlendFactor;
#endif

#endif

///////////////////////////////////////////////////////////////////////////////
// Distortion Masking
#ifdef DISTORTION_MASKING
	float fAttenuationBegin = g_fAttenuationBegin;
	output.color1 = min( output.color0.a / fAttenuationBegin, 1 );
#endif
	
	return output;
}

///////////////////////////////////////////////////////////////////////////////

technique ParticleTechnique
{
	pass P0
	{
		VertexShader = compile vs_3_0 VSMAIN();
		PixelShader  = compile ps_3_0 PSMAIN();
	}
}

///////////////////////////////////////////////////////////////////////////////