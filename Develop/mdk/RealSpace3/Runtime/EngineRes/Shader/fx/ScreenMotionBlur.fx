//-----------------------------------------------------------------------------
// File: ScreenMotionBlur.fx
//
// Desc: 화면 전체에 대한 모션 블러 구현. - 090619, OZ
// http://http.developer.nvidia.com/GPUGems3/gpugems3_ch27.html
// http://aronze.egloos.com/1378393
// Waht a simple!!!!!!! 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------

float4x4 g_previousViewProjectionMatrix;
float4x4 g_ViewInverseMatrix;

//Screen-Quad의 View-공간 정점 위치 정보
float3 g_vFarClipCornerInView;

int g_numSamples = 10;
//#define		g_numSamples	10

float g_fFarZ = 1000;
float g_fSpeed = 1;

float depthScale;

// 진앙지 기반의 블러인 경우의 진앙지 원점. 텍츠져 좌표 기반. 
// 스크린 좌표계 기반을 가지고 있으면 PS서 일일이 같은 계산이 이루어 지기 때문에 셰이더 외부에서 계산하고 상수로 넘겨 주는 게 효율적.
float4 g_vEpicenter = float4( 0.5f, 0.5f, 0,0);

float g_fClrFactor = 0;

struct PSOUTPUT {
	float4 color : COLOR0;
	float4 dof : COLOR1;
};
	
//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);		// texture SceneColor;
sampler s1 : register(s1);		// texture Depth;



float4 ReconstructPosFromDepth(float2 vScreencood, float fDepth)
{
	float3 vRay;
	vRay.xy = lerp(-g_vFarClipCornerInView.xy, g_vFarClipCornerInView.xy, vScreencood);
	vRay.z = g_vFarClipCornerInView.z;
	return float4(vRay*fDepth, 1);
}

    

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Do Blur!!!!!
//-----------------------------------------------------------------------------
float4 screen_motion_blur_basis_cam_move_ps( in float2 texCoord : TEXCOORD0 ) : COLOR
{
	float4 o;
	
	float2 orgCoord = texCoord;
		
	// Get the depth buffer value at this pixel. 
	// 뎁스 버퍼에 노말 z의 부호가 박혀있으므로 abs로 이를 제거해줘야 한다.
	float4 zOver = abs(tex2D( s1, texCoord));
	// 지오메트리 패스의 뎁스 버퍼에는 0~1값이 기록 되어 있다.
	float zOverW = zOver.x;
	
	// H is the viewport position at this pixel in the range -1 to 1.   
	float4 H = float4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1);   

	float3 viewPos = ReconstructPosFromDepth(texCoord, zOverW);
	float4 worldPos = mul(float4(viewPos,1), g_ViewInverseMatrix);

	// Current viewport position   
	float4 currentPos = H;   
	// Use the world position, and transform by the previous view-   
	// projection matrix.   
	float4 previousPos = mul(worldPos, g_previousViewProjectionMatrix);   
	// Convert to nonhomogeneous points [-1,1] by dividing by w.   
	previousPos /= previousPos.w;   
	// Use this frame's position and last frame's to compute the pixel   
	// velocity.   
	float2 velocity = (currentPos - previousPos)/2.f;  
	
	// 속력의 y값을 뒤집어 줘야 한다. 왜 그런가 ㅠㅠ 왼손 좌표계가 아니라서 좀 이상한가보다. try&err로 잡아냈다 ㅠㅠ 어이상실...
	// x값을 뒤집으면 진행 반향의 반대 느낌? 아 헤깔..
	velocity.x = -velocity.x;
	
	// %연산 수가 크면 에러를 뱉는다. 15로 제한하자.
	float loopCnt = (float)g_numSamples % 15;
	
	// 길이를 맞춘다.
	velocity /= loopCnt;
	velocity *= g_fSpeed;

	// Get the initial color at this pixel.   
	float4 color = tex2D( s0, texCoord);   // sceneSampler
	texCoord += velocity; 
	float fFactorAssem = 1;
	for(float i = 1; i < loopCnt; ++i, texCoord += velocity)   
	{   
		// Sample the color buffer along the velocity vector.   
		float4 currentColor = tex2D( s0, texCoord);	// sceneSampler 
		// Add the current color to our color sum.
		float fFactor = 1 - i/loopCnt;
		color += currentColor * fFactor;	// 꼬리가 옅어지게 칼라 팩터를 맥인다.
		fFactorAssem += fFactor;
	}   
	// Average all of the samples to get the final blur color.   
	//float4 finalColor = color / fFactorAssem;	// 레지스터 줄이자
	o = color / fFactorAssem;

	o.a = 1;
	
	return o;
}




