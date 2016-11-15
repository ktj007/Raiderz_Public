interface()
{
	$name = texcoord
	$define = texcoord
	$dependency = texcoord_export
}

shared float4	g_UVTransform[16];
shared int		g_MapChannel[16];
shared float4	g_vSavedAlpha;


float2 GetTexcoordResult(in float4 vUV, in int nSampler)
{
	//vUV.xy => Mapchannel :0, vUV.zw => Mapchannel :1

	float2 vTexcoord = 0;
	if(g_MapChannel[nSampler]==0)
		vTexcoord = vUV.xy;
	else
		vTexcoord = vUV.zw;

	vTexcoord = vTexcoord *g_UVTransform[nSampler].zw +g_UVTransform[nSampler].xy;

	return vTexcoord;
}

struct VSOUTPUT
{
	float2 vTexCoord	: TEXCOORD%;
};

void VSMAIN(out VSOUTPUT output)
{
	float2 vTexcoord =0;
	IMPORT(E_vTexcoord, vTexcoord = E_vTexcoord);
	output.vTexCoord = vTexcoord;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	EXPORT(float2, E_vDiffuseCoord, input.vTexCoord);
	EXPORT(float2, E_vOpacityCoord, input.vTexCoord);
	EXPORT(float2, E_vNormalCoord, input.vTexCoord);
	EXPORT(float2, E_vSpecularCoord, input.vTexCoord);
	EXPORT(float2, E_vSelfIlluminationCoord, input.vTexCoord);	
	EXPORT(float2, E_vRenderTargetCoord, input.vTexCoord);
}