interface()
{
	$name = transform_vblur
	$define = transform
	$define = transform_vblur
	$dependency = transform_constant
}

struct VSOUTPUT
{
	float4 vPosition	: POSITION;
};

shared matrix	g_matWorldPrev;
shared float	g_fMotionBlurFactorFront = 0;


void VSMAIN(inout VSOUTPUT output)
{
	// 로컬상의 현재 프레임 위치
	float4 vLocalPosition = 0;
	IMPORT ( E_vLocalPosition, vLocalPosition = E_vLocalPosition);
	// 월드상으로
	float4 vWorldPosition = mul(vLocalPosition, g_matWorld);
	EXPORT (float4 , E_vWorldPosition , vWorldPosition );
	
	// 로컬상의 이전 프레임 위치
	IMPORT ( E_vLocalPositionPrev, vLocalPosition = E_vLocalPositionPrev);
	// 월드상으로
	float4 vWorldPositionPrev = mul(vLocalPosition, g_matWorldPrev);
	
	float3 vLocalNormal = float3(0,0,1);
	IMPORT ( E_vLocalNormal, vLocalNormal = E_vLocalNormal);
	
	float3 vWorldNormal = normalize(mul(vLocalNormal, (float3x3)g_matWorld));
	EXPORT (float3, E_vWorldNormal, vWorldNormal);

	float4 vLocalTangent = 0;
	IMPORT ( E_vLocalTangent, vLocalTangent = E_vLocalTangent);

	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);
	EXPORT (float, E_vPositionZ, output.vPosition.z);
	EXPORT (float, E_vPositionW, output.vPosition.w);	// use 4 BLUR - 090618, OZ
	
	EXPORT (float3, E_vEyePosition, g_vEyePosition);
	
	float4 vVelocity = vWorldPosition - vWorldPositionPrev;
	
	float fDot = dot( vWorldNormal, normalize(vVelocity.xyz));
	bool bFront = ( 0 < fDot);
	
	output.vPosition = bFront ? vWorldPosition : vWorldPositionPrev;
	output.vPosition = mul(output.vPosition, g_matViewProj);
	EXPORT (float , E_fVelocityFactor , ( bFront ? g_fMotionBlurFactorFront : 1) );
	
	float fAlpha = bFront ? 1 : 0;
	
	EXPORT (float, E_fTailAlpha, fAlpha);
}