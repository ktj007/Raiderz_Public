interface()
{
	$name = depth_shadow_pcss
	$define = depth_shadow
	$dependency = position
	$dependency = depth_read
}

#define BLOCKER_SEARCH_NUM_SAMPLES 16
#define PCF_NUM_SAMPLES 16
// #define NEAR_PLANE 1
#define POISSON_COUNT 16
#define LIGHT_DISTANCE 	3000

shared float2 g_lightSizeUV;
shared float g_lightFrustumDepth;

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

float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation 
{ 
//    return (zReceiver - zBlocker) / zBlocker; 
	return (zReceiver - zBlocker) / ( LIGHT_DISTANCE / g_lightFrustumDepth);
} 
 
void FindBlocker(
                out float avgBlockerDepth,  
                out float numBlockers,
                sampler DepthMapSampler,
				float2 uv, float zReceiver )
{
    //This uses similar triangles to compute what  
    //area of the shadow map we should search 
    float searchWidth = g_lightSizeUV.x; // * (zReceiver - NEAR_PLANE) / zReceiver;
	float searchHeight = g_lightSizeUV.y; // * (zReceiver - NEAR_PLANE) / zReceiver;
 
    float blockerSum = 0; 
    numBlockers = 0; 
 
    for( int i = 0; i < POISSON_COUNT ; ++i ) 
    { 
		float2 uvStep = float2(poissonDisk[i].x * searchWidth, poissonDisk[i].y * searchHeight);
		float shadowMapDepth = tex2D(DepthMapSampler, uv + uvStep).x;

        if ( shadowMapDepth < zReceiver ) { 
                blockerSum += shadowMapDepth; 
                numBlockers++; 
            } 
     } 
 
    avgBlockerDepth = blockerSum / numBlockers; 
} 
 
float PCF_Filter(sampler DepthMapSampler, float2 uv, float zReceiver, float2 filterRadiusUV ) 
{ 
	// /**REGULAR_PCF_FILTER */
    //float stepSize = 2 * filterRadiusUV / PCF_NUM_SAMPLES; 
    //uv -= float2(filterRadiusUV, filterRadiusUV); 
    //float sum = 0; 
    //for (int i=0; i<PCF_NUM_SAMPLES; i++) { 
    //for (int j=0; j<PCF_NUM_SAMPLES; j++) { 
    //       float2 offset = float2(i*stepSize,j*stepSize); 
		  // sum += tex2D(DepthMapSampler, uv + offset).x > zReceiver;
    //    } 
    //} 
    //return sum / (PCF_NUM_SAMPLES*PCF_NUM_SAMPLES);
	float sum = 0.0f;
	for ( int i = 0; i < POISSON_COUNT; ++i )
	{
		float2 offset = poissonDisk[i] * filterRadiusUV;
//		sum += tex2D(DepthMapSampler, uv + offset).x > zReceiver;
		sum += IsShadow( float4(uv + offset,0,1) , zReceiver);
	}
	return sum / POISSON_COUNT;
} 

float PCSS ( sampler DepthMapSampler, float4 coords, float d  ) 
{ 
	coords /= coords.w;
	 /** ONLY_PCF_FILTER */
 //   float2 uv = coords.xy; 
 //   float zReceiver = coords.z; 
 //
	//// Asuming orthogonal projection for shadow map 
 //   float2 filterRadiusUV = 0.03f * g_lightSizeUV;
 //
 //   // STEP 3: filtering 
	//float shadow = PCF_Filter(DepthMapSampler, uv, zReceiver, filterRadiusUV );
	//return shadow;

	 /** PCSS_FILTER */
    float2 uv = coords.xy; 
//    float zReceiver = coords.z; 
	float zReceiver = d;
 
    // STEP 1: blocker search 
    float avgBlockerDepth = 0; 
    float numBlockers = 0; 
    FindBlocker( avgBlockerDepth, numBlockers, DepthMapSampler, uv, zReceiver ); 
 
    // STEP 2: penumbra size 
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth); 
 
	// Asuming orthogonal projection for shadow map 
    float2 filterRadiusUV = penumbraRatio * g_lightSizeUV;
 
    // STEP 3: filtering 
	float shadow = PCF_Filter(DepthMapSampler, uv, zReceiver, filterRadiusUV );
	if ( numBlockers < 1 )
		shadow = 1;

	return shadow;
}

void PSMAIN( inout PSOUTPUT output)
{
	float d = 0;
	IMPORT ( E_fShadowCompareD, d = E_fShadowCompareD);

	float2 refUV = 0;
	IMPORT ( E_vShadowRefUV, refUV = E_vShadowRefUV);

    // DepthMap = R32F
    float c = PCSS(DepthMap, float4(refUV,0,1) , d);
	EXPORT( float3, E_vShadow, c);
}
