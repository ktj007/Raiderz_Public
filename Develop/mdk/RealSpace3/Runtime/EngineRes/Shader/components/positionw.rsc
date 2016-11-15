interface()
{
	$name = position
	$define = position
}

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
};

void VSMAIN(inout VSOUTPUT output)
{
	output.vPosition =output.vPosition;
}