interface()
{
	$name = position_pointlight
	$define = position
}

struct VSINPUT
{
	float4 vPosition	: POSITION;

	float3 vLightPos : NORMAL;
	float3 vLightInfo : TEXCOORD0;		//x: Radius, y: Attenuation Start, z: Visibility

	float4 vAxisX : TEXCOORD1;
	float4 vAxisY : TEXCOORD2;
	float4 vAxisZ : TEXCOORD3;
};

void VSMAIN(VSINPUT input)
{

	float3 vRadius =input.vLightInfo.x;
	float3 vLightAreaRange = { input.vAxisX.w, input.vAxisY.w, input.vAxisZ.w };
	
	float3 vPosition = input.vPosition.xyz * (vRadius+vLightAreaRange*0.5f);

	float4 vLocalPosition =1;
	vLocalPosition.x = dot(vPosition, input.vAxisX);
	vLocalPosition.y = dot(vPosition, input.vAxisY);
	vLocalPosition.z = dot(vPosition, input.vAxisZ);
	vLocalPosition.xyz += input.vLightPos;


	EXPORT(float4, E_vLocalPosition, vLocalPosition);

	EXPORT(float3, E_vLightAreaRange, vLightAreaRange);
	EXPORT(float3, E_vLightAxisX, input.vAxisX.xyz);
	EXPORT(float3, E_vLightAxisY, input.vAxisY.xyz);
	EXPORT(float3, E_vLightAxisZ, input.vAxisZ.xyz);
}