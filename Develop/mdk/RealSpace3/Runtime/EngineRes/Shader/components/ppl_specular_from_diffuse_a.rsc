interface()
{
	$name = ppl_specular_diffuse_alpha
	$dependency = diffusemap
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 1;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);

	float fDiffuseAlpha = 0;
	IMPORT( E_fDiffuseAlpha, fDiffuseAlpha = E_fDiffuseAlpha);

	float3 vSpecular = vPixelSpecular * fDiffuseAlpha;
	EXPORT( float3,  E_vLitSpecular, vSpecular);
}
