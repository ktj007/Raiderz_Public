interface()
{
	$name = instance_ao
	$define = instance_ao
	$dependency = instancing
}

struct VSOUTPUT
{
	float fAO : TEXCOORD%;
};

void VSMAIN(inout VSOUTPUT output)
{
	float fAO;
	IMPORT( E_fInstanceAO, fAO = E_fInstanceAO);
	output.fAO = fAO;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	EXPORT( float, E_fAO, input.fAO);
}



