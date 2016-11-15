///////////////////////////////////////////////////////////////////////////////

shared matrix	g_matWorldViewProj;
shared matrix	g_matWorldView;

shared float	g_fTime;						// current time
shared float 	g_fWindStrength;				// wind strength
shared float4	g_vWindDirection;				// wind direction
shared float4	g_avUnitizedGrassBillboard[4];	// the actual camera-facing billboard vertex positions
shared float	g_fGrassFarLod;					// the furthest distance the grass will be drawn
shared float	g_fGrassShrinkTransitionLength;	// How far to fade out the grass

shared float 	g_fGrassJigglingPeriod;	
shared float 	g_fGrassJigglingStrength;
shared float 	g_fGrassIncliningStrength;

shared float	g_fFarDist;
shared float4	g_vResolution;

const float		g_fAlphaRef = 0.31f;


sampler DiffuseMap : register(s0);
sampler LightBufferMap : register(s11);

///////////////////////////////////////////////////////////////////////////////

struct VSINPUT
{
	float4	vPosition		: POSITION;
	float3	vNormal			: NORMAL;
	float2	vDiffuseCoord	: TEXCOORD0;
	float4	vParams			: TEXCOORD1;
};

struct VSOUTPUT
{
	float4	vPosition		: POSITION;
	float4	vDiffuseCoord	: TEXCOORD0;	// z에 깊이 저장. a에 alpha 저장
	float3	vNormal			: TEXCOORD1;
};

VSOUTPUT VSMAIN( VSINPUT input )
{
	VSOUTPUT output = (VSOUTPUT)0;

	float4 vLocalPosition = input.vPosition;

	float4 vWindPosition = float4( vLocalPosition.xyz, 1.0f);
	
	// 기본적인 떨림 : 위치와 시간에 따라 다양화되게 위치와 시간을 입력값으로 사용 
	float4 tempVec = (1,2,3,4);
	tempVec+= g_vWindDirection;
	float fWindAngle =  g_fTime * g_fGrassJigglingPeriod + dot(vWindPosition, tempVec);		
	
	// 기본적인 떨림을 sin/cos함수로 -1 ~ 1 까지의 값을 구한다 ( 어느정도 랜덤화된 떨림 )
	float2 vSinCos;
	sincos(fWindAngle, vSinCos.x, vSinCos.y);
	vSinCos.y*= vSinCos.x + vSinCos.y;		// (x,y) = ( sin , cos ) => 풀이 원을 그림, 그래서 cos값을 변경시켜주어 원모양의 움직임을 벗어나게 해준다

	// 바람방향으로의 움직임     = 떨림      * 쎄기인자          		+ 기울여짐 * 쎄기인자
	float  fBackAndForthMovement = vSinCos.x *  g_fGrassJigglingStrength	+ g_vWindDirection.y  * g_fGrassIncliningStrength * 0.66;	
	float  fRightAndLeftMovement = vSinCos.y *  g_fGrassJigglingStrength	+ g_vWindDirection.x  * g_fGrassIncliningStrength;

 	float4 vFinalVertex = g_avUnitizedGrassBillboard[input.vParams.x];		// get the unitized billboard

	vFinalVertex.xy *= input.vParams.z;				// blade width
	vFinalVertex.z *= input.vParams.w;				// blade height

	//기울어진 만큼 높이를 맞추어줌
	float beforeHeight = vFinalVertex.z;
	vFinalVertex.z = sqrt(beforeHeight*beforeHeight - fRightAndLeftMovement*fRightAndLeftMovement - fBackAndForthMovement*fBackAndForthMovement);
	
	//좌우/앞뒤 움직임 적용
	vFinalVertex.x += fRightAndLeftMovement * input.vParams.y;	//좌우움직임		
	vFinalVertex.y += fBackAndForthMovement * input.vParams.y;	//앞뒤움직임	
		
	vFinalVertex.xyz += vLocalPosition;				// move to position
	vFinalVertex.w = 1.0f;						// ensure the w is 1.0
	vLocalPosition = vFinalVertex;

	// VS OUT ----------------------------------------
	output.vPosition = mul(vLocalPosition, g_matWorldViewProj);
	// View Normal
	float3 vWorldViewNormal = normalize(mul( input.vNormal, (float3x3)g_matWorldView));
	output.vNormal = vWorldViewNormal;
	// View Depth
	float4 vWorldViewPosition = mul(vLocalPosition, g_matWorldView);
	output.vDiffuseCoord.z = vWorldViewPosition.z;
	// LOD alpha component
	output.vDiffuseCoord.a = 1.0f - saturate((output.vPosition.z - g_fGrassFarLod) / g_fGrassShrinkTransitionLength);
	// Texcoord
	output.vDiffuseCoord.xy = input.vDiffuseCoord.xy;

	return output;
}


struct PSOUTPUT {
#if RT1 == 1
	float4 color  : COLOR0;
#elif RT2 == 1
	float4 color  : COLOR0;
	float4 color1 : COLOR1;
#elif RT3 == 1
	float4 color  : COLOR0;
	float4 color1 : COLOR1;
	float4 color2 : COLOR2;
#endif
};

PSOUTPUT PSMAIN( 
	VSOUTPUT input
#if LIGHTBUFFER == 1
	, float4 vPos : VPOS
#endif	
)
{
	const float fAO = 0;
	
	float4 vDiffuseColor = tex2D( DiffuseMap, input.vDiffuseCoord.xy );
	float fAlpha = input.vDiffuseCoord.a * vDiffuseColor.a;
	clip( fAlpha - g_fAlphaRef);
	
#if LIGHTBUFFER == 1
	float2 vLightCoord = vPos.xy / g_vResolution.xy + g_vResolution.zw;
	float4 fLighting = tex2D( LightBufferMap, vLightCoord);	
	vDiffuseColor.rgb *= fLighting.rgb;	
#endif // LIGHTBUFFER
	
	// 디퓨즈 버퍼 값
	float4 vDiffuseAoBufferOut = float4( vDiffuseColor.rgb, fAO);

	// 노말 버퍼 값
	float3 vNormal = input.vNormal;
	float4 vNormalBufferOut = float4( vNormal.x, vNormal.y, 1, 1);
	
	// 뎁스 스페큘라 버퍼 값
	float fLinearDepth = -input.vDiffuseCoord.z/g_fFarDist;
	int nZDirection = (sign(vNormal.z)>=0)*2-1;
	float4 vDepthSpecBufferOut = float4( fLinearDepth * nZDirection, 0, 1, 1);

	PSOUTPUT output = (PSOUTPUT)0;	
#if RT1 == 1
	vDiffuseAoBufferOut.a = 1;
	output.color = vDiffuseAoBufferOut;
#elif RT2 == 1
	output.color = vNormalBufferOut;
	output.color1 = vDepthSpecBufferOut;
#elif RT3 == 1
	output.color = vNormalBufferOut;
	output.color1 = vDepthSpecBufferOut;
	output.color2 = vDiffuseAoBufferOut;
#endif

	return output;
}

///////////////////////////////////////////////////////////////////////////////

technique MainTechnique
{
	pass P0
	{
		// VPOS 때문에 SM3
#if LIGHTBUFFER == 1
		VertexShader = compile vs_3_0 VSMAIN();
		PixelShader  = compile ps_3_0 PSMAIN();
#else
		VertexShader = compile vs_2_0 VSMAIN();
		PixelShader  = compile ps_2_0 PSMAIN();
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////