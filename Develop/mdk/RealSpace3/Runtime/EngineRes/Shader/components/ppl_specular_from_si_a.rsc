interface()
{
	$name = ppl_specular_si_alpha
	$dependency = selfilluminationmap
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 1;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);

	float fSelfilluminationAlpha = 0;
	IMPORT( E_fSelfilluminationAlpha, fSelfilluminationAlpha = E_fSelfilluminationAlpha);

	float3 vSpecular = vPixelSpecular * fSelfilluminationAlpha;
	EXPORT( float3,  E_vLitSpecular, vSpecular);
}
