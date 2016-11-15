interface()
{
	$name = g_depth
	$define = g_depth
	$dependency = transform
}

shared float g_fFarDist;

struct VSOUTPUT
{
	float3 vPosInView	: TEXCOORD%;
};


void VSMAIN(inout VSOUTPUT output)
{
  	float4 vPosInView =0;
  	IMPORT( E_vWorldViewPosition, vPosInView = E_vWorldViewPosition);
  	output.vPosInView = vPosInView.xyz;
  	
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	EXPORT( float, E_fLinearDepth, -input.vPosInView.z/g_fFarDist );
}