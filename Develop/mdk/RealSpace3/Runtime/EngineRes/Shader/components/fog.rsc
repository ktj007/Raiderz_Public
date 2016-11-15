interface()
{
	$name = fog
	$define = fog
	$dependency = transform
}

struct VSOUTPUT
{
	float  fFog				: FOG;
};

shared float4		g_vFogFactor;   // x : near, y : far, z : dist , w : factor (multiply)

void VSMAIN(inout VSOUTPUT output)
{
	float fPositionZ = 0;
	IMPORT( E_vPositionZ, fPositionZ = E_vPositionZ);

    float fogNear = g_vFogFactor.x;
    float fogEnd = g_vFogFactor.y;
    float reciprocalfogDist = g_vFogFactor.z;
    float fFactor = g_vFogFactor.w;

    // 1이면 포그가 안먹는 상태.
    output.fFog = saturate((fogEnd - fPositionZ)*reciprocalfogDist) + fFactor;
}