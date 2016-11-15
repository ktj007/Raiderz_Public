interface()
{
	$name = texcoord_world
	$define = texcoord_export
	$dependency = transform
}

shared matrix  	g_textureTransform;

void VSMAIN()
{
	float4 vWorldPosition = 0;
	IMPORT ( E_vWorldPosition, vWorldPosition = E_vWorldPosition);

	float2 vTexcoord = mul(vWorldPosition ,g_textureTransform).xy;
	EXPORT(float2, E_vTexcoord, vTexcoord);
}