interface()
{
	$name = ppl_clip_alpha
	$define = ppl_clip_alpha
}

shared float	g_fAlphaRef = 0;

void PSMAIN(inout PSOUTPUT output)
{
	if( output.color.a <= g_fAlphaRef )
		discard;
	// or
	// clip 은 arg < 0 인데 arg<=0이되려면... 어쩌나...
	// clip( output.color.a - g_fAlphaRef);
}
