interface()
{
	$name = ppl_EdgeColor
	
}

shared float3 g_vEdgeColor;

void PSMAIN(inout PSOUTPUT output)
{
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);
	output.color.a = fAlpha;

	output.color.rgb = g_vEdgeColor;
}
