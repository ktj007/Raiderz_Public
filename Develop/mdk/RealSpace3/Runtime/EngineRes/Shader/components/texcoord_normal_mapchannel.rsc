interface()
{
	$name = texcoord_normal_mapchannel
	$dependency = texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vNormalTexcoord =0;
	IMPORT( E_vTexCoord2, vNormalTexcoord = E_vTexCoord2);

	EXPORT(float2, E_vNormalCoord, vNormalTexcoord);
}