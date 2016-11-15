interface()
{
	$name = depth
	$define = transform
	$dependency = position
	$dependency = texcoord
}

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
	float3 vPos : TEXCOORD%;
};

void VSMAIN(inout VSOUTPUT output)
{
	float4 vLocalPosition = 0;
	IMPORT ( E_vLocalPosition, vLocalPosition = E_vLocalPosition);

	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);
	output.vPos = mul(vLocalPosition, g_matWorldView).xyz;
}

sampler DiffuseMap : register(s0);
	
void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	float2 vDiffuseCoord = 0;
	IMPORT( E_vDiffuseCoord, vDiffuseCoord = E_vDiffuseCoord);

	float4 vDiffuseColor = tex2D( DiffuseMap, vDiffuseCoord );

	float d= input.vPos.z;
 	float a = vDiffuseColor.a;

	output.color = float4(d,d,d,a);
}
