interface()
{
	$name = ppl_alpha_from_diffuse_a
	$dependency = diffusemap
}

void PSMAIN( inout PSOUTPUT output)
{
	// alpha 축적 얻음
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);
	// 소스 알파
	float fAlphaSrc = 1;
	IMPORT( E_fDiffuseAlpha, fAlphaSrc = E_fDiffuseAlpha);
	// alpha 최종 축적
	float fTargetAlpha = fAlpha * fAlphaSrc;
	EXPORT(float, E_fAlpha, fTargetAlpha);
	
	output.color.a = fTargetAlpha;
}