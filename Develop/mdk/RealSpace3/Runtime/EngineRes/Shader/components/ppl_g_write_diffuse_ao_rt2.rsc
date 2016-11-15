interface()
{
	$name = ppl_g_write_diffuse_ao_rt2
	$define = ppl_g_write_color3
	$dependency = g_constant
}

void PSMAIN(inout PSOUTPUT output)
{
	float fAO = 1;	/// 1인 경우가 AO 처리 안하는 것
	IMPORT( E_fAO, fAO = E_fAO);
	
	// 아웃풋의 color를 그대로 기입. 
	// output.color를 그대로 사용 하고 다른 G버퍼들을 color 1,2,3에 기입하는 것이 보기 좋겠지만, color3를 diffuse, DSF, noInfo 등의 선택 사항으로 두어야 하기에...
	
	
	output.color2 = float4( output.color.r, output.color.g, output.color.b, fAO);
}
