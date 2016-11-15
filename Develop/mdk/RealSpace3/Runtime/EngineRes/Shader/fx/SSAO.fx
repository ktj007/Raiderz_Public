//-----------------------------------------------------------------------------
// File: SSAO.fx
//
// Desc: Screen Space Ambient Occlusion
// 랜덤 벡터 생성
//  - http://wiki.gamedev.net/index.php/D3DBook:Screen_Space_Ambient_Occlusion
//  - http://kyruie.tistory.com/31
// 오클루젼 계산
//  - shaderx7 6.1
// halo removal : 적용 실패. 시간을 더 두고 볼것
// - http://www.gamedev.net/community/forums/topic.asp?topic_id=550699
// - 이 방법을 사용 하면 blur 패스가 필요 없어진댄다...
// - 그러나 전통적인 SSAO보다는 느려진다는데, 차라리 blur패스를 거치는게 나은지..
//-----------------------------------------------------------------------------















#define NUM_SAMPLE_NUM 16


// x = RT width
// y = RT height
// z = near
// w = far
float4		vViewDimensions;

// x = default occlusion value
// y = kernel sphere radius 
// z = multiple of kernel radius to use for range truncation
// w = delta scale multiplier
//float4		vSSAOParams = float4( 0.5f, 40.0f, 4.0f, 200.0f );
float4		vSSAOParams = float4( 0.0f, 30.0f, 4.0f, 200.0f );



// 깊이 버퍼에 써져 있는 값은 0~1사이의 z/w값이다. 이를 실제 깊이 정보로 변환하기 위해서는 뷰프로젝션 인버스 매트릭스가 필요하다.
// 하지만 깊이만 필요하고 x,y는 필요가 없으므로 3,4열의 정보만 셋팅
//matrix	g_matProjInv;
float4 g_vProjInvZ;
float4 g_vProjInvW;


// for Gaussian ---------------------------
// Contains sampling offsets used by the techniques
float2	g_avSampleOffsets[13];
float4	g_avSampleWeights[13];
int		g_nSampleCount = 13;


#define NUM_GAUSS	6
float	g_fWeight[NUM_GAUSS];

float	g_fBright = 0.5f;



sampler DepthSampler	: register(s0);
sampler RandomSampler	: register(s1);
sampler AOSampler		: register(s2);	// AO 생성 시는 마스크로 사용
sampler NormalSampler	: register(s3);




float4 finalColor( in float fValue)  
{  
	float sample = fValue;
	
 	sample = saturate( fValue + 0.05f);
   	//sample = ( pow( sample, 4) /*+ sample*/);
   	//sample = /*saturate*/( sample * 2);
   	//sample = saturate( pow( sample, 8));

 	// 어두운 영역만 추려내기..
	// 전반적으로 밝기를 높이고
	//sample = sample * 2;
	//sample = saturate( sample);

	// 전반적으로 대비를 높이고
	sample = pow( sample, 2);
	// 일정 이상 밝기는 날려버린다
 	//sample = smoothstep( 0, 0.5f, sample);
	// 0 ~ 1값이 되었고 기존의 0.5 이상은 날아가버렸으므로
	// 다시 0.5이하의 값들로 조정
	//sample = sample / 2;



	
	return float4( sample, sample, sample, 1);
}  


float3 finalNormal( in float3 norm)
{
	norm.z = -sqrt(1.0f - norm.x*norm.x - norm.y*norm.y);
	return norm;
}


//-----------------------------------------------------------------------------
float readDepth( in float2 coord)  
{  
	// 마스크를 읽음. 깊이가 0이 되면 AO가 발생 안하겠지..
	//float fMaskValue = saturate( 1 - tex2D( AOSampler, coord ).a) * vViewDimensions.w * 3;
	
	// 샘플링 할 점의 깊이를 얻음. 
	// fDepthValue : 0~1. 리니어 아님.
	float fDepthValue = abs(tex2D( DepthSampler, coord ).r);
	float4 vDepthValue = float4( 0,0,fDepthValue,1);
	
	// 뷰 프로젝션 인버스 매트릭스를 곱하여 나온 결과를 w로 나눈 것과 같은 효과
	return (dot( vDepthValue, g_vProjInvZ) / dot( vDepthValue, g_vProjInvW));
}   



