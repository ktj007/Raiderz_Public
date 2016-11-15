interface()
{
	$name = position
	$define = position
}

struct VSINPUT
{
	float4 vPosition	: POSITION;
};

void VSMAIN(VSINPUT input)
{
	EXPORT(float4, E_vLocalPosition, input.vPosition);
	
	// 090629, OZ
	EXPORT(float4, E_vLocalPositionPrev, input.vPosition);
}