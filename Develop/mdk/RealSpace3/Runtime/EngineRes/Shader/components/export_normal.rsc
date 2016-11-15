interface()
{
	$name = export_normal
	$define = export_normal
	$dependency = transform
}

struct VSOUTPUT
{
	float3 vNormal		: TEXCOORD%;
};


void VSMAIN(inout VSOUTPUT output)
{
	float3 vWorldNormal = 0;
	IMPORT ( E_vWorldNormal, vWorldNormal = E_vWorldNormal);

	output.vNormal 		= vWorldNormal;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	float3 interpolatedNormal = normalize(input.vNormal);
	EXPORT( float3, E_pplNormal, interpolatedNormal);
	EXPORT( float3, E_pplNormalInterpolated, interpolatedNormal);
}
