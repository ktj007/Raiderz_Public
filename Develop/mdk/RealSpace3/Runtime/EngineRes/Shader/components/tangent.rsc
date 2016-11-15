interface()
{
	$name = tangent
	$define = tangent
}

struct VSINPUT
{
	float4 vTangent		: TEXCOORD2;
};

void VSMAIN(VSINPUT input)
{
	EXPORT(float4, E_vLocalTangent, input.vTangent);
}