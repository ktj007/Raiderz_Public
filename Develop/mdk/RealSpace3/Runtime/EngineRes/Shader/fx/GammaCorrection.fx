
float3 EncodeGammaCorrection( in float3 _vColor )
{
#if USE_GAMMA_CORRECT == 1
	return pow( _vColor , 2.2f );
#else
	return _vColor;
#endif
}

float3 DecodeGammaCorrection( in float3 _vColor )
{
#if USE_GAMMA_CORRECT == 1
	return pow( _vColor , 1.f / 2.2f );
#else
	return _vColor;
#endif
}