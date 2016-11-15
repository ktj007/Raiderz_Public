interface()
{
	$name = ppl_cos_view
	$define = ppl_cos_view
	$dependency = transform
}

void PSMAIN(inout PSOUTPUT output)
{
	float3 normal = 0;
	IMPORT( E_pplNormalInterpolated, normal = E_pplNormalInterpolated );

	float cosView = abs( dot( g_vEyeDir, normal ) );
	EXPORT( float, E_fCosView, cosView );
}