//-----------------------------------------------------------------------------
// Name: ScreenMotionBlur
// Type: Technique                                     
// Desc: Do Blur!!!!!
//-----------------------------------------------------------------------------
technique ScreenMotionBlurBasisCamMove
{
	pass P0
	{
		PixelShader = compile ps_3_0 screen_motion_blur_basis_cam_move_ps();
	}
}







//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Do Blur!!!!!
//-----------------------------------------------------------------------------
float4 screen_motion_blur_basis_epicenter_ps( in float2 texCoord : TEXCOORD0 ) : COLOR
{
//	PSOUTPUT o;
	float4 o;
	
	float2 orgCoord = texCoord;
	
	// 현 텍셀의 속도는 진앙지로부터의 거리로 부터 얻어낸다.
	// 진앙지로부터 멀수록 속도가 증가해야하므로. 
	float2 velocity = g_vEpicenter.xy - texCoord;
	
	// %연산 수가 크면 에러를 뱉는다. 15로 제한하자.
	float loopCnt = (float)g_numSamples % 15;
	
	// 길이를 맞춘다.
	velocity /= loopCnt;
	velocity *= g_fSpeed;

	// Get the initial color at this pixel.   
	float4 color = tex2D( s0, texCoord);   // sceneSampler
	texCoord += velocity; 
	float fFactorAssem = 1;
	for(float i = 1; i < loopCnt; ++i, texCoord += velocity)   
	{   
		// Sample the color buffer along the velocity vector.   
		float4 currentColor = tex2D( s0, texCoord);	// sceneSampler 
		// Add the current color to our color sum.
		float fFactor = 1 - i/loopCnt;
		color += currentColor * fFactor;	// 꼬리가 옅어지게 칼라 팩터를 맥인다.
		fFactorAssem += fFactor;
	}   
	// Average all of the samples to get the final blur color.   
	//float4 finalColor = color / fFactorAssem;	// 레지스터 줄이자
// 	o.color = color / fFactorAssem;
// 	o.color.a = 1;
// 	o.dof = tex2D( s1, texCoord);
// 	o.dof.a = 1;

	o = color / fFactorAssem;
	o.a = 1;
	
 	return o;
}




//-----------------------------------------------------------------------------
// Name: ScreenMotionBlur
// Type: Technique                                     
// Desc: Do Blur!!!!!
//-----------------------------------------------------------------------------
technique ScreenMotionBlurBasisEpicenter
{
	pass P0
	{
		PixelShader = compile ps_3_0 screen_motion_blur_basis_epicenter_ps();
	}
}
















//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: fill color for info depth max
//-----------------------------------------------------------------------------
float4 color_fill_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	return float4( g_fFarZ, 1, 1, 1);	
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: fill color for info depth max
//-----------------------------------------------------------------------------
technique ColorFill
{
	pass P0
	{
		PixelShader = compile ps_2_0 color_fill_ps();
	}
}




//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 depth_blur_overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 BlurDepth = tex2D( s0, vTexCoord);

	//return float4( BlurDepth.x/BlurDepth.y, BlurDepth.x, BlurDepth.y, 1);	
	return float4( 1, 0, 1, 1);;
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
technique DepthOverlay
{
	pass P0
	{
		PixelShader = compile ps_3_0 depth_blur_overlay_ps();
	}
}
