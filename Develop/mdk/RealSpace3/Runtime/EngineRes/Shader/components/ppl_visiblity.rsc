interface()
{
	$name = ppl_visiblity
	$define = ppl_visiblity
}

shared float  g_fVisiblity;

void PSMAIN(inout PSOUTPUT output)
{
	output.color.a = output.color.a * g_fVisiblity;
}

