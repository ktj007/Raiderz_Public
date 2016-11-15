interface()
{
	$name = mask_AO
	$define = mask_AO
}


void PSMAIN(inout PSOUTPUT output)
{
	// 0인 경우가 AO작동
	EXPORT( float, E_fAO, 0);
}
