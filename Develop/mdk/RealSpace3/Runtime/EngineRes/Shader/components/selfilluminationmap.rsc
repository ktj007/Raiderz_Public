interface()
{
	$name = selfilluminationmap
	$define = selfilluminationmap
	$dependency = texcoord
}

sampler IlluminationMap : register(s5);

void PSMAIN( inout PSOUTPUT output)
{
	float2 vSelfIlluminationCoord = 0;
	IMPORT( E_vSelfIlluminationCoord, vSelfIlluminationCoord = E_vSelfIlluminationCoord);
	float4 vSelfIlluminationColor = tex2D( IlluminationMap, vSelfIlluminationCoord);

	EXPORT(float4, E_vSelfIlluminationColor, vSelfIlluminationColor);
	EXPORT(float,  E_fSelfIlluminationAlpha, vSelfIlluminationColor.a);
}