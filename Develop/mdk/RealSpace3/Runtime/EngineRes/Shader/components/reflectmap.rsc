interface()
{
	$name		= reflect_map
	$define 	= reflect_map
	$dependency	= texcoord
}

sampler sEnv_Map : register(s2);

shared int g_iUseEnvAlpha;


float fresnel(float3 eyevec, float3 normal, float power, float bias)
{
	normal = normalize(normal);
	eyevec = normalize(eyevec);
	
	float fresnel = saturate(abs(dot(normal,eyevec))); // 노말과 눈 벡터를 통해 모서리 구한다.
	//fresnel = 1 - fresnel; 								// 모서리 구한 값을 뒤집니다.
	fresnel = pow(fresnel, power);						// 모서리 부분이 날카로워지는 정도.
	fresnel += bias; 									// 어느정도로 번지게 만들것인가.
	
	return saturate(fresnel);
}

struct VSOUTPUT
{
	float4 vWorld_Position	: TEXCOORD%;
	float3 vViewWorld		: TEXCOORD%;
};


void VSMAIN(out VSOUTPUT output)
{	
	float4 vWorldPosition = 0;
	IMPORT(E_vWorldPosition, vWorldPosition = E_vWorldPosition);
	output.vWorld_Position = vWorldPosition;

	output.vViewWorld = vWorldPosition - g_vEyePosition;
}

void PSMAIN(in VSOUTPUT input, inout PSOUTPUT output )
{	
	// 일단 반사광 맵 알파 채널의 값으로 탁함을 표현한다.
	float2 vSpecularCoord = 0;
	IMPORT( E_vSpecularCoord, vSpecularCoord = E_vSpecularCoord);
	
	float fGlossiness = g_vSavedAlpha[ g_iUseEnvAlpha ];
	IMPORT( E_fEnvGlossiness, fGlossiness = E_fEnvGlossiness );
	
	// 노말맵 노말 가져온다.
	float3 vBumpNormal = 0;
	IMPORT( E_vNormalColor, vBumpNormal = E_vNormalColor.xyz);
	
	float3 vGetNormal = 0;
	IMPORT(E_pgNormal, vGetNormal = E_pgNormal );	
	vGetNormal = mul(vGetNormal, (float3x3)g_mViewInv).xyz;
		
	float3 I = input.vViewWorld;		// 뷰 공간에서의 정점 위치
	float3 R = reflect( I, vGetNormal );
	
	R.yz = R.zy;
	
	float4 returnColor = texCUBElod( sEnv_Map, float4(R, 6.01329f - (fGlossiness * 6.01329f)));	

	vGetNormal = 0;
	IMPORT(E_pgNormal, vGetNormal = E_pgNormal );
	float fFresnel = fresnel( I, vGetNormal, 2.26f, 0.55f);	// 4번쨰 인자. 원래 색이 나오는 비율, 작을 수록 넓게 나온다.
	
	EXPORT( float, E_vEnv_Fresnel, fFresnel );
	EXPORT( float4, E_vEnvironmentColor, returnColor );
	EXPORT( float,  E_fEnvironmentAlpha, returnColor.a );	// 알파를 굳이 해 줘야 하는가?
}
