interface()
{
	$name = ppl_view_direction
	$define = ppl_view_direction
	$dependency = transform
}

struct VSOUTPUT
{
	float3 vViewDir : TEXCOORD%;
};

void VSMAIN(inout VSOUTPUT output)
{
	float4 vWorldPosition = 0;
	IMPORT(E_vWorldPosition, vWorldPosition = E_vWorldPosition);

	float3 vEyePos = 0;
	IMPORT(E_vEyePosition, vEyePos = E_vEyePosition);

	output.vViewDir = vEyePos.xyz - vWorldPosition.xyz;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	float3 interpolatedViewDir = normalize(input.vViewDir);
	EXPORT(float3, E_pplViewDir, interpolatedViewDir);
}