#include "GammaCorrection.fx"

sampler DiffuseSampler	: register(s0);
sampler LightSampler	: register(s8);
sampler DepthSampler	: register(s5);
sampler NormalSampler	: register(s4);


sampler DiffuseLeftEye	: register(s1);
sampler DiffuseRightEye	: register(s2);


shared float4		g_vFogFactor = float4(0,0,0,0);   // x : near, y : far, z : dist , w : factor (multiply)
shared float3		g_vFogColor = float3(1,1,1);

//Screen-Quad의 View-공간 정점 위치 정보
float3 g_vFarClipCornerInView;


struct PSOUTPUT_DEFERRED {
	float4 color  : COLOR0;
	float4 color1 : COLOR1;
	float4 color2 : COLOR2;
	float4 color3 : COLOR3;
};

float4 ReadLight( in float2 uv)
{
	return tex2D( LightSampler, uv);
}

float3 ReadDiffuse( in float2 uv)
{
	return tex2D( DiffuseSampler, uv).rgb;
}

float ReadLitMask( in float2 uv)
{
	return tex2D( NormalSampler, uv).r;
}

float3 GetLightingColor( in float3 vDiffuse, in float4 vLight)
{
	float3 vClr = vDiffuse;
	// (Diffuse + specular) * lit
	vClr = ( vClr + vLight.aaa) * vLight.rgb;
	
	return vClr;
}

float3 GetMaskedLightingColor( in float3 vDiffuse, in float4 vLight, in float fLitMask)
{
	float3 vClr = vDiffuse;
	if( fLitMask != 10)
	{
		// (Diffuse + specular) * lit
		vClr = ( vClr + vLight.aaa) * vLight.rgb;
	}
	
	return vClr;
}

float ReadDepth( in float2 uv)
{
	// 깊이 얻음.	
	return abs(tex2D( DepthSampler, uv)).r;
}

float GetDepth( in float fDepth, in float2 uv)
{
	float3 vRay;
	vRay.xy = lerp(-g_vFarClipCornerInView.xy, g_vFarClipCornerInView.xy, uv);
	vRay.z = g_vFarClipCornerInView.z;

	return -fDepth * vRay.z;
}

float GetFog( in float posZ)
{
    //float fogNear = g_vFogFactor.x;
    //float fogEnd = g_vFogFactor.y;
    //float reciprocalfogDist = g_vFogFactor.z;
    //float fFactor = g_vFogFactor.w;

	// 0:포그없음 1:포그가득
    return saturate( (posZ - g_vFogFactor.x) * g_vFogFactor.z) + g_vFogFactor.w;
}

float4 PSmergeNoFog( in float2 uv : TEXCOORD0 ) : COLOR
{
	float fReadDepth = ReadDepth( uv);
	float3 vOut = ReadDiffuse( uv);	
	vOut = EncodeGammaCorrection( vOut );
	if( fReadDepth<1)
		vOut = GetLightingColor( vOut, ReadLight(uv));	
	return float4( vOut, 1);
}

float4 PSmergeFog( in float2 uv : TEXCOORD0 ) : COLOR
{
	float fReadDepth = ReadDepth( uv);
	float3 vOut = ReadDiffuse( uv );
	vOut = EncodeGammaCorrection( vOut );
	if( fReadDepth<1)
	{
		
		float posZ = GetDepth(fReadDepth, uv);
		float fFog = GetFog( posZ);
		float3 vLightingColor = GetLightingColor( vOut, ReadLight(uv));
		vOut = lerp( vLightingColor, g_vFogColor, fFog);
	}
	return float4( vOut, 1);
}


technique TmergeNoFog
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSmergeNoFog();
    }
}

technique TmergeFog
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSmergeFog();
    }
}




// 3D TEST//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
float4 PSmergeBlend( in float2 screenTC : TEXCOORD0 ) : COLOR
{
	float4 vLeft = tex2D( DiffuseLeftEye, screenTC);
	vLeft.r = 0.0f;
	//vLeft.b *= 0.5f;
	
	float4 vRight = tex2D( DiffuseRightEye, screenTC);
	vRight.g = 0.0f;
	//vLeft.b *= 0.5f;
	vLeft.b = 0.0f;
	
	float4 vBlend = (vLeft + vRight) ;
	
	return float4( vBlend.rgb, 1);
}

technique TBlend
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSmergeBlend();
    }
}
//////////////////////////////////////////////////////////////////////////////////