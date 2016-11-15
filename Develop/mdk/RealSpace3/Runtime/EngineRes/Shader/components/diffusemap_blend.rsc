interface()
{
	$name = diffusemap_blend
	$define = diffusemap_blend
	$dependency = diffusemap
}

sampler BlendMap : register(s1);
shared float g_fBlendFactor;

void PSMAIN( inout PSOUTPUT output)
{
	float2 vDiffuseCoord = 0;
	IMPORT( E_vDiffuseCoord, vDiffuseCoord = E_vDiffuseCoord );

	float4 vBlendColor = tex2D( BlendMap, vDiffuseCoord );	// 이게 원본 재질이고 Facotr 0 -> 1 이다.

	float4 vDiffuseColor = 0;
	IMPORT( E_vDiffuseColor, vDiffuseColor = E_vDiffuseColor );
		
	//DstFactor( Factor ) * BLENDING_MATERIALMAP + SrcFactor( 1 - Factor ) * DIFFUSE
	vDiffuseColor.rgb = vDiffuseColor.rgb * g_fBlendFactor + vBlendColor.rgb * ( 1 - g_fBlendFactor );
	//vDiffuseColor.rgb = vBlendColor.rgb;// * ( 1 - g_fBlendFactor );
	
	EXPORT(float4, E_vDiffuseColor, vDiffuseColor);
	
	output.color.rgb = vDiffuseColor;
}