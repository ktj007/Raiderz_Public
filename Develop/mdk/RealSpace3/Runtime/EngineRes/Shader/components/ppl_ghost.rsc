interface()
{
	$name = ppl_ghost
	$define = ppl_ghost
	$dependency = ppl_cos_view
}

void PSMAIN(inout PSOUTPUT output)
{
	float cosView = 0;
	IMPORT( E_fCosView, cosView = E_fCosView );

	float term = cosView * cosView;
	
	output.color.a *= saturate( ( 1 - cosView * 1 ) * 5.0 );

	


	output.color.rgb += ( 1 - cosView * cosView ) * output.color.rgb;
}