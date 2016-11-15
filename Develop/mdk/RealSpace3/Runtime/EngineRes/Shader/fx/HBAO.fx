#line 1 "HBAO.fx"


//#define DEPTH_STAGE		0
//#define NORMAL_STAGE	1
//#define NOISE_STAGE		2
//#define MASK_STAGE		3


sampler DepthSampler	: register(s5);
sampler NormalSampler	: register(s4);
sampler MaskSampler		: register(s0);

sampler RandomSampler	: register(s2);
sampler AOSampler		: register(s0);
sampler ShadowSampler	: register(s1);



#define	M_PI			3.14159265f

float	g_R;			// = 50.0f;
float	g_inv_R;		// = 1.0f / 50.0f;			// 1 / g_R
float	g_sqr_R;		// = 2500.0f;				// g_R * g_R
float	g_AngleBias;	// = 0.52f;			// 30 * PI / 180
float	g_TanAngleBias;	// = 0.009f;		// tan(g_AngleBias)
float	g_Attenuation;	// = 0.98f;
float	g_Contrast;		// = 3.00f;
float	g_BrightBias;	// = 0.85f;

//int		NUM_SAMPLE = 6;


float2 g_FocalLen;
float2 g_InvFocalLen;
float2 g_InvResolution;
float2 g_Resolution;

//Screen-Quad의 View-공간 정점 위치 정보
float3 g_vFarClipCornerInView;

float g_FarZ;


//----------------------------------------------------------------------------------
float4 finalColor( in float fValue)  
{  
	float sample = fValue;
		
	return float4( sample, sample, sample, 1);
}  

//----------------------------------------------------------------------------------
float3 fetch_eye_nor( float2 uv)
{
	float4 uv4 = float4( uv, 0,0);
	float3 vNorm = tex2Dlod( NormalSampler, uv4).xyz;
	//float3 vNorm = tex2Dlod( NormalSampler, uv).xyz;
	// 원래는 노말의 Dir을 적용 하는 것이 정확히 맞는 것인데 무조건 -로 두는 것이 시각적으로 좋다. 왤까 대체 ㅠㅠ
	// http://dev:8181/projects/engine/ticket/1209#comment:13
	//int nZDirection = sign( tex2D( DepthSampler, uv).x);
	//vNorm.z = sqrt(1.0f - vNorm.x*vNorm.x - vNorm.y*vNorm.y) * nZDirection;
	vNorm.z = -sqrt(1.0f - vNorm.x*vNorm.x - vNorm.y*vNorm.y);
	return vNorm;
}

//----------------------------------------------------------------------------------
float tan_to_sin(float x)
{
    return x / sqrt(1.0f + x*x);
}


//----------------------------------------------------------------------------------
float tangent(float3 P, float3 S)
{
    return (P.z - S.z) / length(S.xy - P.xy);
}


//----------------------------------------------------------------------------------
float4 ReconstructPosFromDepth(float2 vScreencood, float fDepth)
{
	float3 vRay;
	vRay.xy = lerp(-g_vFarClipCornerInView.xy, g_vFarClipCornerInView.xy, vScreencood);
	vRay.z = g_vFarClipCornerInView.z;
	return float4(vRay*fDepth, 1);
}


//----------------------------------------------------------------------------------
float3 fetch_eye_pos(float2 uv)
{
    // 샘플링 할 점의 깊이를 얻음. 
	// fDepthValue : Linear
	//float fDepthValue = abs(tex2D( DepthSampler, uv).r);
	float4 uv4 = float4( uv, 0,0);
	float fDepthValue = abs(tex2Dlod( DepthSampler, uv4).r);

	float3 viewPos = ReconstructPosFromDepth(uv, fDepthValue).xyz;
	viewPos.z = -viewPos.z;

	return viewPos;
}

//----------------------------------------------------------------------------------
float3 tangent_eye_pos(float2 uv, float4 tangentPlane)
{
    // view vector going through the surface point at uv
    float3 V = fetch_eye_pos(uv);
    float NdotV = dot(tangentPlane.xyz, V);
    // intersect with tangent plane except for silhouette edges
    if (NdotV < 0.0) V *= (tangentPlane.w / NdotV);
    return V;
}

float length2(float3 v) { return dot(v, v); } 

//----------------------------------------------------------------------------------
float3 min_diff(float3 P, float3 Pr, float3 Pl)
{
    float3 V1 = Pr - P;
    float3 V2 = P - Pl;
    return (length2(V1) < length2(V2)) ? V1 : V2;
}

