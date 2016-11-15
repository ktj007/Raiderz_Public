interface()
{
	$name = fog_pixel
	$define = fog
	$dependency = transform
}

struct VSOUTPUT
{
	float  fFog				: FOG;
};

shared float4		g_vFogFactor;   // x : near, y : far, z : dist , w : factor (multiply)
shared float3		g_vFogColor;

void VSMAIN(inout VSOUTPUT output)
{
	float fPositionZ = 0;
	IMPORT( E_vPositionZ, fPositionZ = E_vPositionZ);

    float fogNear = g_vFogFactor.x;
    float fogEnd = g_vFogFactor.y;
    float reciprocalfogDist = g_vFogFactor.z;
    float fFactor = g_vFogFactor.w;

	// 0:포그없음 1:포그가득
    output.fFog = saturate( (fPositionZ - fogNear)*reciprocalfogDist) + fFactor;
}

void PSMAIN(VSOUTPUT input, inout PSOUTPUT output)
{
	output.color.rgb  = lerp( output.color.rgb, g_vFogColor, input.fFog);
}