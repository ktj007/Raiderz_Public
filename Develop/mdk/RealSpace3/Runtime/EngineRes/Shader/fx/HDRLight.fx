//-----------------------------------------------------------------------------
// File: HDRLight.fx
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------
static const int    MAX_SAMPLES            = 16;    // Maximum texture grabs

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
// Contains sampling offsets used by the techniques
float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];

float3 	g_vHDRBrightPassThreshold = float3( 1.f, 0.9f, 0.9f );

//-----------------------------------------------------------------------------
// for Quick Gaussian Filtering
//-----------------------------------------------------------------------------
// x = RT width
// y = RT height
// z = near
// w = far
float4		vViewDimensions = float4( 1440, 900, 0, 0);
const float fQuickOffset[3] = { 0.0, 1.3846153f, 3.2307692f };
const float fQuickWeight[3] = { 0.2270270f, 0.3162162f, 0.0702702f };


//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);
sampler s6 : register(s6);
sampler s7 : register(s7);

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Scale the source texture down to 1/16 scale
//-----------------------------------------------------------------------------
float4 DownScale4x4PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

	for( int i=0; i < 16; i++ )
	{
		sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
	}
	sample = sample * 0.0625f; // /16
	
	return sample;
}

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Scale the source texture down to 1/4 scale and Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------
float4 DownScale4x4nBrightPassFilterPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 sample = 0.0f;

		for( int i=0; i < 16; i++ )
	{
		sample += tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
	}
	sample = sample * 0.0625f; // /16
	
	// Subtract out dark pixels
	sample.rgb -= g_vHDRBrightPassThreshold; //g_fTonemapMax;	// BrightPassThreshold로 사용된다
	
	// Clamp to 0
	sample = max(sample, 0.0f);	
	
	return sample;
}

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
float4 GaussBlur5x5PS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{	
    float4 sample = 0.0f;

	for( int i=0; i < 12; i++ )
	{
		sample += g_avSampleWeights[i] * tex2D( s0, vScreenPosition + g_avSampleOffsets[i] );
	}

	return sample;
}

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------
float4 BrightPassFilterPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	float4 vSample = tex2D( s0, vScreenPosition );
	
	// Subtract out dark pixels
	vSample.rgb -= g_vHDRBrightPassThreshold; //g_fTonemapMax;	// BrightPassThreshold로 사용된다
	
	// Clamp to 0
	vSample = max(vSample, 0.0f);
  	
	return vSample;
}

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Blur the source image along one axis using a gaussian
//       distribution. Since gaussian blurs are separable, this shader is called 
//       twice; first along the horizontal axis, then along the vertical axis.
//-----------------------------------------------------------------------------
float4 BloomPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    // Perform a one-directional gaussian blur
    for(int iSample = 0; iSample < 15; iSample++)
    {
        vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];
        vColor = tex2D(s0, vSamplePosition);
        vSample += g_avSampleWeights[iSample]*vColor;
    }
	
    return vSample;
}

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Perform blue shift, tone map the scene, and add post-processed light
//       effects
//-----------------------------------------------------------------------------
struct PSOUTPUT {
	float4 color : COLOR0;
	float4 dof : COLOR1;
};


PSOUTPUT FinalScenePassPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{
	PSOUTPUT o;
	
    float4 vSample 		= tex2D(s0, vScreenPosition);
    vSample.a = 1.0f;	// framebuffer는 xrgb 포멧이다. alpha값은 fix로 두자
    float4 vBloom = tex2D(s1, vScreenPosition);
 	vSample += vBloom;

	o.color = vSample;
	o.dof =  tex2D(s3, vScreenPosition);
    return o;
}

//-----------------------------------------------------------------------------
// for Quick Gaussian Filtering
// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
// http://www.geeks3d.com/20100909/shader-library-gaussian-blur-post-processing-filter-in-glsl/
//-----------------------------------------------------------------------------
float4 QuickGaussBlurVerticalPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{	
    float4 sample = tex2D( s0, vScreenPosition ) * fQuickWeight[0];
    
    for (int i=1; i<3; i++)
    {
		sample += tex2D( s0, vScreenPosition + float2( 0, fQuickOffset[i]/vViewDimensions.y)) * fQuickWeight[i];
		sample += tex2D( s0, vScreenPosition - float2( 0, fQuickOffset[i]/vViewDimensions.y)) * fQuickWeight[i];
    }

	return sample;
}

float4 QuickGaussBlurHorizontalPS(in float2 vScreenPosition : TEXCOORD0) : COLOR
{	
    float4 sample = tex2D( s0, vScreenPosition ) * fQuickWeight[0];
    
    for (int i=1; i<3; i++)
    {
		sample += tex2D( s0, vScreenPosition + float2( fQuickOffset[i]/vViewDimensions.x, 0)) * fQuickWeight[i];
		sample += tex2D( s0, vScreenPosition - float2( fQuickOffset[i]/vViewDimensions.x, 0)) * fQuickWeight[i];
    }

	return sample;
}



//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Desc: Performs a single horizontal or vertical pass of the blooming filter
//-----------------------------------------------------------------------------
technique Bloom {
    pass P0 {        
        PixelShader  = compile ps_2_0 BloomPS();
    }
}

//-----------------------------------------------------------------------------
// Desc: Scale the source texture down to 1/16 scale
//-----------------------------------------------------------------------------
technique DownScale4x4 {
    pass P0 {
        PixelShader  = compile ps_2_0 DownScale4x4PS();
    }
}
//-----------------------------------------------------------------------------
// Desc: Scale the source texture down to 1/4 scale n Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------
technique DownScale4x4nBrightPassFilter {
    pass P0 {
        PixelShader  = compile ps_2_0 DownScale4x4nBrightPassFilterPS();
    }
}

//-----------------------------------------------------------------------------
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
technique GaussBlur5x5 {
    pass P0 {
        PixelShader  = compile ps_2_0 GaussBlur5x5PS();
    }
}

//-----------------------------------------------------------------------------
// Desc: Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------
technique BrightPassFilter {
    pass P0 {
        PixelShader  = compile ps_2_0 BrightPassFilterPS();
    }
}

//-----------------------------------------------------------------------------
// Desc: Minimally transform and texture the incoming geometry
//-----------------------------------------------------------------------------
technique FinalScenePass
{
    pass P0
    {
        PixelShader  = compile ps_2_0 FinalScenePassPS();
    }
}

//-----------------------------------------------------------------------------
// for Quick Gaussian Filtering
//-----------------------------------------------------------------------------
technique QuickGaussBlur
{
    pass P0
    {
        PixelShader  = compile ps_2_0 QuickGaussBlurVerticalPS();
    }
    pass P1
    {
        PixelShader  = compile ps_2_0 QuickGaussBlurHorizontalPS();
    }
}
