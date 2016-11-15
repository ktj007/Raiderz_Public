interface()
{
	$name = dual_normalmap_overay
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


	//-------------------------------------------------------------
	//Apply 'Overlay' Blend Mode
	float4 vBlendedColor = 2.0f * vNormalColor1 * vNormalColor0;
	vBlendedColor = (2*(vNormalColor0+vNormalColor1)-1 -vBlendedColor) *(vNormalColor0>0.5);
	vBlendedColor = saturate(vBlendedColor);
	//-------------------------------------------------------------


	//Apply Layer Opacity
	float4 vNormalColor =  lerp(vNormalColor0, vBlendedColor, g_fNormalLayerOpacity.y);


	EXPORT(float4, E_vNormalColor, vNormalColor);
	EXPORT(float,  E_fNormalAlpha, vNormalColor.a);
}