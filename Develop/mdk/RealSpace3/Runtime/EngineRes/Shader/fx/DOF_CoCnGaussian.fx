//-----------------------------------------------------------------------------
// File: DOF.fx
//
// Desc: DOF 구현. shaderx Tip&Trick의 것을 참조
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------
#define SATURATED_LINEAR_DEPTH	1

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4 viewportScale;
float4 viewportBias;

float g_fmaxCoC;

float2 filterTaps[13];


// Contains sampling offsets used by the techniques
float2 g_avSampleOffsets[13];
float4 g_avSampleWeights[13];

float g_fBlurFactor = 2;

int g_nNumTaps = 13;

//Scheuermann의 DOF 문서 방식을 채용
// - Near와 Far 플랜을 지정.
// - 포커스 플랜 너머의 블러의 최대 값을 지정 가능
// - 전방 클리어 하게를 파라미터로 지정 가능

// vDofParams 계수 : 
// x = near blur depth; y = focal plane depth; z = far blur depth 
// w = blurriness cutoff constant for objects behind the focal plane 
shared float4 g_vDofParams;

// 깊이 버퍼에 써져 있는 값은 0~1사이의 z/w값이다. 이를 실제 깊이 정보로 변환하기 위해서는 뷰프로젝션 인버스 매트릭스가 필요하다.
// 하지만 깊이만 필요하고 x,y는 필요가 없으므로 3,4열의 정보만 셋팅
//matrix	g_matProjInv;
float4 g_vProjInvZ;
float4 g_vProjInvW;

//Screen-Quad의 View-공간 정점 위치 정보
float3 g_vFarClipCornerInView;



//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);		// texture tSceneColor;
sampler s1 : register(s1);		// texture tBlurDepth blured;
sampler s2 : register(s2);		// texture tGaussian;
sampler s3 : register(s3);		// texture tBlurDepth org;
sampler s4 : register(s4);		// noise vector



float4 ReconstructPosFromDepth(float2 vScreencood, float fDepth)
{
	float3 vRay;
	vRay.xy = lerp(-g_vFarClipCornerInView.xy, g_vFarClipCornerInView.xy, vScreencood);
	vRay.z = g_vFarClipCornerInView.z;
	return float4(vRay*fDepth, 1);
}



struct VS_INPUT
{
	float4 vPos: POSITION;
	float2 vTexCoord: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 vPos: POSITION;
	float2 vTexCoord: TEXCOORD;
};

struct PS_INPUT
{
	float2 vTexCoord: TEXCOORD;
};


//-----------------------------------------------------------------------------
// Type: Vertex shader                                      
// Desc: Screen space vertex shader
//-----------------------------------------------------------------------------
VS_OUTPUT filter_dof_vs(VS_INPUT v)
{
	VS_OUTPUT o;

	// Scale and bias viewport
	o.vPos = v.vPos * viewportScale + viewportBias;

	// Output tex. coordinates
	o.vTexCoord = v.vTexCoord;

	return o;
}








//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: DoF filter shader
//-----------------------------------------------------------------------------

float4 GetResult( in float2 vTexCoord, in float2 vCenterBlurDepth, in float2 vRotation)
{
	float4 colorSum = 0;
	float fCenterBlur = vCenterBlurDepth.y;
	float fCenterDepth = vCenterBlurDepth.x;
	
	// CoC 탭의 반지름
	float fSizeCoC = fCenterBlur * g_fmaxCoC;
	float totalContribution = 0;
	
	// Run through all taps
	int nTapCnt = g_nNumTaps % 14;
	for (int i = 0; i < nTapCnt; i++)
	{
		float2 vRotatedKernel = reflect( filterTaps[i], vRotation);
		float2 tapCoord = vTexCoord + vRotatedKernel * fSizeCoC;
		
		// Fetch tap sample
		float4 tapColorOrg = tex2D( s0, tapCoord);
		float2 tapDepthBlur = tex2D( s1, tapCoord).xy;
		float4 tapColorGaussian = tex2D( s2, tapCoord);
	
		float fBlurFactor = g_fBlurFactor;
		// 신랜더에서 기록 한 z값을 비교 뎁스로 사용
		float fTapDepth = tapDepthBlur.x;
		float fTapBlur = tapDepthBlur.y;

		// 포커스 지점보다 멀리 있는 상은 블러 팩터를 변경하여 원본 값이 최소 반 이상은 반영 되도록 한다.
		if( g_vDofParams.y < fTapDepth)
			fBlurFactor = 2;
		
		// 가우시안의 것과 원본의 것을 선형 보간으로
		// 블러값이 작으면 org, 블러값이 크면 가우시안
		// lerp : 항상 헤깔린다. 인자가 1에 가까울수록 뒤에꺼, 인자가 0에 가까울수록 앞에꺼
		// 블러값이 1이어서 가우시안의 컬러만 추출하게되면, 가우시안의 해상도가 낮은 것 때문에 전반적인 해상도가 떨어져보인다.
		// 블러값이 아무리 높아도 원본의 컬러가 어느정도 반영되도록 하여 해상도를 높인다
		// g_fBlurFactor = 2 -> centerDepthBlur.y( 0~1) -> 0 ~ 0.5
		float4 tapColor = lerp( tapColorOrg, tapColorGaussian, fCenterBlur / fBlurFactor );
		
		// Compute tap contribution
		float tapContribution = ( fTapDepth >= fCenterDepth) ? 1.0f : fTapBlur;
		
		// Accumulate color and contribution
		colorSum += tapColor * tapContribution;
		totalContribution += tapContribution;
	}

	// Normalize to get proper luminance
	colorSum = colorSum / totalContribution;	
	colorSum.a = 1;

	return colorSum;
}

