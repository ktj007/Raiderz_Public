interface()
{
	$name = depth_shadow
	$define = depth_shadow
	$dependency = position
	$dependency = depth_read
}

float IsShadow_RAW(float4 vecCoord)
{
	float4 d = tex2Dproj(DepthMap, vecCoord);
	vecCoord.z  /= vecCoord.w;    
    float z = vecCoord.z;
    float c = (z <= d.r ? 1 : 0);
	return c;
}

void PSMAIN( inout PSOUTPUT output)
{
	float d = 0;
	IMPORT ( E_fShadowCompareD, d = E_fShadowCompareD);

	float2 refUV = 0;
	IMPORT ( E_vShadowRefUV, refUV = E_vShadowRefUV);
	
	float c = IsShadow( float4(refUV,0,1) ,  d );
	EXPORT( float3, E_vShadow, c);
}
