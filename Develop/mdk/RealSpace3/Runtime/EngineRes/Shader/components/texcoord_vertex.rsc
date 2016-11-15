interface()
{
	$name = texcoord_vertex
	$define = texcoord_export
}

struct VSINPUT
{
	float2 vTexCoord	: TEXCOORD0;
};

void VSMAIN(VSINPUT input)
{
	EXPORT(float2, E_vTexcoord, input.vTexCoord);
}