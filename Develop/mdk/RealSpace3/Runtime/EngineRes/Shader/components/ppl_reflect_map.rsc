interface()
{
	$name 		= ppl_reflect_map
	$define 	= ppl_reflect_map
}


void PSMAIN(inout PSOUTPUT output)
{
  	float4 vEnvMapColor = 0.f;
	IMPORT( E_vEnvironmentColor, vEnvMapColor = E_vEnvironmentColor );
	
	float3 vSpecColor = 0.f;
	IMPORT( E_vSpecularColor, vSpecColor = E_vSpecularColor);
	
	float fGlossiness = g_vSavedAlpha[ g_iUseEnvAlpha ];
		
	float fSpecGlossiness = 0.f;
	IMPORT( E_fSpecularGlossiness, fSpecGlossiness = E_fSpecularGlossiness);	
	
	//fSpecGlossiness *= 0.5f * fGlossiness + 0.5f;
	
	float fFresnel = 0.f;
	IMPORT( E_vEnv_Fresnel, fFresnel = E_vEnv_Fresnel );

	EXPORT( float, E_fSpecularGlossiness, fSpecGlossiness);
		
	float fFr = vSpecColor.r * fFresnel;		
	output.color.rgb = lerp( output.color.rgb, vEnvMapColor.rgb, fFr );
}