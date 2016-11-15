interface()
{
	$name = pplvblur
	$define = pplvblur
	$dependency = transform_vblur
}


struct VSOUTPUT
{
	float4 vAlphaForce	: COLOR2;
};

void VSMAIN(inout VSOUTPUT output)
{
	float fAlphaForce;
	IMPORT( E_fTailAlpha, fAlphaForce = E_fTailAlpha);
	output.vAlphaForce.a = fAlphaForce;
	
	
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	output.color.a = output.color.a * input.vAlphaForce.a ;		// 원래 알파가 셋팅 되어 있는 것을 고려하여 기존 알파 값을 조건 채용  - 090722, OZ
}