// shaderX 7의 소스를 오클루전 구하는 영역 부분만 위키 것으로 변형
float4 PSmainCRYTECKadjustGDWIKI( in float2 screenTC : TEXCOORD0 ) : COLOR
{
	const float3 pSphere[10] = {
		float3(-0.010735935, 0.01647018, 0.0062425877),
		float3(-0.06533369, 0.3647007, -0.13746321),
		float3(-0.6539235, -0.016726388, -0.53000957),
		float3(0.40958285, 0.0052428036, -0.5591124),
		float3(-0.1465366, 0.09899267, 0.15571679),
		float3(-0.44122112, -0.5458797, 0.04912532),
		float3(0.03755566, -0.10961345, -0.33040273),
		float3(0.019100213, 0.29652783, 0.066237666),
		float3(0.8765323, 0.011236004, 0.28265962),
		float3(0.29264435, -0.40794238, 0.15964167)
	};

 

	float2 screenSize = vViewDimensions.xy;
	// 파 클리핑 면 거리 실 단위
	float farClipDist = vViewDimensions.w;	// - vViewDimensions.z;
	
	// 로테이션 벡터 얻음. 
	float2 rotationTC = screenTC * 18;


	rotationTC = screenTC * screenSize / 4;


	float3 vRotation = 2 * tex2D( RandomSampler, rotationTC).rgb - 1;

		
	// 현재 픽셀의 뎁스 얻어옴. 실단위
	float fPixelDepth = readDepth( screenTC);
	float3 vPixelNorm = finalNormal( tex2D( NormalSampler, screenTC).xyz);
	//float fPixelMask = tex2D( AOSampler, screenTC).a;
	//bool bNoSelfAO = ((0.1f < fPixelMask) && (fPixelMask < 0.9f));
	float fRadius = vSSAOParams.y; 
	float fFullDepth = vViewDimensions.w - vViewDimensions.z;
	float fFar = vViewDimensions.w;
	float fRadiusTruncation = vSSAOParams.z;
	float fDefaultValue = vSSAOParams.x;

	// vSSAOParams
	// x = default occlusion value
	// y = kernel sphere radius 
	// z = multiple of kernel radius to use for range truncation
	// w = delta scale multiplier


	float3 vKernelScale = float3( fRadius / fPixelDepth, fRadius / fPixelDepth, fRadius) ;
	
		
	float Accessibility = 0;
	float AccessibilityTmp = 0;
	float SamCnt = 0.0000001f;	// devide by zero 안나오게
	//int nSelfAOCnt = 0;

	float fDotAssum = 0;

	//float fAOIgnoreValue = lerp( saturate( vRotatedKernel.z), vSSAOParams.x, saturate( vRotatedKernel.z / ( vSSAOParams.y * vSSAOParams.z ) ) );
	
	// 심플 영역과 Accessibility 누적
	for( int i=0; i<10; i++)
	{
		// generate offset vector ( this code line is executed only at shader compile stage)
		// 큐브 코너를 사용 그리고 서로 다른 길이를 부여
		float3 vOffset = pSphere[i];
		// 오프셋 벡터를 리프렉트로 만듦.
		float3 vRotatedKernel = reflect( vOffset, vRotation) * vKernelScale;
		float2 vSamTC = vRotatedKernel.xy + screenTC.xy;
 		float fSampleDepth = readDepth( vSamTC);
		float3 vSampleNorm = finalNormal( tex2D( NormalSampler, vSamTC).xyz);
 		// AO를 발생 시키는지 아닌지의 마스크 정보
 		//float fSampleMask = tex2D( AOSampler, vSamTC).a;
 		
//  		if( fSampleMask == 0)
// 		{
// 			// AO를 하지 않는 것의 경우 비교 값을 같게 함으로써 AO가 발동 안하게
// 			//fSampleDepth = fPixelDepth;
// 		}
// 		else
 		{
			// 미터 단위 상태로 계산을 수행하자
			float fDelta = max( fSampleDepth - fPixelDepth + vRotatedKernel.z, 0.0f );
			float fRange = abs( fDelta ) / ( vKernelScale.z * fRadiusTruncation );
			float fDot = dot( vSampleNorm, vPixelNorm);

			// 내적을 누적해서 모든 샘플링의 노말이 거의 일치하면 이놈은 광활한 평면에 있는 것이므로 AO를...
			fDotAssum += fDot;

			// 내적 값이 일정 이상의 경우만 적용 되게....
			fDot = smoothstep( 0.95f, 1, fDot);
			float fFactor = saturate( 1 - fDot);
			//fFactor = pow( fFactor, 2);

			fFactor = 1;

	
			Accessibility += lerp( fDefaultValue, lerp( saturate( fDelta), fDefaultValue, saturate( fRange ) ), fFactor);
			SamCnt++;
		}

		// 셀프 AO를 하지 않는 것은 .. 어랏? 위에랑 똑같나.. ㅡㅡ? 좀더 신중히 코드를 간략화 해보자..
		// 내적 연산 적용 시 Self 미 적용
		// 노말 내적 연산을 곱해주는 경우에는 터레인의 검은 선 현상이 발생하지 않을 것이다.
		// 왜냐면 연속적인 노말의 연속이므로... 
// 		if( 0.1f < fSampleMask && fSampleMask < 0.9f )
// 		{
// 			// notthing
// 			nSelfAOCnt++;
// 			AccessibilityTmp += fDefaultValue;
// 		}

	}



	// 평균 값
	Accessibility = Accessibility / SamCnt;
	Accessibility = 1 - Accessibility;



// 	if( bNoSelfAO && (nSelfAOCnt==NUM_SAMPLE_NUM))
// 	{
// 		Accessibility = 1 - (AccessibilityTmp / NUM_SAMPLE_NUM);
// 	}



	float4 vOut = finalColor( Accessibility);



	// 내적 누적의 평균
	float fDotAverage = saturate( fDotAssum / SamCnt);
	// 1과 가까운 놈만 선택 -> 분포 : 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 1
	//fDotAverage = smoothstep( 0.95f, 1, fDotAverage);
	fDotAverage = pow( fDotAverage, 3);
	//fDotAverage = 1 - fDotAverage;
	// 값에 반영
	// 0 : 평면이므로 기본 값 반영
	// ~ 
	// 1 : 원래대로
	return lerp( vOut, float4(1,1,1,1), fDotAverage);

	
}


