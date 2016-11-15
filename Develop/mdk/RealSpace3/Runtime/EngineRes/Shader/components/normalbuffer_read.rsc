interface()
{
	$name = normalbuffer_read
	$define = normalbuffer_read
	$dependency = m_constant
}


sampler NormalBufferMap : register(s7);


void PSMAIN( inout PSOUTPUT output)
{
	float4 vBufferCoord;
	IMPORT ( E_vBufferCoord, vBufferCoord = E_vBufferCoord);
	
	float4 fNormal = tex2Dproj( NormalBufferMap, vBufferCoord);
	EXPORT(float2, E_vScreenNormal, fNormal.xy);
}