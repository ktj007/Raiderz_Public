//-----------------------------------------------------------------------------
// File: ScreenMotionBlur.fx
//
// Desc: 화면 전체에 대한 모션 블러 구현. - 090619, OZ
// http://http.developer.nvidia.com/GPUGems3/gpugems3_ch27.html
// http://aronze.egloos.com/1378393
// Waht a simple!!!!!!! 
//-----------------------------------------------------------------------------
	
//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);


float g_fDecreaseAlpha = 0.1f;

// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS_Accum( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{   
	float4 Out = 0;

	
	
	// 기존 축적
	float4 C0 = tex2D( s0, vTexCoord);
	// 추가 장면 마스킹
	float4 C1 = tex2D( s1, vTexCoord);	
	// 추가 장면 정보 : 컬러버퍼
	float4 C2 = tex2D( s2, vTexCoord);
	
 	// 추가 장면의 알파는 뭔가 그려져 있으면 1, 아니면 0
 	// 패스 랜더 과정에서 흰색으로 그리므로 rgb중 어느 하나의 속성을 그대로 알파로 사용 해도 됨
 	C2.a = sign( C1.r);
	C2.a = max( C2.a, 0);

	// 기존 축적의 알파 감쇠
	C0.a -= g_fDecreaseAlpha;
	C0.a = max( C0.a, 0);
	
	// 이전 장면을 현재 장면 위에 덮는 식으로 하자	
	Out.xyz = ( C0 * C0.a)
			+ ( C2 * (1-C0.a));
	
	Out.a = max( C0.a, C2.a);
	Out.a = min( Out.a, 1);
	
	return Out;
	
}
// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique Accum
{
    pass P0
    {
        PixelShader  = compile ps_2_0 PS_Accum();
    }
}



// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS_Blur( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{   
	float4 C0 = tex2D( s0, vTexCoord);	// 원본 장면
	float4 C1 = tex2D( s1, vTexCoord);	// 누적 소스
	
	float4 Out = 0;
	Out.xyz = ( C1.xyz * C1.a) + ( C0.xyz * (1-C1.a));
	Out.a = 1;
	
	return Out;
}
// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique Blur
{
    pass P0
    {
        PixelShader  = compile ps_2_0 PS_Blur();
    }
}



// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS_MaskInfo( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{   
	float4 C0 = tex2D( s0, vTexCoord);	// 누적버퍼
	
	float4 Out = float4( C0.r, C0.g, C0.b, 1);
	
	
	Out = C0;
	
//	Out.x = max( max( Out.x, Out.y), Out.z);
//	Out.y = Out.a;
	Out.a = 1;
	
	return Out;
}
// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique MaskInfo
{
    pass P0
    {
        PixelShader  = compile ps_2_0 PS_MaskInfo();
    }
}



// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS_Overlay( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{   
	float4 C0 = tex2D( s0, vTexCoord);	// 누적버퍼
	
	float4 Out = float4( C0.a, C0.a, C0.a, 1);
	
	
	Out = C0;
	
//	Out.x = max( max( Out.x, Out.y), Out.z);
//	Out.y = Out.a;
	Out.a = 1;
	
	return Out;
}
// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique Overlay
{
    pass P0
    {
        PixelShader  = compile ps_2_0 PS_Overlay();
    }
}
