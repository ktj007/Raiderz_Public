interface()
{
	$name = cloudmap2
	$dependency = transform
}

struct VSOUTPUT
{
	float4 vCloudCoord	: TEXCOORD%;
};

shared matrix  	g_cloudTransform;
shared float	g_fCloudFactor;
shared matrix  	g_cloudTransform2;
shared float	g_fCloudFactor2;

sampler CloudMap : register(s3);


void VSMAIN( inout VSOUTPUT output)
{
	float4 vWorldPosition = 0;
	IMPORT ( E_vWorldPosition, vWorldPosition = E_vWorldPosition);

	output.vCloudCoord.xy	= mul(vWorldPosition ,g_cloudTransform).xy ;
	output.vCloudCoord.zw	= mul(vWorldPosition ,g_cloudTransform2).xy ;
}

void PSMAIN( VSOUTPUT input, inout PSOUTPUT output)
{
	// 계산 공식은 terrain.fx 에 맞춤
	float3 vCloudMapColor = tex2D( CloudMap, input.vCloudCoord.xy ).rgb;
	float3 vCloudColor = lerp( vCloudMapColor, float3(1,1,1), g_fCloudFactor );

	float3 vCloudMapColor2 = tex2D( CloudMap, input.vCloudCoord.zw ).rgb;
	float3 vCloudColor2 = lerp( vCloudMapColor2, float3(1,1,1), g_fCloudFactor2 );

	EXPORT( float3, E_vShadow, vCloudColor.rgb * vCloudColor2.rgb);
}