technique Tmain
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSmainCRYTECKadjustGDWIKI();
    }
}
//-----------------------------------------------------------------------------



// --------------------- denoise -------------------------------------------

float4 Blur4x4PS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
	float fFactorX = 1/vViewDimensions.x;
	float fFactorY = 1/vViewDimensions.y;

	vScreenPosition += float2( 0.5f * fFactorX, 0.5f * fFactorY);
	
    float sample = 0.0f;

	for( int x=0; x<4; x++)
	{
		for( int y=0; y<4; y++)
		{	
			//float2 vSamplePostion = vScreenPosition + float2( (x-0.5f)/vViewDimensions.x, (y-0.5f)/vViewDimensions.y);
			float2 vSamplePostion = vScreenPosition + float2( (x-2)*fFactorX, (y-2)*fFactorY);

			sample += tex2D( AOSampler, vSamplePostion).r;
		}
	}

	sample = sample / 16.0f;


	return float4( sample, sample, sample, 1);

}


technique Blur4x4
{
    pass P0
    {
        PixelShader  = compile ps_3_0 Blur4x4PS();
    }
}

//-----------------------------------------------------------------------------





// --------------------- Blur4x4BaiasEdge -------------------------------------------

float4 Blur4x4BaiasEdgePS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
	float fFactorX = 1/vViewDimensions.x;
	float fFactorY = 1/vViewDimensions.y;

	vScreenPosition += float2( 0.5f * fFactorX, 0.5f * fFactorY);
	
    float sample = 0.0f;
	float cnt = 0.0000001f;	// devide by zero 방지

	for( int x=0; x<4; x++)
	{
		for( int y=0; y<4; y++)
		{	
			//float2 vSamplePostion = vScreenPosition + float2( (x-0.5f)/vViewDimensions.x, (y-0.5f)/vViewDimensions.y);
			float2 vSamplePostion = vScreenPosition + float2( (x-2)*fFactorX, (y-2)*fFactorY);

			// 랜덤 샘플러의 인덱스에 엣지를 설정
			if( tex2D( RandomSampler, vSamplePostion).r < 0.5f)
			{
				sample += tex2D( AOSampler, vSamplePostion).r;
				cnt = cnt + 1;
			}
		}
	}

	sample = sample / cnt;


	//TESTOZ
	//sample = tex2D( DepthSampler, vScreenPosition).r;
	//sample = tex2D( AOSampler, vScreenPosition).r;


	return float4( sample, sample, sample, 1);

}


