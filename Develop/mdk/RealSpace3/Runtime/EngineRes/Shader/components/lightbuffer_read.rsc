interface()
{
	$name = lightbuffer_read
	$define = lightbuffer_read
	$define = ppl_specular
	$dependency = m_constant
}


sampler LightBufferMap : register(s8);


void PSMAIN( inout PSOUTPUT output)
{
	float4 vBufferCoord;
	IMPORT ( E_vBufferCoord, vBufferCoord = E_vBufferCoord);
	
	float4 fLighting = tex2Dproj( LightBufferMap, vBufferCoord);
	// 라이팅 값
	float3 vLitColor = fLighting.rgb;
	EXPORT(float3, E_vLitColor, vLitColor);
	// 쌩 스페큘라의 값도 여기서 걍 뽑아준다.
	// 위에 ppl_specular도 E_vPixelSpecular를 사용 하는 것들을 위해 선언 한것.
	EXPORT(float3, E_vPixelSpecular, fLighting.aaa);
}