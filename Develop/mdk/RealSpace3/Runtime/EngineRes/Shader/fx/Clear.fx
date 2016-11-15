struct VS_OUTPUT
{
    float4 Pos			: POSITION;
};

float4	ClearMain( VS_OUTPUT In ) : COLOR
{
	float fMaxDist = 1000000;	// 우리가 안쓰는 매우 큰 값
	return float4(fMaxDist,fMaxDist,fMaxDist,1);
}

Technique Clear
{
	pass P0 
	{
		PixelShader = compile ps_2_0 ClearMain();
	}
}

sampler DiffuseMap : register(s0);

struct RF_OUTPUT
{
    float4 Pos			: POSITION;
	float2 vDiffuseCoord : TEXCOORD0;
};

float4	RenderFloatPS( RF_OUTPUT In ) : COLOR
{
	float4 vDiffuseColor = tex2D( DiffuseMap, In.vDiffuseCoord );
	
	//깊이맵 값을 fMaxDist 값으로 나누면 깊이맵에 렌더링된 부분이 검은색으로만 나와서 원근을 확인할 수 없음.
	//깊이값에 따라 색이 다르게 나타나게 하기 위해 fMaxDist보다 작은 값으로 나눔.
	float c = vDiffuseColor.r/10000.0f;
	return float4(c,c,c,1);
}

float4	RenderAlphaPS( RF_OUTPUT In ) : COLOR
{
	float4 vDiffuseColor = tex2D( DiffuseMap, In.vDiffuseCoord );
	
	float c = vDiffuseColor.a;
	return float4(c,c,c,c);
}

Technique RenderFloatRenderTarget
{
	pass P0 
	{
		PixelShader = compile ps_2_0 RenderFloatPS();
	}
}

Technique RenderAlpha
{
	pass P0 
	{
		PixelShader = compile ps_2_0 RenderAlphaPS();
	}
}





//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Render StaticDepthMap to RenderTarget

sampler DepthMap: register(s0);

float3 g_TexcoordDelta;

float4	RenderDepthMapPS( float2 vTexcoord :TEXCOORD0 ) : COLOR
{
	float2 vT = vTexcoord+float2(g_TexcoordDelta.x, -g_TexcoordDelta.y);

	clip(sign(vT.x));
	clip(sign(vT.y));
	clip(sign(1-vT.x));
	clip(sign(1-vT.y));
	
	
	return tex2D( DepthMap,  vT);	
}

Technique RenderDepthMap
{
	pass P0 
	{
		PixelShader = compile ps_2_0 RenderDepthMapPS();
	}
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------





//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Combine StaticDepthMap & DynamicDepthMap

sampler StaticDepthMap: register(s0);
sampler DynamicDepthMap: register(s1);

float4	CombineDepthMapPS( float2 vTexcoord : TEXCOORD0) : COLOR
{
	float fStaticDepth = tex2D( StaticDepthMap, vTexcoord).r;
	float fDynamicDepth = tex2D( DynamicDepthMap, vTexcoord).r;

	float fDepthResult = min(fStaticDepth, fDynamicDepth);
	return float4(fDepthResult,0,0,1);
}

Technique CombineDepthMap
{
	pass P0 
	{
		PixelShader = compile ps_2_0 CombineDepthMapPS();
	}
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Combine Common ShadowMap & Double ShadowMap

sampler CommonShadowMap: register(s0);
sampler DoubleShadowMap: register(s1);

float4	CombineDoubleShadowMapPS( float2 vTexcoord : TEXCOORD0) : COLOR
{
	float fCommonDepth = tex2D( CommonShadowMap, vTexcoord).r;
	float fDoubleDepth = tex2D( DoubleShadowMap, vTexcoord).r;

	return float4(fCommonDepth,fDoubleDepth,0,1);
}

Technique CombineDoubleShadowMap
{
	pass P0 
	{
		PixelShader = compile ps_2_0 CombineDoubleShadowMapPS();
	}
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------