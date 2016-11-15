interface()
{
	$name = instance_partscolor
	$define = instance_partscolor
	$dependency = diffusemap_partscolor_constant
	$dependency = instancing
}

struct VSOUTPUT
{
	float4 vPartsColor : COLOR3;
};

void VSMAIN(inout VSOUTPUT output)
{
	float4 vinstanceClr;
	IMPORT( E_vInstancePartsColor, vinstanceClr = E_vInstancePartsColor);
	output.vPartsColor = vinstanceClr;	
}


void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	g_vPartsColor = input.vPartsColor;
}

