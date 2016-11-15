interface()
{
	$name = texcoord_si_mapchannel
	$dependency = texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vSelfIlluminationCoord=0;
	IMPORT( E_vTexCoord2, vSelfIlluminationCoord = E_vTexCoord2);

	EXPORT(float2, E_vSelfIlluminationCoord, vSelfIlluminationCoord);
}