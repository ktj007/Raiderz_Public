interface()
{
	$name = add_color
	$define = add_color
}

void PSMAIN(inout PSOUTPUT output)
{
	float3 vAddColor = 0;
	IMPORT( E_vAddColor, vAddColor += E_vAddColor );
	output.color.rgb += vAddColor;
}
