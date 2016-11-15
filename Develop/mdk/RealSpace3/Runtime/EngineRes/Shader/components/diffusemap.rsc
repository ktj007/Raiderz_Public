interface()
{
	$name = diffusemap
	$define = diffusemap
	$dependency = texcoord
}

sampler DiffuseMap : register(s0);

void PSMAIN( inout PSOUTPUT output)
{
	float2 vDiffuseCoord = 0;
	IMPORT( E_vDiffuseCoord, vDiffuseCoord = E_vDiffuseCoord);
	float4 vDiffuseColor = tex2D( DiffuseMap, vDiffuseCoord);
	
	g_vSavedAlpha[0] = vDiffuseColor.a;
	
	EXPORT(float4, E_vDiffuseColor, vDiffuseColor);
	EXPORT(float,  E_fDiffuseAlpha, vDiffuseColor.a);
	// alpha소스를 이곳에서도 익스포트 하지 말자. 
	// 알파 소스는 축적 되므로 여기서부터 축적되면 안됨.
	// EXPORT(float,  E_fAlpha, vDiffuseColor.a);
	
	// alpha 처리는 여기서 하지 않는다.
	vDiffuseColor.a = 1.0;
	output.color = vDiffuseColor;
}