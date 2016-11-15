interface()
{
	$name = texcoord_diffuse_mapchannel
	$dependency = texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vDiffuseTexcoord =0;
	IMPORT( E_vTexCoord2, vDiffuseTexcoord = E_vTexCoord2);

	EXPORT(float2, E_vDiffuseCoord, vDiffuseTexcoord);
}