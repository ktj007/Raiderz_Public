interface()
{
	$name = physique_dq
	$define = physique
	$dependency = position
}

struct VSINPUT
{
	float2 vWeight		: BLENDWEIGHT;
	float4 vIndex		: BLENDINDICES;	
};

static const int MAX_MATRICES = 55;

//shared float4x4 g_blendPrevMatrices[MAX_MATRICES];		// 이전 프레임의 값. 환장하게쎄연. - 090629, OZ
//shared float4x4 g_blendMatrices[MAX_MATRICES];	

// 더블 쿼터니언 사용으로 - 090701, OZ
shared float2x4 g_blendDQs[MAX_MATRICES];


void VSMAIN(VSINPUT input)
{
	float4 vPosition = (float4)0;
	float4 vPositionPrev = (float4)0;	// 090629, OZ
	float3 vNormal = 0;
	float4 vTangent = 0;
	float3 Weight;
	
	Weight[0] = input.vWeight[0];
	Weight[1] = input.vWeight[1];
	Weight[2] = 1.0f-(input.vWeight[0]+input.vWeight[1]);
	
	float3 Index = input.vIndex;
		
	float2x4 blendDQ = Weight[0] * g_blendDQs[ Index[0]];
	blendDQ += Weight[1] * g_blendDQs[ Index[1]];
	blendDQ += Weight[2] * g_blendDQs[ Index[2]];
	
	float len = length( blendDQ[0]);
	blendDQ /= len;

	// 포지션 ========================================================================================
	float4 vP = 0;
	IMPORT(E_vLocalPosition, vP = E_vLocalPosition);
	float3 position = vP.xyz + 2.0f * cross( blendDQ[0].yzw, cross( blendDQ[0].yzw, vP.xyz) + blendDQ[0].x * vP.xyz);
	float3 trans = 2.0f * ( blendDQ[0].x * blendDQ[1].yzw - blendDQ[1].x * blendDQ[0].yzw + cross( blendDQ[0].yzw, blendDQ[1].yzw));
	vPosition = float4( position + trans, 1);
	
	//TESTOZ
	vPosition = float4( position, 1);
	
	// 노말 ========================================================================================
	float3 vN = 0;
	IMPORT(E_vLocalNormal, vN = E_vLocalNormal);
	vNormal = vN + 2.0f * cross( blendDQ[0].yzw, cross( blendDQ[0].yzw, vN) + blendDQ[0].x * vN);
	
	// 노말 ========================================================================================
	IMPORT(E_vLocalTangent, vTangent = E_vLocalTangent);
	vTangent.xyz = vTangent.xyz + 2.0f * cross( blendDQ[0].yzw, cross( blendDQ[0].yzw, vTangent.xyz) + blendDQ[0].x * vTangent.xyz);
		
	EXPORT(float4, E_vLocalPosition, vPosition);
	//EXPORT(float4, E_vLocalPositionPrev, vPositionPrev);	// 090629, OZ	TESTOZ
	// 아래로 하면 잘 된다.
	// g_blendPrevMatrices로 나온 값을 사용 하면 이상한건가 ㅠㅠ
	EXPORT(float4, E_vLocalPositionPrev, vPosition);	// 090629, OZ	TESTOZ
	EXPORT(float3, E_vLocalNormal, vNormal);
	EXPORT(float4, E_vLocalTangent, vTangent);
}