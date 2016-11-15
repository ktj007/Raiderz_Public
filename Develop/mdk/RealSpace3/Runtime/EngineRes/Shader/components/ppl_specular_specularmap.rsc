interface()
{
	$name = ppl_specular_specularmap
	$dependency = ppl_specular
	$dependency = specularmap
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 0;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);

	float3 vSpecularColor = 0;
	IMPORT( E_vSpecularColor, vSpecularColor = E_vSpecularColor);

	vPixelSpecular *= vSpecularColor;

	output.color.rgb += vPixelSpecular;
}
