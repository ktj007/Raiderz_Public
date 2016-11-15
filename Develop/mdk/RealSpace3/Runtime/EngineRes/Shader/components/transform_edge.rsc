interface()
{
	$name		= transform_edge
	$define		= transform
	$dependency = transform_constant
	$dependency = normal
}

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
};

shared float2 g_fTickNess;

void VSMAIN(inout VSOUTPUT output)
{
	float3 vLocalNormal = 0;
	IMPORT ( E_vLocalNormal, vLocalNormal = E_vLocalNormal );
	vLocalNormal = mul( vLocalNormal, (float3x3)g_matWorld );
	
	float4 vLocalPosition = 0;
	IMPORT ( E_vLocalPosition, vLocalPosition = E_vLocalPosition);
	vLocalPosition = mul( vLocalPosition, g_matWorld );
	
	vLocalPosition.xy	+= (vLocalNormal *  g_fTickNess.x ).xy;
	vLocalPosition.z	+= (vLocalNormal *  g_fTickNess.y ).z;
	vLocalPosition.xyz	+= normalize( vLocalPosition.xyz - g_vEyePosition ) * 2.5f;
	
	output.vPosition = mul( vLocalPosition, g_matViewProj);
}