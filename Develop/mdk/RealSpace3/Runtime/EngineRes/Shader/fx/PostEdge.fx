texture g_txSrcColor;
texture g_txSrcEdge;
texture g_txSrcEdgeTest;

sampler2D g_samSrcEdgeTest =
sampler_state
{
    Texture = <g_txSrcEdgeTest>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};


sampler2D g_samSrcColor =
sampler_state
{
    Texture = <g_txSrcColor>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};
sampler2D g_samSrcEdge =
sampler_state
{
    Texture = <g_txSrcEdge>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

float	g_fScreenWidth;
float	g_fScreenHeight;
float	g_fEdgePower;
float	g_vEdgeTickNess;
float	g_fFactor	= 2.f;		// 색 진하기. 약하게 주면 연하게 나온다.


float offset[3] = { 0.0, 1.3846153846, 3.2307692308 }; 
float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 }; 


float4 PostEdgePS( in float2 Tex : TEXCOORD0 ) : COLOR
{
	float3 vTmpColor = tex2D( g_samSrcEdge, Tex ).rgb;
	float fColorSum = vTmpColor.r + vTmpColor.g + vTmpColor.b;
	
	if( 0.f >= fColorSum )
		discard;
	
	float2 off = (g_vEdgeTickNess / g_fScreenWidth, g_vEdgeTickNess / g_fScreenHeight );

	float s00 = tex2D( g_samSrcEdge, Tex + float2(-off.x, -off.y)).r;
	float s01 = tex2D( g_samSrcEdge, Tex + float2( 0,   -off.y)).r;
   	float s02 = tex2D( g_samSrcEdge, Tex + float2( off.x, -off.y)).r;

 	float s10 = tex2D( g_samSrcEdge, Tex + float2(-off.x,  0)).r;
   	float s12 = tex2D( g_samSrcEdge, Tex + float2( off.x,  0)).r;

   	float s20 = tex2D( g_samSrcEdge, Tex + float2(-off.x,  off.y)).r;
   	float s21 = tex2D( g_samSrcEdge, Tex + float2( 0,    off.y)).r;
   	float s22 = tex2D( g_samSrcEdge, Tex + float2( off.x,  off.y)).r;

   	float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
   	float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

   	float edgeSqr = (sobelX * sobelX + sobelY * sobelY);

	float outLineColor = 1.0 - (edgeSqr > 0.07 * 0.07);
	
	if( outLineColor < 1.f )
		return float4(tex2D( g_samSrcEdge, Tex ).rgb, 1.f);
		
	return float4(0, 0, 0, 0);
}


float4 PostEdgeBlurV( in float2 Tex : TEXCOORD0 ) : COLOR
{			
	float4 Color = 0;
	Color = tex2D( g_samSrcEdgeTest, Tex ) * weight[0];
	
	for (int i = 1 ; i < 3 ; i++)
	{ 
		Color += tex2D( g_samSrcEdgeTest, Tex +  float2(0.f, offset[i] * g_fFactor)  / g_fScreenHeight) * weight[i]; 
		Color += tex2D( g_samSrcEdgeTest, Tex -  float2(0.f, offset[i] * g_fFactor)  / g_fScreenHeight) * weight[i]; 
	} 
    return Color;    
}


float4 PostEdgeBlurH( in float2 Tex : TEXCOORD0 ) : COLOR
{	
	float4 Color = 0;
	Color = tex2D( g_samSrcEdgeTest, Tex ) * weight[0];
	
	for (int i = 1 ; i < 3 ; i++)
	{ 
		Color += tex2D( g_samSrcEdgeTest, Tex +  float2(offset[i] * g_fFactor, 0.f) / g_fScreenWidth) * weight[i]; 
		Color += tex2D( g_samSrcEdgeTest, Tex -  float2(offset[i] * g_fFactor, 0.f) / g_fScreenWidth) * weight[i]; 
	} 
    
    return Color;

}

technique PostEdge
{
    pass P0
    {
		VertexShader = null;
		PixelShader  = compile ps_3_0 PostEdgePS();	// 외곽선만 만들어 낸다.
		ZEnable = false;	
    }
}


float4 PostEdgeAdd(in float2 Tex : TEXCOORD0 ) : COLOR
{				
	float3 vColor = tex2D( g_samSrcEdge, Tex).rgb;
	float fColorAdd = vColor.r + vColor.g + vColor.b;
	if( 0 < fColorAdd )
		discard;
	float4 vRet = tex2D(g_samSrcEdgeTest, Tex);
	return vRet * g_fEdgePower;
}

float4 PostEdgeAdd_GammaCorrect(in float2 Tex : TEXCOORD0 ) : COLOR
{				
	float3 vColor = tex2D( g_samSrcEdge, Tex).rgb;
	float fColorAdd = vColor.r + vColor.g + vColor.b;
	if( 0 < fColorAdd )
		discard;
	float4 vRet = tex2D(g_samSrcEdgeTest, Tex);
	vRet.rgb = pow( vRet.rgb, 2.2f);
	return vRet * g_fEdgePower;
}


float4 CopyScene(in float2 Tex : TEXCOORD0 ) : COLOR
{		
	return tex2D( g_samSrcColor, Tex);
}

technique PostEdgeBlur
{
	pass P0
    {
		VertexShader = null;
		PixelShader  = compile ps_3_0 PostEdgeBlurH();
		ZEnable = false;
    }
    
    pass P1
    {
		VertexShader = null;
		PixelShader  = compile ps_3_0 PostEdgeBlurV();
		ZEnable = false;
    }
}

technique PostEdgeSceneCopy
{
	pass P0
    {
		VertexShader = null;
		PixelShader  = compile ps_3_0 CopyScene();
		ZEnable = false;	
	}
}

technique PostEdgeAddScene
{
	pass P0
    {
		VertexShader = null;
		PixelShader  = compile ps_3_0 PostEdgeAdd();
		ZEnable = false;
		
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
    }
	
	pass P1
    {
		VertexShader = null;
		PixelShader  = compile ps_3_0 PostEdgeAdd_GammaCorrect();
		ZEnable = false;
		
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
    }

}