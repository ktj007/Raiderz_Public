//-----------------------------------------------------------------------------
// File: ScreenMotionBlur.fx
//
// Desc: 오브젝트 픽셀 모션블러
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------


int g_numSamples = 15;

float depthScale = 1;
float g_fDevideVelocity = 1;

	
//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);		// texture SceneColor;
sampler s1 : register(s1);		// texture Velocoty;
 

//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Do Blur!!!!!
//-----------------------------------------------------------------------------
float4 blur_ps( in float2 texCoord : TEXCOORD0 ) : COLOR
{
	float4 o;
	
	float2 orgCoord = texCoord;
		
	float4 velocity = tex2D( s1, texCoord);
	
	// %연산 수가 크면 에러를 뱉는다. 15로 제한하자.
	float loopCnt = (float)g_numSamples % 16;
	
	// 길이를 맞춘다.
	velocity /= loopCnt;
	velocity *= g_fDevideVelocity;

	// Get the initial color at this pixel.   
	float4 color = tex2D( s0, texCoord);   // sceneSampler
	texCoord += velocity; 
	float fFactorAssem = 1;
	for(float i = 1; i < loopCnt; ++i, texCoord += velocity)   
	{   
		// Sample the color buffer along the velocity vector.   
		float4 currentColor = tex2D( s0, texCoord);	// sceneSampler 
		float4 currentVelocity = tex2D( s1, texCoord);	// sceneSampler 
		
		// 속도가 0인 영역은 건들지 않는다.
		float fLen = sign( length( currentVelocity));
		
		// Add the current color to our color sum.
		//float fFactor = (1 - i/loopCnt) * fLen;
		//fFactor *= 3;
		//fFactor = min( fFactor, 1);
		//color += currentColor * fFactor;	// 꼬리가 옅어지게 칼라 팩터를 맥인다.
		//fFactorAssem += fFactor;
		
		color += currentColor * fLen;
		fFactorAssem += 1 * fLen;
	}
	
	// Average all of the samples to get the final blur color.   
	o = color / fFactorAssem;
	o.a = 1;
	
	return o;
}




//-----------------------------------------------------------------------------
// Name: ScreenMotionBlur
// Type: Technique                                     
// Desc: Do Blur!!!!!
//-----------------------------------------------------------------------------
technique Blur
{
	pass P0
	{
		PixelShader = compile ps_3_0 blur_ps();
	}
}







//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
float4 overlay_ps( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	float4 Velocity = tex2D( s0, vTexCoord);
	
	Velocity.xy *= 20.0f;

	//return float4( BlurDepth.x/BlurDepth.y, BlurDepth.x, BlurDepth.y, 1);	
	return Velocity;
}

//-----------------------------------------------------------------------------
// Name: DepthBlurOverlay
// Type: Technique                                     
// Desc: Helper shader for visualizing depth/blurriness
//-----------------------------------------------------------------------------
technique Overlay
{
	pass P0
	{
		PixelShader = compile ps_3_0 overlay_ps();
	}
}
