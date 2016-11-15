interface()
{
	$name = ppl_glow
	$define = ppl_glow
	$dependency = ppl_cos_view
}

shared float4 g_vGlowColorAndDiffuseRate; // rgb: color, a:diffuse rate

void PSMAIN(inout PSOUTPUT output)
{
	float3 pplNormal = 0;
	IMPORT( E_pplNormal, pplNormal = E_pplNormal );

	float cosView = 0;
	IMPORT( E_fCosView, cosView = E_fCosView );

	float3 glowColor = pow( 1 - cosView, g_vGlowColorAndDiffuseRate.a ) * g_vGlowColorAndDiffuseRate.rgb;

	float3 nDotu = max( dot( pplNormal, float3( 0, 0, -1 ) ), 0 );
	glowColor -= glowColor * nDotu;

  EXPORT( float3, E_vAddColor, glowColor );
}

