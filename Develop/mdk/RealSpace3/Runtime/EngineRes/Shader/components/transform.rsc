interface()
{
	$name = transform
	$define = transform
	$dependency = transform_constant
}

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
};

void VSMAIN(inout VSOUTPUT output)
{
	float4 vLocalPosition = 0;
	IMPORT ( E_vLocalPosition, vLocalPosition = E_vLocalPosition);
	
	float4 vWorldPosition = mul(vLocalPosition, g_matWorld);
	EXPORT (float4 , E_vWorldPosition , vWorldPosition );
	
	float4 vWorldViewPosition = mul(vLocalPosition, g_matWorldView);
	EXPORT (float4 , E_vWorldViewPosition , vWorldViewPosition );
	
	float3 vLocalNormal = float3(0,0,1);
	IMPORT ( E_vLocalNormal, vLocalNormal = E_vLocalNormal);
	
	float3 vWorldNormal = normalize(mul(vLocalNormal, (float3x3)g_matWorld));
	EXPORT (float3, E_vWorldNormal, vWorldNormal);
	
	float3 vWorldViewNormal = normalize(mul(vLocalNormal, (float3x3)g_matWorldView));
	EXPORT (float3, E_vWorldViewNormal, vWorldViewNormal);

	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);
	EXPORT (float, E_vPositionZ, output.vPosition.z);
	EXPORT (float, E_vPositionW, output.vPosition.w);	// use 4 BLUR - 090618, OZ
	
	EXPORT (float3, E_vEyePosition, g_vEyePosition);
}