//----------------------------------------------------------------------------------
float falloff(float r)
{
    return 1.0f - g_Attenuation*r*r;
}

//----------------------------------------------------------------------------------
float2 snap_uv_offset(float2 uv)
{
    //return round(uv * g_Resolution) * g_InvResolution;
	return uv;
}

float2 snap_uv_coord(float2 uv)
{
    //return uv - (frac(uv * g_Resolution) - 0.5f) * g_InvResolution;
	return uv;
}

//----------------------------------------------------------------------------------
float3 tangent_vector(float2 deltaUV, float3 dPdu, float3 dPdv)
{
    return deltaUV.x * dPdu + deltaUV.y * dPdv;
}

//----------------------------------------------------------------------------------
float tangent(float3 T)
{
    return -T.z / length(T.xy);
}

//----------------------------------------------------------------------------------
float biased_tangent(float3 T)
{
    float phi = atan(tangent(T)) + g_AngleBias;
    return tan(min(phi, M_PI*0.5f));
}


//----------------------------------------------------------------------------------
float2 rotate_direction(float2 Dir, float2 CosSin)
{
    return float2(Dir.x*CosSin.x - Dir.y*CosSin.y, 
                  Dir.x*CosSin.y + Dir.y*CosSin.x);
}



float4 PSmainLowQuality( in float2 screenTC : TEXCOORD0 ) : COLOR
{
	// 0일 시만 AO 처리
	[branch]
	float fMaskP = tex2D( MaskSampler, screenTC).a;
	if( fMaskP != 0)
		return float4( 1, 1, 1, 1);

#ifdef SHADOWONLY
	// 라이트가 밝은 부분이면 역시 AO 스킵
	float fShadow = tex2D( ShadowSampler, screenTC).b;
	[branch]
	if( fShadow >= 1)
		return float4( 1, 1, 1, 1);
#endif // SHADOWONLY
	
	// 현재 픽셀의 뎁스 얻어옴. 실단위
	float3 P = fetch_eye_pos( screenTC);

	float fFactor = 1;
  	// 포그 구하는 공식으로 일정 이상 거리에서는 AO를 처리 안하도록 하자
  	const float fFalloffFar		= 1500.f;
  	const float fFalloffNear	= 1200.f;
  	const float fReciprocalDist	= 1.0f / ( fFalloffFar - fFalloffNear);
  	/// 1: 림가득 0: 림없음
 	fFactor = 1 - saturate( ((P.z) - fFalloffNear) * fReciprocalDist);
 	[branch]
 	if( fFactor <= 0)
 		return 1;
      
	float2 step_size = 0.5f * g_R  * g_FocalLen / P.z;
	
	// Nearest neighbor pixels on the tangent plane
	float3 Pr, Pl, Pt, Pb;
	float3 N = fetch_eye_nor( screenTC);
	float4 tangentPlane = float4(N, dot(P, N));
	Pr = tangent_eye_pos( screenTC + float2(g_InvResolution.x, 0), tangentPlane);
	Pl = tangent_eye_pos( screenTC + float2(-g_InvResolution.x, 0), tangentPlane);
	Pt = tangent_eye_pos( screenTC + float2(0, g_InvResolution.y), tangentPlane);
	Pb = tangent_eye_pos( screenTC + float2(0, -g_InvResolution.y), tangentPlane);

	// Screen-aligned basis for the tangent plane
	float3 dPdu = min_diff(P, Pr, Pl);
	float3 dPdv = min_diff(P, Pt, Pb) * (g_Resolution.y * g_InvResolution.x);
		
	float resultAO = 0;
	float numSam = 0.00001f;

	float4 rotationTC = (float4)0;
#ifdef RANDOMSPRAY
	rotationTC.xy = screenTC * g_Resolution / 8.0f;
	float fRnd = tex2Dlod( RandomSampler, rotationTC).r;	
	float theta = (rotationTC.x * rotationTC.y * fRnd) * M_PI * 2;
#else
	rotationTC.xy = screenTC * g_Resolution / 4.0f;
	float fRnd = tex2Dlod( RandomSampler, rotationTC).r;	
	float theta = fRnd * M_PI * 2;
#endif
	float2 rand = float2( cos(theta), sin(theta));
	float alpha = 2.0f * M_PI / NUM_SAMPLE;

	// 심플 영역과 Accessibility 누적
	for( int i=0; i<NUM_SAMPLE*2; i++)
	{
		int d = i / 2;

		float angle = alpha * d;
		float2 dir = float2( cos(angle), sin(angle));
		float2 vRotatedKernel = rotate_direction(dir, rand.xy) * step_size.xy *  ((float)d)/NUM_SAMPLE;

		[branch]
		if( length(vRotatedKernel.xy) <= length(g_InvResolution))
			continue;
		
		// 0,1 -> -1 , 1
		vRotatedKernel = vRotatedKernel * sign(((i%2) * 2) -1);

		float4 vSamTC;
		vSamTC.xy = vRotatedKernel.xy + screenTC.xy;
		vSamTC.zw = 0;

		// 이때 AO샘플러는 마스크 샘플러임.
		//float fMaskS = tex2D( MaskSampler, vSamTC).a;
		float fMaskS = tex2Dlod( MaskSampler, vSamTC).a;
		[branch]
 		if( fMaskS != 0)
 			continue;

		float3 S = fetch_eye_pos( vSamTC.xy);
	
		// Compute tangent vector using the tangent plane
		float3 T = vRotatedKernel.x * dPdu + vRotatedKernel.y * dPdv;

		float tanH = tangent(T) + g_TanAngleBias;
		float sinH = tanH / sqrt(1.0f + tanH*tanH);

		// Ignore any samples outside the radius of influence
		float d2  = length2(S - P);
		if (d2 < g_sqr_R) {
			float tanS = tangent(P, S);
			[branch]
			if(tanS > tanH) {
				// Accumulate AO between the horizon and the sample
				float sinS = tanS / sqrt(1.0f + tanS*tanS);
				float r = sqrt(d2) * g_inv_R;
				resultAO += (falloff(r) * (sinS - sinH)) * fFactor;
			}
		} 

		numSam += 1;
	}

	resultAO = saturate( 1.0 - (resultAO / numSam * g_Contrast));

	return float4( resultAO, resultAO, resultAO, 1);
}



