interface()
{
	$name = decalmap
	$dependency = texcoord
}

sampler DecalMap : register(s0);

shared float4  	g_textureFactor;
shared float4	g_decalCenterpos;	// w는 depth

struct VSOUTPUT
{
	float4 vWorldPosition	: TEXCOORD%;
};

void VSMAIN( inout VSOUTPUT output)
{
	float4 vWorldPosition = 0;
	IMPORT ( E_vWorldPosition, vWorldPosition = E_vWorldPosition);

	output.vWorldPosition = vWorldPosition;
}

void PSMAIN( VSOUTPUT input, inout PSOUTPUT output)
{
	float2 vDecalCoord = 0;
	IMPORT(E_vDiffuseCoord, vDecalCoord = E_vDiffuseCoord);
	vDecalCoord.y = 1 - vDecalCoord.y;
	float4 vDecalMapColor = tex2D( DecalMap, vDecalCoord );

	output.color = vDecalMapColor;
	
	// 경계 Alpha
	float fAlpha = 1;
	float fSubValue = g_decalCenterpos.w - abs( input.vWorldPosition.z - g_decalCenterpos.z ); 
	fAlpha = fSubValue / g_decalCenterpos.w + 0.75;	// 경계 25% 에서 부터 알파 처리 시작 
	fAlpha = saturate(fAlpha);
	output.color.a *= fAlpha;
		
	output.color *= g_textureFactor;
}