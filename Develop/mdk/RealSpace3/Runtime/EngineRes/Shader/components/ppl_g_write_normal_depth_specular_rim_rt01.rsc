interface()
{
	$name = ppl_g_write_normal_depth_specular_rim_rt01
	$define = ppl_g_write_normal_depth_rt01
	$define = ppl_g_write_normal_depth
	$define = ppl_g_write_normal
	$dependency = g_depth
	$dependency = g_constant
}

void PSMAIN(inout PSOUTPUT output)
{
	// 노말라이즈는 쓰는 곳에서 수행해서 쓰도록 한다. ppl_g_write_no_lighting_normal에서 10,10을 기록할 것이므로 읽는 곳에서 노말라이즈 하면 안됨.
	float3 vNormal;
	IMPORT( E_pgNormal, vNormal = E_pgNormal);

	// 알파는 G패스 종속적이지 않은 컴포넌트 부분에서 채워진다. 맨 마지막에 이를 가져다 쓴다.
	// 조합 코드들이 color.a에 직접 지술하기도 하고 E_fAlpha에 기술 하기도 하는데 
	// E_fAlpha를 최종 반영을 하여야 정상적으로 된다.
	float fAlpha = 1;
	IMPORT( E_fAlpha, fAlpha = E_fAlpha);

	// 노말은 xy 두개의 값만 기록하고 사용 시 z는 추측해내서 쓴다.
	output.color = float4( vNormal.x, vNormal.y, 1, fAlpha);

	// 뎁스는 0~1. z/w. z가 필요시 알아서 뽑아내서 사용.
	// normal의 Z축 방향을 DepthBuffer에 같이 저장한다.
	int nZDirection = (sign(vNormal.z)>=0)*2-1;	// if(vNormal.z >= 0) nZDirection =1, else nZDirection = -1
	
	// Specular
	float3 vSpecularLevel = 0;
	IMPORT( E_vLitSpecular, vSpecularLevel = E_vLitSpecular);
	float fSpecularGlossiness = 0;
	IMPORT( E_fSpecularGlossiness, fSpecularGlossiness = E_fSpecularGlossiness);
	// 스페큘라는 (Glossiness).(Level)식으로 저장한다. 
	// SpecularLevel은 스페큘라 맵의 값이거나 1이므로 0~1 사이의 값이다. 1 대신 0.99값으로 대체하여 분해를 쉽게 하도록 하자. 0.001의 오차는 눈에 띄지 않을 것이다.
	float fSpecularCompress = max( fSpecularGlossiness, 0) + min( vSpecularLevel, 0.99f);
	
	// 스페큘라 정보의 부호를 림 마스크로 사용
	float fRimMask = -1;
	IMPORT( E_pgRimMask, fRimMask = E_pgRimMask);
	fSpecularCompress *= fRimMask;

	float fLinearDepth;
	IMPORT( E_fLinearDepth, fLinearDepth = E_fLinearDepth);
	output.color1 = float4( fLinearDepth *nZDirection, fSpecularCompress, 1, 1);
}
