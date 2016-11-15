//-----------------------------------------------------------------------------
// File: EdgeDetect.fx
//
// Desc: 엣지 추출
//-----------------------------------------------------------------------------



const float maskLaplacian[9] = 
{ -1, -1, -1, 
  -1,  8, -1, 
  -1, -1, -1 }; // Laplacian필터



const float coord[3] = { -1, 0, +1 };


// x = RT width
// y = RT height
// z = 외곽선 생성 깊이 
// w = far
float4		vViewDimensions;


// 깊이 버퍼에 써져 있는 값은 0~1사이의 z/w값이다. 이를 실제 깊이 정보로 변환하기 위해서는 뷰프로젝션 인버스 매트릭스가 필요하다.
// 하지만 깊이만 필요하고 x,y는 필요가 없으므로 3,4열의 정보만 셋팅
//matrix	g_matProjInv;
float4 g_vProjInvZ;
float4 g_vProjInvW;

// x = default occlusion value
// y = kernel sphere radius 
// z = multiple of kernel radius to use for range truncation
// w = delta scale multiplier
float4		vSSAOParams = float4( 0.5f, 40.0f, 4.0f, 200.0f );



sampler DepthSampler	: register(s0);
sampler RandomSampler	: register(s1);
sampler AOSampler		: register(s2);	// AO 생성 시는 마스크로 사용
sampler NormalSampler	: register(s3);



float4 finalColor( in float fValue)  
{  
	float sample = fValue;
	
	return float4( sample, sample, sample, sample);
}  


float3 finalNormal( in float3 norm)
{
	norm.z = sqrt(1.0f - norm.x*norm.x - norm.y*norm.y);
	return norm;
}


//-----------------------------------------------------------------------------
float readDepth( in float2 coord)  
{  
	// 마스크를 읽음. 깊이가 0이 되면 AO가 발생 안하겠지..
	//float fMaskValue = saturate( 1 - tex2D( AOSampler, coord ).a) * vViewDimensions.w * 3;
	
	// 샘플링 할 점의 깊이를 얻음. 
	// fDepthValue : 0~1. 리니어 아님.
	float fDepthValue = tex2D( DepthSampler, coord ).r;
	float4 vDepthValue = float4( 0,0,fDepthValue,1);
	
	// 뷰 프로젝션 인버스 매트릭스를 곱하여 나온 결과를 w로 나눈 것과 같은 효과
	//return (dot( vDepthValue, g_vProjInvZ) / dot( vDepthValue, g_vProjInvW));
	return -(dot( vDepthValue, g_vProjInvZ) / dot( vDepthValue, g_vProjInvW));
}   



// shaderX 7의 소스를 오클루전 구하는 영역 부분만 위키 것으로 변형
float4 PSLaplacian( in float2 Tex : TEXCOORD0 ) : COLOR
{
	float Color = 0;
	float fDepthLimit = 40;

	// 라플라스 필터적용    
    for( int i = 0 ; i < 9 ; i++ )
		Color += maskLaplacian[i] * readDepth(Tex + float2( coord[i%3]/vViewDimensions.x, coord[i/3]/vViewDimensions.y)) / vViewDimensions.w * vSSAOParams.y * vSSAOParams.z;


	// 흰색에 가까울수록 외곽선을 뜻하는 상태서는 조금이라도 밝으면 다 흰색으로 바꾸어 버린다.
	float fBias = readDepth( Tex) / vViewDimensions.w;
	Color = smoothstep( fBias/2, fBias, Color);


	//Color = Color - 0.05f;

	//Color = smoothstep();

	//Color = saturate(Color);

	// 컬러를 Gray로 바꾼다
	Color = float4( Color, Color, Color, 1 );

    return Color;

}



technique TLaplacian
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSLaplacian();
    }
}


