interface()
{
	$name = texcoord_normal_mapchannel
	$dependency = texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vSpecularCoord =0;
	IMPORT( E_vTexCoord2, vSpecularCoord = E_vTexCoord2);

	EXPORT(float2, E_vSpecularCoord, vSpecularCoord);
}