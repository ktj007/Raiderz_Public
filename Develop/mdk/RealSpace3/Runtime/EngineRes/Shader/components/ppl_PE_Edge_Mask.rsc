interface()
{
	$name = ppl_Edge_Mask
}

shared float3 g_vPEEdgeColor;

void PSMAIN(inout PSOUTPUT output)
{
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);

	if( fAlpha <= 0.5f )
		discard;
	
	output.color.rgb = g_vPEEdgeColor;
	output.color.a = fAlpha;
}
