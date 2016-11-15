interface()
{
	$name = instancing
	$define = instancing
	$dependency = transform_constant
}

static const int MAX_INSTANCE = 20;	// RMeshNode.h의 것과 맞춰 줄 것.
shared float4x4 g_InstancingMatrices[MAX_INSTANCE];	

struct VSINPUT
{
	float vInstancingIdx	: TEXCOORD3;
};

void VSMAIN(VSINPUT input)
{
	float4x4 matInstanceWorld = g_InstancingMatrices[input.vInstancingIdx];
	float4 vinstanceClr = float4( matInstanceWorld[0].w, matInstanceWorld[1].w, matInstanceWorld[2].w, matInstanceWorld[3].w);
	float fAO = matInstanceWorld[3].w;
	matInstanceWorld[0].w = 0;
	matInstanceWorld[1].w = 0;
	matInstanceWorld[2].w = 0;
	matInstanceWorld[3].w = 1;
	
	g_matWorld = matInstanceWorld;
	g_matWorldView		= mul( g_matWorld, g_matView);
	g_matWorldViewProj	= mul( g_matWorld, g_matViewProj);
	
	EXPORT( float4, E_vInstancePartsColor, vinstanceClr);
	EXPORT( float,E_fInstanceAO,fAO);
}