interface()
{
	$name = add_specific_color
	$define = add_specific_color
}

shared float3 g_vAddColor;

void PSMAIN(inout PSOUTPUT output)
{
  EXPORT( float3, E_vAddColor, g_vAddColor );
}

