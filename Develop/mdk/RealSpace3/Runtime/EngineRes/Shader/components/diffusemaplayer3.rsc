interface()
{
	$name = diffusemaplayer3
	$define = diffusemaplayer3
	$dependency = texcoord
	$dependency = diffusemaplayer2
	$dependency = layer_2ndblend
}

sampler DiffuseMapLayer2 : register(s12);
sampler DiffuseMapMask2 : register(s13);

void PSMAIN( inout PSOUTPUT output)
{
	float4 vPrevColor = 0;
 	IMPORT( E_vDiffuseColor, vPrevColor = E_vDiffuseColor);

	float4 vLayer2Color = ReadLayerMap(12, DiffuseMapLayer2);
	float4 vLayer2Mask = g_bUseMaskMap.z ? ReadLayerMap(13, DiffuseMapMask2) : 1;

	float4 vBlendedColor= DiffuseLayer2ndBlend(vPrevColor, vLayer2Color);

	EXPORT(float4, E_vDiffuseColor, ApplyBlendedLayer(vPrevColor, vBlendedColor, vLayer2Mask, g_vDiffuseLayerOpacity.z*vLayer2Color.a));
}