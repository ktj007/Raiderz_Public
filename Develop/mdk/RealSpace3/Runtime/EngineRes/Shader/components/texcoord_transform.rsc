interface()
{
	$name = texcoord_transform
	$define = texcoord_export
}

struct VSINPUT
{
	float2 vDiffuseCoord	: TEXCOORD0;
};

shared matrix  	g_textureTransform;

void VSMAIN(VSINPUT input)
{
	float2 vTexcoord = mul(float4(input.vDiffuseCoord,0,1) ,g_textureTransform).xy;
	EXPORT(float2, E_vTexcoord, vTexcoord);
}