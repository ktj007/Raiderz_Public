interface()
{
	$name = physique_qt
	$define = physique
	$dependency = position
}

struct VSINPUT
{
	float2 vWeight		: BLENDWEIGHT;			
	float4 vIndex		: BLENDINDICES;	
};

static const int MAX_MATRICES = 54;	// RActorNodePhysique의 MAX_BONE_COUNT는 54로 되어있음 - 090720, OZ

shared float4 g_blendQuats[MAX_MATRICES];
shared float4 g_blendTrans[MAX_MATRICES];
shared float g_blendScale = 1;


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
	
	float4 Index = input.vIndex;

	for( int i=0;i<3;i++)
	{
		float4x4 mBlend = 0;
		float4 quat =  g_blendQuats[Index[i]];
		float3 tran =  g_blendTrans[Index[i]].xyz;
		float scale =  g_blendTrans[Index[i]].w;
		
		// 회전 적용 - MQuat::Conv2Matrix MQuat::SetMatrixRotation기능 구현
		float _xx, _yy, _zz, _xy, _xz, _yz, _wx, _wy, _wz;
		_xx = 2.0f * quat.x * quat.x;
		_yy = 2.0f * quat.y * quat.y;
		_zz = 2.0f * quat.z * quat.z;
		_xy = 2.0f * quat.x * quat.y;
		_xz = 2.0f * quat.x * quat.z;
		_yz = 2.0f * quat.y * quat.z;
		_wx = 2.0f * quat.w * quat.x;
		_wy = 2.0f * quat.w * quat.y;
		_wz = 2.0f * quat.w * quat.z;
		mBlend[0].x = 1.0f - (_yy + _zz);	// mBlend._11
		mBlend[1].x = (_xy - _wz);			// mBlend._21
		mBlend[2].x = (_xz + _wy);			// mBlend._31
		mBlend[0].y = (_xy + _wz);			// mBlend._12
		mBlend[1].y = 1.0f - (_xx + _zz);	// mBlend._22
		mBlend[2].y = (_yz - _wx);			// mBlend._32
		mBlend[0].z = (_xz - _wy);			// mBlend._13
		mBlend[1].z = (_yz + _wx);			// mBlend._23
		mBlend[2].z = 1.0f - (_xx + _yy);	// mBlend._33
		mBlend[0].w = 0.0f;					// mBlend._14
		mBlend[1].w = 0.0f;					// mBlend._24
		mBlend[2].w = 0.0f;					// mBlend._34
		// 스케일링 적용
		mBlend[0] *= scale;
		mBlend[1] *= scale;
		mBlend[2] *= scale;
		// 이동 적용
		mBlend[3].x = tran.x;				// mBlend._41
		mBlend[3].y = tran.y;				// mBlend._42
		mBlend[3].z = tran.z;				// mBlend._43
		mBlend[3].w = 1.0f;					// mBlend._44

		
		// 로컬로 레지스터좀 사용하자 - 090629, OZ
		float4 vP;
		IMPORT(E_vLocalPosition, vP = E_vLocalPosition);
		
		vPosition     += mul( vP, mBlend) * Weight[i];
		vPositionPrev += mul( vP, mBlend) * Weight[i];
		
		IMPORT(E_vLocalNormal, vNormal += mul(E_vLocalNormal,(float3x3)mBlend)*Weight[i]);	
		IMPORT(E_vLocalTangent, vTangent.xyz += mul(E_vLocalTangent.xyz,(float3x3)mBlend)*Weight[i]);
		IMPORT(E_vLocalTangent, vTangent.w = E_vLocalTangent.w);
	}
	
	EXPORT(float4, E_vLocalPosition, vPosition);
	EXPORT(float4, E_vLocalPositionPrev, vPositionPrev);
	
	EXPORT(float3, E_vLocalNormal, vNormal);
	EXPORT(float4, E_vLocalTangent, vTangent);
}