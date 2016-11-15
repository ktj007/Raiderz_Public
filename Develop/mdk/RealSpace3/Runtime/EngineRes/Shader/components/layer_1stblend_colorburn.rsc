interface()
{
	$name = layer_1stblend_colorburn
	$define = layer_1stblend
}

float4 DiffuseLayer1stBlend(float4 color0, float4 color1)
{
	return max( 1.0f - (1.0f - color0) / (color1+0.0001f), 0.0f );
}