interface()
{
	$name 		= ppl_self_illumination
	$dependency = selfilluminationmap
}

shared float g_vIlluminationScale;

void PSMAIN(inout PSOUTPUT output)
{
  float4 vIlluminationColor;
	IMPORT( E_vSelfIlluminationColor, vIlluminationColor = E_vSelfIlluminationColor);
	
	vIlluminationColor.rgb *= g_vIlluminationScale;
	EXPORT( float3, E_vAddColor, vIlluminationColor.rgb);
}
