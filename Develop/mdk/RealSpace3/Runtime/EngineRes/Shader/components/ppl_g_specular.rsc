interface()
{
	$name = ppl_g_specular
	$define = ppl_g_specular
	$dependency = g_constant
}

shared float	g_fGlossiness;

void PSMAIN(inout PSOUTPUT output)
{
	float3 vLitSpecular = 0;
	IMPORT( E_vLitSpecular, vLitSpecular = E_vLitSpecular);

	// specular from map에서 이미 모노크롬 화상으로 변환 해서 뱉어주므로 값을 택일 할 필요 없음.
	//float fPixelSpecular = max( max( vLitSpecular.r, vLitSpecular.g), vLitSpecular.b);
	float fPixelSpecular = vLitSpecular.r;
	
	EXPORT( float2, E_pgSpecular, float2( fPixelSpecular, g_fGlossiness));
}
