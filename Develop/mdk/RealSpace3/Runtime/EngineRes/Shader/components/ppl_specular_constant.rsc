interface()
{
	$name = ppl_specular_constant
	$define = ppl_specular_constant
}

shared float	g_fGlossiness;

void PSMAIN(inout PSOUTPUT output)
{
	EXPORT( float, E_fSpecularGlossiness, g_fGlossiness);
}
