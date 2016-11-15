interface()
{
	$name = m_constant
	$define = m_constant
	$dependency = transform
}

struct VSOUTPUT
{
	float4 vBufferCoord	: TEXCOORD%;
};


shared matrix  	g_ScreenSpaceTransformMStage;


void VSMAIN(inout VSOUTPUT output)
{
	float4 vWorldPosition = 0;
	IMPORT ( E_vWorldPosition, vWorldPosition = E_vWorldPosition);

	output.vBufferCoord = mul(vWorldPosition ,g_ScreenSpaceTransformMStage);
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	EXPORT (float4 , E_vBufferCoord, input.vBufferCoord);	
}


