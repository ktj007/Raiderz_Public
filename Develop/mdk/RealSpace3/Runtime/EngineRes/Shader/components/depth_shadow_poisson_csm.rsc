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

void IsShadow_Poisson(float2 vecCoord, float d, out float2 outfShadow)
{
	float2 fTexOffset = {g_fDepthTexelSize, g_fDepthTexelSize/g_nSplitCount};

	outfShadow = 0.0f;
	
    for(int i = 0; i < POISSON_COUNT; ++i)
    {
		float2 shadowMapUV = vecCoord + g_fFilterSize * fTexOffset * poissonDisk[i];
	
		float2 fShadow =1;
		IsShadow( shadowMapUV , d, fShadow);
		
		outfShadow +=fShadow;
    }
    
	outfShadow /= POISSON_COUNT;
}

void PSMAIN(inout PSOUTPUT output)
{
	float d = 0;
	IMPORT ( E_fShadowCompareD, d = E_fShadowCompareD);

	float2 refUV = 0;
	IMPORT ( E_vShadowRefUV, refUV = E_vShadowRefUV);
	
	float4 vPosition_View = 0;
	IMPORT ( E_vViewPosition, vPosition_View = E_vViewPosition);
	
	int2 fSplitPosition =0;
	IMPORT( E_fCSM_SplitPosition, fSplitPosition = E_fCSM_SplitPosition);
	
	float2 fShadow = 1;

	if(fSplitPosition.x>=0)
		IsShadow_Poisson( refUV, d, fShadow );

	if(fSplitPosition.x < fSplitPosition.y)
	{
		float d2 = 0;
		IMPORT ( E_fShadowCompareD2, d2 = E_fShadowCompareD2);

		float2 refUV2 = 0;
		IMPORT ( E_vShadowRefUV2, refUV2 = E_vShadowRefUV2);

		float fCSM_LerpTerm =0;
		IMPORT( E_fCSM_LerpTerm, fCSM_LerpTerm = E_fCSM_LerpTerm);

		float2 fShadow2 =1;
		IsShadow_Poisson( refUV2, d2, fShadow2);

		fShadow = lerp(fShadow2, fShadow, fCSM_LerpTerm);
	}


// 	//------------------------------------------
// 	//Dedug øÎ: ∞¢ ±∏∞£¿ª ªˆ¿∏∑Œ ≥™≈∏≥ø.
// 	float3 DebugColor[8] = 
// 	{	1, 0, 0,		//ª°
// 		0, 1, 0,		//≥Ï
// 		0, 0, 1,		//∆ƒ
// 		1, 1, 0,		//≥Î
// 		1, 0, 1,		//¿⁄¡÷
// 		0, 1, 1,		//«œ¥√
// 		1, 1, 1,		//πÈ
// 		0, 0, 0,		//»Ê
// 	};
// 	float3 Result=0;
// 	if(fSplitPosition.x < fSplitPosition.y)
// 	{
// 		Result = DebugColor[fSplitPosition.x+4];
// 	}
// 	else
//  		Result = DebugColor[fSplitPosition.x];
// 
// 	//fShadow =Result;
// 	//------------------------------------------
				
	EXPORT( float3, E_vShadow, fShadow.x);
	EXPORT( float3, E_vDoubleShadow, fShadow.y);
}
