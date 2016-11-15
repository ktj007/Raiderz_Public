interface()
{
	$name = layer_1stblend_darken
	$define = layer_1stblend
}

float4 DiffuseLayer1stBlend(float4 color0, float4 color1)
{
	return min(color0, color1);
}