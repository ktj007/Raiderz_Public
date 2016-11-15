interface()
{
	$name = layer_2ndblend_colorburn
	$define = layer_2ndblend
}

float4 DiffuseLayer2ndBlend(float4 color0, float4 color1)
{
	return max( 1.0f - (1.0f - color0) / (color1+0.0001f), 0.0f );
}