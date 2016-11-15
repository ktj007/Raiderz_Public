interface()
{
	$name = shadow_write
	$define = transform
	$dependency = position
	$dependency = texcoord
}

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
};

void VSMAIN(inout VSOUTPUT output)
{
	float4 vLocalPosition = 0;
	IMPORT ( E_vLocalPosition, vLocalPosition = E_vLocalPosition);

	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);
}

sampler DiffuseMap : register(s0);

shared float g_fShadowValue;
	
void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	float2 vDiffuseCoord = 0;
	IMPORT( E_vDiffuseCoord, vDiffuseCoord = E_vDiffuseCoord);

	float a = tex2D( DiffuseMap, vDiffuseCoord ).a;

	output.color = float4(g_fShadowValue,g_fShadowValue,g_fShadowValue,a);
}