float4 PSmainHighQuality( in float2 screenTC : TEXCOORD0 ) : COLOR
{
	
	
	g_AngleBias = 30 * M_PI / 180;
	g_TanAngleBias = tan(g_AngleBias);


	// 현재 픽셀의 뎁스 얻어옴. 실단위
	float3 P = fetch_eye_pos( screenTC);

	float2 step_size = 0.5f * g_R  * g_FocalLen / P.z;
	// 로테이션 벡터 얻음. 
	float4 rotationTC;
	rotationTC.xy = screenTC * 18.0f;
	rotationTC.zw = 0;
	//rotationTC = screenTC * g_Resolution / 4.0f;
	float3 vRnd = tex2Dlod( RandomSampler, rotationTC).rgb;
	float2 vRotation = normalize(2 * vRnd.xy - 1);
	

	// 너무 작은 샘플링 범위면 아예 하지 말지어다.
	float fFactor = step_size.x / g_InvResolution.x;
	if( fFactor < 2.0f)
		return 1;
	// 일정 범위 이하면 fadeout되게
	fFactor = saturate( fFactor - 2.5f);
//	float fFactor = 1;

		

	float3 N = fetch_eye_nor( screenTC);

	// Nearest neighbor pixels on the tangent plane
	float3 Pr, Pl, Pt, Pb;
	float4 tangentPlane = float4(N, dot(P, N));
	Pr = tangent_eye_pos( screenTC + float2(g_InvResolution.x, 0), tangentPlane);
	Pl = tangent_eye_pos( screenTC + float2(-g_InvResolution.x, 0), tangentPlane);
	Pt = tangent_eye_pos( screenTC + float2(0, g_InvResolution.y), tangentPlane);
	Pb = tangent_eye_pos( screenTC + float2(0, -g_InvResolution.y), tangentPlane);

	// Screen-aligned basis for the tangent plane
	float3 dPdu = min_diff(P, Pr, Pl);
	float3 dPdv = min_diff(P, Pt, Pb) * (g_Resolution.y * g_InvResolution.x);

	
		
	float resultAO = 0;
	float h0 = 0;



	float theta = vRotation.x * M_PI * 3;


	rotationTC.xy = screenTC * g_Resolution / 4.0f;
	theta = (rotationTC.x * rotationTC.y) * M_PI * 2;
	//theta = 0;


	float3 rand = float3( cos(theta), sin(theta), vRnd.y);
	float alpha = 2.0f * M_PI / 8;


	// 심플 영역과 Accessibility 누적
	int numSam=0;
	for( int i=0; i<16; i++)
	{
	
		int d = i / 2;

		float angle = alpha * d;
		float2 dir = float2( cos(angle), sin(angle));
		float2 deltaUV = rotate_direction(dir, rand.xy) * step_size.xy * ((float)d)/8.0f;

		if( length(deltaUV.xy) <= length(g_InvResolution))
		continue;

		if( i % 2)
			deltaUV = -deltaUV;


		// Jitter starting point within the first sample distance
		float2 uv = screenTC + deltaUV;		




		// Snap first sample uv and initialize horizon tangent
		float2 snapped_duv = snap_uv_offset(uv - screenTC);
		float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
		float tanH = tangent(T) + g_TanAngleBias;

		float2 snapped_uv = snap_uv_coord(uv);
		float3 S = fetch_eye_pos(snapped_uv);
		uv += deltaUV;


		// Ignore any samples outside the radius of influence
		float d2 = length2(S - P);
		if (d2 < g_sqr_R) {
			float tanS = tangent(P, S);

			[branch]
			if (tanS > tanH) {
				// Compute tangent vector associated with snapped_uv
				float2 snapped_duv = snapped_uv - screenTC;
				float3 T = tangent_vector(snapped_duv, dPdu, dPdv);
				float tanT = tangent(T) + g_TanAngleBias;

				// Compute AO between tangent T and sample S
				float sinS = tan_to_sin(tanS);
				float sinT = tan_to_sin(tanT);
				float r = sqrt(d2) * g_inv_R;
				float h = sinS - sinT;
				resultAO += falloff(r) * (h - h0) * fFactor;;
				h0 = h;

				// Update the current horizon angle
				tanH = tanS;
			}




		}

		numSam += 1;
	}




	if( numSam == 0)
		return 1;

	resultAO = saturate( 1 - resultAO / numSam * g_Contrast);


	return float4( resultAO, resultAO, resultAO, 1);


	
}



