interface()
{
	$name = opacitymap
	$define = opacitymap
	$dependency = texcoord
}

sampler OpacityMap : register(s7);

void PSMAIN( inout PSOUTPUT output)
{
	float2 vOpacityCoord = 0;
	IMPORT( E_vOpacityCoord, vOpacityCoord = E_vOpacityCoord);
	float4 vOpacity   = tex2D( OpacityMap, vOpacityCoord );
	
	g_vSavedAlpha[3] = vOpacity.a;

	EXPORT(float3, E_vOpacityColor, vOpacity.rgb);
	EXPORT(float, E_fOpacityAlpha, vOpacity.a);
}