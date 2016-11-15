interface()
{
	$name = texcoord_opacity_mapchannel
	$dependency = texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vOpacityCoord=0;
	IMPORT( E_vTexCoord2, vOpacityCoord = E_vTexCoord2);

	EXPORT(float2, E_vOpacityCoord, vOpacityCoord);
}