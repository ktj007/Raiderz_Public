interface()
{
	$name = layer_2ndblend_average
	$define = layer_2ndblend
}

float4 DiffuseLayer2ndBlend(float4 color0, float4 color1)
{
	return (color0 + color1)*0.5f;
}