interface()
{
	$name = texcoord_normal_trans
	$dependency = texcoord
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vNormalTexcoord = 0;
	IMPORT( E_vNormalCoord, vNormalTexcoord = E_vNormalCoord);
	vNormalTexcoord = vNormalTexcoord *g_UVTransform[4].zw +g_UVTransform[4].xy;

	EXPORT(float2, E_vNormalCoord, vNormalTexcoord);
}