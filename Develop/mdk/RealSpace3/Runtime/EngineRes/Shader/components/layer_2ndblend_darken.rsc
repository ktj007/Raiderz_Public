interface()
{
	$name = layer_2ndblend_darken
	$define = layer_2ndblend
}

float4 DiffuseLayer2ndBlend(float4 color0, float4 color1)
{
	return min(color0, color1);
}