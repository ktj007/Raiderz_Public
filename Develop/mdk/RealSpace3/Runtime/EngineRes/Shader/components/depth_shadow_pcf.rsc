interface()
{
	$name = depth_shadow_pcf
	$define = depth_shadow
	$dependency = position
	$dependency = depth_read
}

shared float		g_fDepthTexelSize;

///////////////////////////////////////////////////////////////////////////////
// 어디서 줏어온 PCF 필터 수정 버젼
float IsShadow_PCF(float4 vecCoord, float d)
{
    float fFilterSize = 4.0;
    float fStartOffset = ( fFilterSize - 1.0f ) / 2.0f ;
    float fTexOffset = g_fDepthTexelSize;

	vecCoord.xyz  /= vecCoord.w; 
	vecCoord.w = 1;
	
	float fragDepth = vecCoord.z;
//    fragDepth -= 0.01;
	
    vecCoord.xy -= fStartOffset * fTexOffset;
    float lit = 0.0f;
    for(int i = 0; i < fFilterSize; ++i)
    {
        for(int j = 0; j < fFilterSize; ++j)
        {
			lit += IsShadow( vecCoord + float4(i*fTexOffset, j*fTexOffset,0,0) , d);
        }
    }
    return lit / (fFilterSize*fFilterSize);
}

void PSMAIN( inout PSOUTPUT output)
{
	float d = 0;
	IMPORT ( E_fShadowCompareD, d = E_fShadowCompareD);

	float2 refUV = 0;
	IMPORT ( E_vShadowRefUV, refUV = E_vShadowRefUV);

	float c = IsShadow_PCF( float4(refUV,0,1), d );
	EXPORT( float3, E_vShadow, c);
}
