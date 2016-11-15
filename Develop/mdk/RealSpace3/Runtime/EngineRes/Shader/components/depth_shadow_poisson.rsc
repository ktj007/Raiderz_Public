interface()
{
	$name = depth_shadow_poisson
	$define = depth_shadow
	$dependency = position
	$dependency = depth_read
}

shared float		g_fDepthTexelSize;
shared float		g_fFilterSize = 1.5;

///////////////////////////////////////////////////////////////////////////////
// poisson « ≈Õ
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
const float2 poissonDisk[POISSON_COUNT] = {
   -0.326212, -0.405805,
   -0.840144, -0.073580,
   -0.695914,  0.457137,
   -0.203345,  0.620716,
    0.962340, -0.194983,
    0.473434, -0.480026,
    0.519456,  0.767022,
    0.185461, -0.893124,
    0.507431,  0.064425,
    0.896420,  0.412458,
   -0.321940, -0.932615,
   -0.791559, -0.597705,
};

float IsShadow_Poisson(float4 vecCoord, float d)
{
    float fTexOffset = g_fDepthTexelSize;

    float lit = 0.0f;
    for(int i = 0; i < POISSON_COUNT; ++i)
    {
		float4 shadowMapUV = float4(vecCoord.xy + g_fFilterSize * fTexOffset * poissonDisk[i], 0, 1 );
		lit += IsShadow( shadowMapUV , d );
    }
    return lit / (POISSON_COUNT);
}

void PSMAIN(inout PSOUTPUT output)
{
	float d = 0;
	IMPORT ( E_fShadowCompareD, d = E_fShadowCompareD);

	float2 refUV = 0;
	IMPORT ( E_vShadowRefUV, refUV = E_vShadowRefUV);

	float c = IsShadow_Poisson( float4(refUV,0,1), d );
	EXPORT( float3, E_vShadow, c);
}
