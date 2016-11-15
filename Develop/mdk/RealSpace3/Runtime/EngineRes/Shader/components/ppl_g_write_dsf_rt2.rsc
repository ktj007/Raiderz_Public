interface()
{
	$name = ppl_g_write_dsf_rt3
	$define = ppl_g_write_color3
	$dependency = g_depth
	$dependency = g_constant
}

shared float g_fDSFID = 0;

void PSMAIN(inout PSOUTPUT output)
{
	// 알파는 G패스 종속적이지 않은 컴포넌트 부분에서 채워진다. 맨 마지막에 이를 가져다 쓴다.
	// 조합 코드들이 color.a에 직접 지술하기도 하고 E_fAlpha에 기술 하기도 하는데 
	// E_fAlpha를 최종 반영을 하여야 정상적으로 된다.
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);

	// DSFID는 그대로 출력
	output.color2 = float4( g_fDSFID, 1, 1, fAlpha);
}
