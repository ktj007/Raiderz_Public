interface()
{
	$name		= texcoord_env_mapchannel
	$dependency	= texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vEnvironmentCoord = 0;
	IMPORT( E_vTexCoord2, vEnvironmentCoord = E_vTexCoord2);

	EXPORT( float2, E_vEnvironmentCoord, vEnvironmentCoord );
}