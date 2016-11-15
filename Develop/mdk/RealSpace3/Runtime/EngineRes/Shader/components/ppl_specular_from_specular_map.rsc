interface()
{
	$name = ppl_specular_specular_map
	$dependency = specularmap
}

void PSMAIN( inout PSOUTPUT output)
{
	float3 vPixelSpecular = 1;
	IMPORT( E_vPixelSpecular, vPixelSpecular = E_vPixelSpecular);

	float3 vSpecularColor = 0;
	IMPORT( E_vSpecularColor, vSpecularColor = E_vSpecularColor);

	vPixelSpecular *= vSpecularColor;

	// 모노크롬 화상으로 그레이 스케일링 Directx 9
	const float3 RGB2Y = {0.299f, 0.587f, 0.114f};
	float fPixelSpecular = dot( vPixelSpecular, RGB2Y);
	vPixelSpecular = float3( fPixelSpecular, fPixelSpecular, fPixelSpecular);

	EXPORT( float3,  E_vLitSpecular, vPixelSpecular);
}