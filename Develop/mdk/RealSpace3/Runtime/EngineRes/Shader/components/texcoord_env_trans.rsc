interface()
{
	$name		= texcoord_env_trans
	$dependency	= texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vEnvironmentCoord = 0;
	IMPORT( E_vEnvironmentCoord, vEnvironmentCoord = E_vEnvironmentCoord );
	vEnvironmentCoord = vEnvironmentCoord * g_UVTransform[2].zw + g_UVTransform[2].xy;

	EXPORT( float2, E_vEnvironmentCoord, vEnvironmentCoord );
}