interface()
{
	$name = ppl_specular_normal_alpha
	$dependency = normalmap
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 1;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);

	float fNormalAlpha = 0;
	IMPORT( E_fNormalAlpha, fNormalAlpha = E_fNormalAlpha);

	float3 vSpecular = vPixelSpecular * fNormalAlpha;
	EXPORT( float3,  E_vLitSpecular, vSpecular);
}
