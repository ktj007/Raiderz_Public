interface()
{
	$name = ppl_lighting
	$define = lighting
}

void PSMAIN(inout PSOUTPUT output)
{
	float3 vLitColor = 0;
	IMPORT( E_vLitColor, vLitColor += E_vLitColor );
	float3 vLitSpecular = 0;
	IMPORT( E_vLitSpecular, vLitSpecular = E_vLitSpecular);
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);
	
	output.color.rgb  = (output.color.rgb + vLitSpecular) * vLitColor.rgb;
	output.color.a = fAlpha;
}