technique Blur4x4BaiasEdge
{
    pass P0
    {
        PixelShader  = compile ps_3_0 Blur4x4BaiasEdgePS();
    }
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// 어두운 영역을 한 픽셀씩 확대
float4 PSexpandDarkforce( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float fFactorX = 1/vViewDimensions.x;
	float fFactorY = 1/vViewDimensions.y;
	
	float fS = 1;
	float fV = 1;
	
	// 중
	fS = tex2D( AOSampler, vTexCoord).g;
	fV = min( fS, fV);
	
	// 상
	fS = tex2D( AOSampler, vTexCoord + float2( 0, -fFactorY)).r;
	fV = min( fS, fV);
	// 하
	fS = tex2D( AOSampler, vTexCoord + float2( 0,  fFactorY)).r;
	fV = min( fS, fV);
	// 좌
	fS = tex2D( AOSampler, vTexCoord + float2( -fFactorX, 0)).r;
	fV = min( fS, fV);
	// 우
	fS = tex2D( AOSampler, vTexCoord + float2(  fFactorX, 0)).r;
	fV = min( fS, fV);
	
	
	// 좌상
	fS = tex2D( AOSampler, vTexCoord + float2( -fFactorX, -fFactorY)).r;
	fV = min( fS, fV);
	// 좌하
	fS = tex2D( AOSampler, vTexCoord + float2( -fFactorX,  fFactorY)).r;
	fV = min( fS, fV);
	// 우상
	fS = tex2D( AOSampler, vTexCoord + float2( fFactorX, -fFactorY)).r;
	fV = min( fS, fV);
	// 우하
	fS = tex2D( AOSampler, vTexCoord + float2( fFactorX, fFactorY)).r;
	fV = min( fS, fV);

	
	
	return float4( fV, fV, fV, 1);
}

technique TexpandDarkforce
{
	pass P0
	{
		PixelShader = compile ps_3_0 PSexpandDarkforce();
	}
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// 밝은 영역을 한 픽셀씩 확대
// 엣지를 타이트하게 검출 하기 위해 뎁스 맵을 확장 하는 경우는 밝은 부분 기준으로 확장하여야 한다,
float4 PSexpandBrightforce( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float fFactorX = 1/vViewDimensions.x;
	float fFactorY = 1/vViewDimensions.y;
	
	float fS = 0;
	float fV = 0;
	
	// 중
	fS = tex2D( DepthSampler, vTexCoord).r;
	fV = max( fS, fV);
	
	// 상
	fS = tex2D( DepthSampler, vTexCoord + float2( 0, -fFactorY)).r;
	fV = max( fS, fV);
	// 하
	fS = tex2D( DepthSampler, vTexCoord + float2( 0,  fFactorY)).r;
	fV = max( fS, fV);
	// 좌
	fS = tex2D( DepthSampler, vTexCoord + float2( -fFactorX, 0)).r;
	fV = max( fS, fV);
	// 우
	fS = tex2D( DepthSampler, vTexCoord + float2(  fFactorX, 0)).r;
	fV = max( fS, fV);
	
	
	// 좌상
	fS = tex2D( DepthSampler, vTexCoord + float2( -fFactorX, -fFactorY)).r;
	fV = max( fS, fV);
	// 좌하
	fS = tex2D( DepthSampler, vTexCoord + float2( -fFactorX,  fFactorY)).r;
	fV = max( fS, fV);
	// 우상
	fS = tex2D( DepthSampler, vTexCoord + float2( fFactorX, -fFactorY)).r;
	fV = max( fS, fV);
	// 우하
	fS = tex2D( DepthSampler, vTexCoord + float2( fFactorX, fFactorY)).r;
	fV = max( fS, fV);

	
	
	return float4( fV, fV, fV, 1);
}

technique TexpandBrightforce
{
	pass P0
	{
		PixelShader = compile ps_3_0 PSexpandBrightforce();
	}
}
//-----------------------------------------------------------------------------




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

float4 GaussBlurPS
    (
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	
    float sample = 0.0f;

	//float loopCnt = (float)g_nSampleCount % 15;
	
	//for( int i=0; i < loopCnt; i++ )
	for( int i=0; i < 5; i++ )
	{
		sample += g_avSampleWeights[i] * tex2D( AOSampler, vScreenPosition + g_avSampleOffsets[i]).r;
	}

	return float4( sample, sample, sample, 1);
	
}


technique GaussBlur
{
    pass P0
    {
        PixelShader  = compile ps_3_0 GaussBlurPS();
    }
}



//-----------------------------------------------------------------------------
float4 GaussBlurPSX( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{   
	float weightFactorN[NUM_GAUSS];
	float weightFactorP[NUM_GAUSS];

	int i;
	for( i=0; i<NUM_GAUSS; i++)
	{
		// 샘플링 점 위치 선정
		weightFactorN[ i] = -i / vViewDimensions.x;
		//weightFactorP[ NUM_GAUSS - (i+1)] = i / vViewDimensions.x; ????
		weightFactorP[ i] = i / vViewDimensions.x;
	}


    float Color = 0;
	bool bIgnoreN = false; 
	bool bIgnoreP = false;
    float fWeightSum = 0.0000001f;	// devide by zero 방지용

	float2 vTexN, vTexP;
    for( i=0; i<NUM_GAUSS; i++)
    {
		// 샘플링 대상 위치 선정
		vTexN = vTexCoord + float2( weightFactorN[i], 0 );
		vTexP = vTexCoord + float2( weightFactorP[i], 0 );

		// 마스킹 확인
		if( i > 0)
		{
			if(!bIgnoreN)
				bIgnoreN = ( tex2D( DepthSampler, vTexN).r > 0.9f) ? true : false;
			if(!bIgnoreP)
				bIgnoreP = ( tex2D( DepthSampler, vTexP).r > 0.9f) ? true : false;
		}

		// 컬러 얻음
		if(bIgnoreN && bIgnoreP)
		{
			break;
		}
		if(!bIgnoreN)
		{
			Color += g_fWeight[i] * tex2D( AOSampler, vTexN).r;
			fWeightSum += g_fWeight[i];
		}
		if(!bIgnoreP)
		{
			Color += g_fWeight[i] * tex2D( AOSampler, vTexP).r;
			fWeightSum += g_fWeight[i];
		}
    }
	Color = Color / fWeightSum;

	// TESTOZ
	//Color = tex2D( AOSampler, vTexCoord).r;
   
    return float4( Color, Color, Color, 1);
}


technique GaussBlurX
{
    pass P0
    {
        PixelShader  = compile ps_3_0 GaussBlurPSX();
    }
}


float4 GaussBlurPSY( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{  
	float weightFactorN[NUM_GAUSS];
	float weightFactorP[NUM_GAUSS];

	int i;
	for( i=0; i<NUM_GAUSS; i++)
	{
		weightFactorN[ i] = -i / vViewDimensions.y;
		//weightFactorP[ NUM_GAUSS - (i+1)] = i / vViewDimensions.y;  ????
		weightFactorP[ i] = i / vViewDimensions.y;
	}

	float Color = 0;
	bool bIgnoreN = false; 
	bool bIgnoreP = false;
    float fWeightSum = 0.0000001f;	// devide by zero 방지용
    
	float2 vTexN, vTexP;
    for( i=0; i<NUM_GAUSS; i++)
    {
		// 샘플링 대상 위치 선정
		vTexN = vTexCoord + float2( 0, weightFactorN[i]);
		vTexP = vTexCoord + float2( 0, weightFactorP[i]);

		// 마스킹 확인
		if( i > 0)
		{
			if(!bIgnoreN)
				bIgnoreN = ( tex2D( DepthSampler, vTexN).r > 0.9f) ? true : false;
			if(!bIgnoreP)
				bIgnoreP = ( tex2D( DepthSampler, vTexP).r > 0.9f) ? true : false;
		}

		// 컬러 얻음
		if(bIgnoreN && bIgnoreP)
		{
			break;
		}
		if(!bIgnoreN)
		{
			Color += g_fWeight[i] * tex2D( AOSampler, vTexN).r;
			fWeightSum += g_fWeight[i];
		}
		if(!bIgnoreP)
		{
			Color += g_fWeight[i] * tex2D( AOSampler, vTexP).r;
			fWeightSum += g_fWeight[i];
		}

    }
	Color = Color / fWeightSum;

	// TESTOZ
	//Color = tex2D( AOSampler, vTexCoord).r;
    
    //return float4( Color, Color, Color, 1);
	// 블러 최동 단계에서 색을 보정한다. 보정 -> 블러가 아닌 블러 -> 보정 되게ㅐ,
	return finalColor( Color);
}

technique GaussBlurY
{
    pass P0
    {
        PixelShader  = compile ps_3_0 GaussBlurPSY();
    }
}

//-----------------------------------------------------------------------------






