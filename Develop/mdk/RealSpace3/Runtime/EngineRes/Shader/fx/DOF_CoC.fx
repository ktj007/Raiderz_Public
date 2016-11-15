//-----------------------------------------------------------------------------
// File: DOF.fx
//
// Desc: DOF 구현. shaderx Tip&Trick의 것을 참조
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------

//static const int    NUM_DOF_TAPS            = 12;    // Number of outer filter taps

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4 viewportScale;
float4 viewportBias;

float maxCoC;

//float2 filterTaps[NUM_DOF_TAPS];
float2 filterTaps[12];

float depthScale;

//int g_nNumTaps = 12;

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);		// texture tSceneColor;
sampler s1 : register(s1);		// texture tBlurDepth;



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
float4 filter_dof_ps(PS_INPUT v) : COLOR
{
	// s0 - SceneColorSampler
	// s1 - DepthBlurSampler
	
	// Get center sample
	float4 colorSum = tex2D( s0, v.vTexCoord);
	float2 centerDepthBlur = tex2D( s1, v.vTexCoord);

	// Compute CoC size based on blurriness
	// y값이 blurriness값
	float sizeCoC = centerDepthBlur.y * maxCoC;

	float totalContribution = 1.0f;

	// Run through all taps
	//for (int i = 0; i < NUM_DOF_TAPS; i++)
	for (int i = 0; i < 12; i++)
	{
		// Compute tap coordinates
		float2 tapCoord = v.vTexCoord + filterTaps[i] * sizeCoC;

		// Fetch tap sample
		float4 tapColor = tex2D( s0, tapCoord);
		float2 tapDepthBlur = tex2D( s1, tapCoord);

		// Compute tap contribution
		float tapContribution = (tapDepthBlur.x > centerDepthBlur.x) ? 1.0f : tapDepthBlur.y;

		// Accumulate color and contribution
		colorSum += tapColor * tapContribution;
		totalContribution += tapContribution;
	}

	// Normalize to get proper luminance
	float4 finalColor = colorSum / totalContribution;

	return finalColor;
	
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
		PixelShader = compile ps_2_0 filter_dof_ps();
	}
}








//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 depth_blur_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = tex2D( s1, vTexCoord);

	return float4( BlurDepth.x * depthScale, BlurDepth.y, 0, 0);	
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
		PixelShader = compile ps_2_0 depth_blur_overlay_ps();
	}
}



//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 depth_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = tex2D( s1, vTexCoord);

	return float4( BlurDepth.x * depthScale, 0, 0, 0);	
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
		PixelShader = compile ps_2_0 depth_overlay_ps();
	}
}



//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 blur_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = tex2D( s1, vTexCoord);

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
		PixelShader = compile ps_2_0 blur_overlay_ps();
	}
}






























//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: DoF filter shader
//-----------------------------------------------------------------------------
//float4 filter_dof_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
//{
//	// s0 - SceneColorSampler
//	// s1 - DepthBlurSampler
//		
//	// Get center sample
//	float4 colorSum = tex2D( s0, vTexCoord);
//	float2 centerDepthBlur = tex2D( s1, vTexCoord);
//
//	// Compute CoC size based on blurriness
//	// y값이 blurriness값
//	float sizeCoC = centerDepthBlur.y * maxCoC;
//
//	float totalContribution = 1.0f;
//
//	// Run through all taps
//	//for (int i = 0; i < NUM_DOF_TAPS; i++)
//	for (int i = 0; i < 12; i++)
//	{
//		// Compute tap coordinates
//		float2 tapCoord = vTexCoord + filterTaps[i] * sizeCoC;
//
//		// Fetch tap sample
//		float4 tapColor = tex2D( s0, tapCoord);
//		float2 tapDepthBlur = tex2D( s1, tapCoord);
//
//		// Compute tap contribution
//		float tapContribution = ( tapDepthBlur.x > centerDepthBlur.x) ? 1.0f : tapDepthBlur.y;
//
//		// Accumulate color and contribution
//		colorSum += tapColor * tapContribution;
//		totalContribution += tapContribution;
//	}
//
//	// Normalize to get proper luminance
//	float4 finalColor = colorSum / totalContribution;
//
//	return finalColor;
//}
