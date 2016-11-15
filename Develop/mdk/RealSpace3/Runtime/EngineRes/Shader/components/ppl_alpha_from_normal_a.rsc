interface()
{
	$name = ppl_alpha_from_normal_a
	$dependency = normalmap
}

void PSMAIN( inout PSOUTPUT output)
{
	/// alpha 축적 얻음
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);
	/// 소스 알파
	float fAlphaSrc = 1;
	IMPORT( E_fNormalAlpha, fAlphaSrc = E_fNormalAlpha);
	/// alpha 최종 축적
	float fTargetAlpha = fAlpha * fAlphaSrc;
	EXPORT(float, E_fAlpha, fTargetAlpha);
	
	output.color.a = fTargetAlpha;
}