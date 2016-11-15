interface()
{
	$name = factor
}

shared float4  	g_textureFactor;

void PSMAIN(inout PSOUTPUT output)
{
	output.color  = output.color * g_textureFactor ;
	
	// 팩터의 alpha를 반영하긴 하지만 바로 오퍼시티 아웃으로 사요 하지는 말자.
	// alpha 축적 얻음
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);
	// 소스 알파
	float fAlphaSrc = g_textureFactor.a;
	// alpha 최종 축적
	float fTargetAlpha = fAlpha * fAlphaSrc;
	EXPORT(float, E_fAlpha, fTargetAlpha);
	
}