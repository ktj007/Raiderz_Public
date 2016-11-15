interface()
{
	$name = diffusemaplayer2
	$define = diffusemaplayer2
	$dependency = texcoord
}

sampler DiffuseMapLayer0 : register(s0);
sampler DiffuseMapMask0 : register(s2);

sampler DiffuseMapLayer1 : register(s10);
sampler DiffuseMapMask1 : register(s11);


shared float3 g_vDiffuseLayerOpacity;
shared float3 g_bUseMaskMap;


float4 ReadLayerMap(int iSampler, sampler Sampler)
{
	float4 vUV =0;
	IMPORT( E_vDiffuseCoord, vUV.xy = E_vDiffuseCoord);
	IMPORT( E_vTexCoord2, vUV.zw = E_vTexCoord2);

	float2 vTexcoord = GetTexcoordResult(vUV, iSampler);
	
	return tex2D( Sampler, vTexcoord );
}

float4 ApplyBlendedLayer(float4 vPrevColor, float4 vBlendedColor, float4 vMask, float fOpacity)
{
	return lerp(vPrevColor, vBlendedColor, fOpacity*vMask);
}

void PSMAIN( inout PSOUTPUT output)
{
	float4 vLayer0Color = ReadLayerMap(0, DiffuseMapLayer0) * g_vDiffuseLayerOpacity.x;
	if(g_bUseMaskMap.x)
		vLayer0Color *= ReadLayerMap(3, DiffuseMapMask0);

	float4 vLayer1Color = ReadLayerMap(10, DiffuseMapLayer1);
	float4 vLayer1Mask = g_bUseMaskMap.y ? ReadLayerMap(11, DiffuseMapMask1) : 1;

	float4 vBlendedColor= DiffuseLayer1stBlend(vLayer0Color, vLayer1Color);

	EXPORT(float4, E_vDiffuseColor, ApplyBlendedLayer(vLayer0Color, vBlendedColor, vLayer1Mask, g_vDiffuseLayerOpacity.y*vLayer1Color.a));	
}