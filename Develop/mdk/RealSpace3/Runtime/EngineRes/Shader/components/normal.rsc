interface()
{
	$name = normal
	$define = normal
}

struct VSINPUT
{
	float3 vNormal		: NORMAL;
};

void VSMAIN(VSINPUT input)
{
	EXPORT(float3, E_vLocalNormal,	input.vNormal);
}