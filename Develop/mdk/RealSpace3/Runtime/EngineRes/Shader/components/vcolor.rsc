interface()
{
	$name = vcolor
	$define = vcolor
}

struct VSINPUT
{
	float4 vDiffuse		: COLOR0;
};

struct VSOUTPUT
{
	float4 vDiffuse		: COLOR0;
};

void VSMAIN(VSINPUT input, inout VSOUTPUT output)
{
	output.vDiffuse = input.vDiffuse;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	EXPORT( float4, E_fVColor, input.vDiffuse);
	EXPORT( float, E_fVAlpha, input.vDiffuse.a);
	
	output.color  = output.color * input.vDiffuse ;
}