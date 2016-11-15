interface()
{
	$name = texcoord2
	$define = texcoord2
}

struct VSINPUT
{
	float2 vTexCoord	    : TEXCOORD1;
};

struct VSOUTPUT
{
	float2 vTexCoord	    : TEXCOORD%;
};

void VSMAIN(VSINPUT input, inout VSOUTPUT output)
{
	output.vTexCoord = input.vTexCoord;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	EXPORT(float2, E_vTexCoord2, input.vTexCoord);
}