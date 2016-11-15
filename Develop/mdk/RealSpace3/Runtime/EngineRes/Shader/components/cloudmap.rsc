interface()
{
	$name = cloudmap
	$dependency = transform
}

struct VSOUTPUT
{
	float2 vCloudCoord	: TEXCOORD%;
};

shared matrix  	g_cloudTransform;
shared float	g_fCloudFactor;

sampler CloudMap : register(s3);

void VSMAIN( inout VSOUTPUT output)
{
	float4 vWorldPosition = 0;
	IMPORT ( E_vWorldPosition, vWorldPosition = E_vWorldPosition);

	output.vCloudCoord	= mul(vWorldPosition ,g_cloudTransform).xy ;
}

void PSMAIN( VSOUTPUT input, inout PSOUTPUT output)
{
	float3 vCloudMapColor = tex2D( CloudMap, input.vCloudCoord ).rgb;
	float3 vCloudColor = lerp( vCloudMapColor, float3(1,1,1), g_fCloudFactor );

	EXPORT( float3, E_vShadow, vCloudColor.rgb);
}