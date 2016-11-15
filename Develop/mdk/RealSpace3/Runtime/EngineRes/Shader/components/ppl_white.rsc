interface()
{
	$name = ppl_white
	$define = lighting
}

void PSMAIN(inout PSOUTPUT output)
{
	output.color.rgb = 1;

	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);
	output.color.a = fAlpha;
}
