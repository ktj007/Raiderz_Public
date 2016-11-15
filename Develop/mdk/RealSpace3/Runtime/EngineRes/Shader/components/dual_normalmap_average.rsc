interface()
{
	$name = dual_normalmap_average
	$define = normalmap
	$dependency = texcoord
}

sampler NormalMap : register(s4);
sampler SecondNormalMap : register(s14);

shared float2 g_fNormalLayerOpacity;

void PSMAIN( inout PSOUTPUT output)
{
	//-------------------------------------------------------------
	//Calculate Texture coord
	float4 vUV =0;
	IMPORT( E_vNormalCoord, vUV.xy = E_vNormalCoord);
	IMPORT( E_vTexCoord2, vUV.zw = E_vTexCoord2);

	float2 vTexcoord0 = GetTexcoordResult(vUV, 4);
	float2 vTexcoord1 = GetTexcoordResult(vUV, 14);
	//-------------------------------------------------------------


	//Read Normal Map
	float4 vNormalColor0 = tex2D( NormalMap, vTexcoord0) *g_fNormalLayerOpacity.x;
	float4 vNormalColor1 = tex2D( SecondNormalMap, vTexcoord1);


	//Apply 'Average' Blend Mode
	float4 vBlendedColor = (vNormalColor1 + vNormalColor0) *0.5f;


	//Apply Layer Opacity
	float4 vNormalColor =  lerp(vNormalColor0, vBlendedColor, g_fNormalLayerOpacity.y);


	EXPORT(float4, E_vNormalColor, vNormalColor);
	EXPORT(float,  E_fNormalAlpha, vNormalColor.a);
}