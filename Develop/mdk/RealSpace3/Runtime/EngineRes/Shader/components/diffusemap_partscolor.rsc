interface()
{
	$name = diffusemap_partscolor
	$define = diffusemap_partscolor
	$dependency = diffusemap_partscolor_constant
	$dependency = diffusemap
}

void PSMAIN( inout PSOUTPUT output)
{
	float fDiffuseAlpha = 0;
	IMPORT ( E_fDiffuseAlpha, fDiffuseAlpha = E_fDiffuseAlpha);
	
	// 디퓨즈 알파값을 파츠 칼라 블렌딩 파라메터로 사용하므로 알파값은 리셋한다.
	float fAlpha = 1;
	EXPORT(float, E_fAlpha, fAlpha);

	// parts 컬러 조합 vDiffuseColor.rgb * vDiffuseColor.a + g_vPartsColor.rgb * ( 1 - vDiffuseColor.a )
	output.color.rgb = lerp( output.color.rgb, g_vPartsColor.rgb, 1 - fDiffuseAlpha );
}