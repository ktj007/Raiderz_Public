interface()
{
	$name = ppl_specular_opacity_a
	$dependency = opacitymap
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 1;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);
	// OpacityAlpha는 opacity맵에서 읽은 alpha 채널
	float fOpacityAlpha = 0;
	IMPORT( E_fOpacityAlpha, fOpacityAlpha = E_fOpacityAlpha);

	float3 vSpecular = vPixelSpecular * fOpacityAlpha;
	EXPORT( float3,  E_vLitSpecular, vSpecular);
}
