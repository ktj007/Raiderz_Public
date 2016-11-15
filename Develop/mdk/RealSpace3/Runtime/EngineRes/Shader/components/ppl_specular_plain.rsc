interface()
{
	$name = ppl_specular_plain
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 0;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);

	EXPORT( float3,  E_vLitSpecular, vPixelSpecular);
}