float2 GetCenterDepthBlur( in float2 vTexCoord)
{
	// s0 - SceneColorSampler
	// s1 - DepthBlurSampler
	
	// Get center sample
	float4 centerDepthBlur = tex2D( s1, vTexCoord);
	float4 centerDepthBlurOrg = tex2D( s3, vTexCoord);
	
	float fCenterDepth = centerDepthBlur.x;
	float fCenterBlur = centerDepthBlur.y;
	
	// 블러가 확장 된 것이 영향을 미치는데 원경의 블러가 확장 된 것이면 무시하고 원래의 것을 사용
	// 포커스에 가까운 상의 실루엣이 포커스 아웃된 블러링이 적용 되는 것을 막음
	if( centerDepthBlur.y > centerDepthBlurOrg.y && centerDepthBlur.x > centerDepthBlurOrg.x)
	{
		fCenterBlur = centerDepthBlurOrg.y;
	}
	
	return float2( fCenterDepth, fCenterBlur);
}

float4 filter_dof_ps_noise(PS_INPUT v) : COLOR
{
	float4 o;
	
	// 로테이션 벡터 얻음. 
	float2 rotationTC = v.vTexCoord * 18;
	float2 vRotation = normalize(2 * tex2D( s4, rotationTC).rg - 1);
	
	return GetResult( v.vTexCoord, GetCenterDepthBlur( v.vTexCoord), vRotation);
	
}

float4 filter_dof_ps(PS_INPUT v) : COLOR
{
	float4 o;
	
	return GetResult( v.vTexCoord, GetCenterDepthBlur( v.vTexCoord), float2(1,0));
	
}

//-----------------------------------------------------------------------------
// Name: FilterDoF
// Type: Technique                                     
// Desc: DoF filter shader
//-----------------------------------------------------------------------------
technique FilterDoF
{
	pass P0
	{
		VertexShader = compile vs_2_0 filter_dof_vs();
		PixelShader = compile ps_3_0 filter_dof_ps();
	}
}

technique FilterDoF_noise
{
	pass P0
	{
		VertexShader = compile vs_2_0 filter_dof_vs();
		PixelShader = compile ps_3_0 filter_dof_ps_noise();
	}
}




//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: make info buffer texture
//-----------------------------------------------------------------------------
float4 make_depth_blur_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
#ifdef SATURATED_LINEAR_DEPTH
	// Depth Buffer가 리니어로 바뀌어서 읽은 값 그냥 사용하면 됨.
	float fDepth = abs(tex2D( s0, vTexCoord)).x;
#else
	float4 vDepthInfo = abs(tex2D( s0, vTexCoord));
	float fDepth = -ReconstructPosFromDepth(vTexCoord, vDepthInfo).z;
#endif	

	float f;
	if( fDepth < g_vDofParams.y)
	{
		// scale depth value between near blur distance and focal distance to 
		// [-1, 0] range
		f = ( fDepth - g_vDofParams.y) / ( g_vDofParams.y - g_vDofParams.x);
		f = max( f, -1);
	}
	else
	{
		// scale depth value between focal distance and far blur distance to
		// [0, 1]
		f = ( fDepth - g_vDofParams.y) / ( g_vDofParams.z - g_vDofParams.y);
		// clamp the far blur to a maximum blurriness
		f = clamp (f, 0, g_vDofParams.w);
	}
	// scale and bias into [0, 1] range
	float blur = abs(f);
	
	return float4( fDepth, blur, 0, 1);
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: make info buffer texture
//-----------------------------------------------------------------------------
technique MakeDepthBlur
{
	pass P0
	{
		PixelShader = compile ps_2_0 make_depth_blur_ps();
	}
}





//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 depth_blur_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = abs(tex2D( s1, vTexCoord));

	return float4( BlurDepth.x, BlurDepth.y, 0, 0);	
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
technique DepthBlurOverlay
{
	pass P0
	{
		PixelShader = compile ps_3_0 depth_blur_overlay_ps();
	}
}



//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 depth_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = abs(tex2D( s1, vTexCoord));

	return float4( BlurDepth.x, 0, 0, 0);	
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
technique DepthOverlay
{
	pass P0
	{
		PixelShader = compile ps_3_0 depth_overlay_ps();
	}
}



//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 blur_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = abs(tex2D( s1, vTexCoord));

	return float4( BlurDepth.y, 0, 0, 0);	
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
technique BlurOverlay
{
	pass P0
	{
		PixelShader = compile ps_3_0 blur_overlay_ps();
	}
}







//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
float4 GaussBlur5x5PS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
    float4 sample = 0.0f;

	for( int i=0; i < 13; i++ )
	{
		sample += g_avSampleWeights[i] * tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
	}

	return sample;
}

//-----------------------------------------------------------------------------
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
technique GaussBlur5x5
{
    pass P0
    {
        PixelShader  = compile ps_3_0 GaussBlur5x5PS();
    }
}


//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
float4 Expand5x5PS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	float4 sample = abs(tex2D( s0, vScreenPosition));

	for( int i=0; i < 13; i++ )
	{
		float4 vOffset = tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
		// 주변의 큰 텍셀의 블러값을 채용하면서 텍셀의 깊이도 채용하여
		// 후처리 과정중 원경의 블러 값이 새어나온 것인지 아닌지를 판별하는 근거 자료로 사용
		if( sample.y < vOffset.y)
		{
			sample.xy = vOffset.xy;
		}	
	}

	return sample;
}

//-----------------------------------------------------------------------------
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point. + org
//-----------------------------------------------------------------------------
technique Expand5x5
{
    pass P0
    {
        PixelShader  = compile ps_3_0 Expand5x5PS();
    }
}