technique Tmain
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSmainLowQuality();
    }
}
//-----------------------------------------------------------------------------





// --------------------- denoise -------------------------------------------
float4 Blur4x4PS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
	float fFactorX = g_InvResolution.x;
	float fFactorY = g_InvResolution.y;
	//vScreenPosition += float2( 0.5f * fFactorX, 0.5f * fFactorY);
	
    float sample = 0.0f;

	for( int x=-1; x<3; x++)
	{
		for( int y=-1; y<3; y++)
		{	
			float2 vSamplePostion = vScreenPosition + float2( (x-0.5f)/g_Resolution.x, (y-0.5f)/g_Resolution.y);
			sample += tex2D( AOSampler, vSamplePostion).r;
		}
	}
	sample = sample / 16.0f;

	return float4( sample, sample, sample, 1);
}


technique Tdenoise4x4
{
    pass P0
    {
        PixelShader  = compile ps_3_0 Blur4x4PS();
    }
}


// --------------------- denoise -------------------------------------------
float4 Blur2x2PS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
	float fFactorX = g_InvResolution.x;
	float fFactorY = g_InvResolution.y;
	//vScreenPosition += float2( 0.5f * fFactorX, 0.5f * fFactorY);
	
    float sample = 0.0f;

	for( int x=0; x<2; x++)
	{
		for( int y=0; y<2; y++)
		{	
			float2 vSamplePostion = vScreenPosition + float2( (x-0.5f)/g_Resolution.x, (y-0.5f)/g_Resolution.y);
			sample += tex2D( AOSampler, vSamplePostion).r;
		}
	}
	sample = sample / 4.0f;

	return float4( sample, sample, sample, 1);
}


technique Tdenoise2x2
{
    pass P0
    {
        PixelShader  = compile ps_3_0 Blur2x2PS();
    }
}


//-----------------------------------------------------------------------------
float4 PScopy( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float fVal = tex2D( AOSampler, vTexCoord).r;
	return float4( fVal, fVal, fVal, 1);
}

technique Tcopy
{
	pass P0
	{
		PixelShader = compile ps_3_0 PScopy();
		//PixelShader = compile ps_3_0 PSexpandDarkforce();
	}
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
float4 PSsubtract( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float fVal = tex2D( AOSampler, vTexCoord).r;
	float4 vLit = tex2D( ShadowSampler, vTexCoord);
	
	vLit.rgb = max( vLit.rgb - (1-fVal), 0);
		
	return vLit;
}

technique Tsubtract
{
	pass P0
	{
		PixelShader = compile ps_3_0 PSsubtract();
	}
}
//-----------------------------------------------------------------------------

