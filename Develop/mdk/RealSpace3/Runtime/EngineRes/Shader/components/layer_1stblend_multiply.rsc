interface()
{
	$name = layer_1stblend_multiply
	$define = layer_1stblend
}

float4 DiffuseLayer1stBlend(float4 color0, float4 color1)
{
	return color0 * color1;
}