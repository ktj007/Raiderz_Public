interface()
{
	$name = ppl_m_cos_view
	$define = ppl_cos_view
	$dependency = lightbuffer_read
}

void PSMAIN(inout PSOUTPUT output)
{
	float2 vScreenNormal = 0;
	IMPORT( E_vScreenNormal, vScreenNormal = E_vScreenNormal );
	
	// 스크린 스페이스 노말이므로 x,y값이 작으면 그만큼 Z방향의 노말에 가깝다는 뜻이 됨.
	// 반대로 z,y가 클수록 외곽선을 뜻함
	// 1보다 큰 값이 써져 있어서는 절대 안됨
	float cosView = 1 - max( abs(vScreenNormal.x), abs(vScreenNormal.y));
	EXPORT( float, E_fCosView, cosView );
}

