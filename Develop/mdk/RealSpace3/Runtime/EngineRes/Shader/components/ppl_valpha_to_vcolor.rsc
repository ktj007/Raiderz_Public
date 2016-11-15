interface()
{
	$name = ppl_valpha_to_vcolor
	$define = ppl_valpha_to_vcolor
	$dependency = vcolor
}

void PSMAIN(inout PSOUTPUT output)
{
	float fVAlpha;
	IMPORT( E_fVAlpha, fVAlpha = E_fVAlpha);
	output.color = output.color * fVAlpha;
}