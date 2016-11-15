//Sampler
sampler LightMap : register(s1);
sampler GDepth : register(s5);


float	g_R = 30.0f;
float4	g_FocalLen;
float4	g_InvFocalLen;
float4	g_InvResolution;
float4	g_Resolution;
float	g_FarZ;


#define	M_PI			3.14159265f
#define NUM_SAMPLE		3


//----------------------------------------------------------------------------------
float2 rotate_direction(float2 Dir, float2 CosSin)
{
    return float2(Dir.x*CosSin.x - Dir.y*CosSin.y, 
                  Dir.x*CosSin.y + Dir.y*CosSin.x);
}


// --------------------- denoise -------------------------------------------
float4 BlurPS
    (
    in float2 screenTC : TEXCOORD0
    ) : COLOR
{

	float fPointDepthValue = abs(tex2D(GDepth, screenTC).x);
	float fPointDepth = fPointDepthValue * g_FarZ;
	float2 step_size = 0.5f * g_R  * g_FocalLen.xy / fPointDepth;
	
	//step_size = g_InvResolution.xy * g_R;
	//step_size = g_InvResolution * g_R;
	//step_size = 0.5f * g_R  * (1-fPointDepth);
	//step_size = g_InvResolution.xy * g_Resolution.xy / 100;
	
	
	step_size = max( step_size, float2( 0.01f, 0.01f));
	
	
	
	float2 rotationTC = screenTC * g_Resolution.xy / 4.0f;
	float theta = (rotationTC.x * rotationTC.y) * M_PI * 2;
	
	

	
	
	float2 rand = float2( cos(theta), sin(theta));
	float alpha = 2.0f * M_PI / NUM_SAMPLE;
	
	
	
	float4 vPointValue = tex2D( LightMap, screenTC);
	float3 sample = vPointValue.rgb; 
	int numSam = 1;
	float fFactor = 1;
	
	
	// 심플 영역과 Accessibility 누적
	for( int i=0; i<NUM_SAMPLE*2; i++)
	{
		int d = i / 2;

		float angle = alpha * d;
		float2 dir = float2( cos(angle), sin(angle));
		float2 vRotatedKernel = rotate_direction(dir, rand.xy) * step_size.xy *  ((float)d)/NUM_SAMPLE;

		//if( length(vRotatedKernel.xy) <= length(g_InvResolution.xy))
		//	continue;
			
		// 0,1 -> -1 , 1
		vRotatedKernel = vRotatedKernel * sign(((i%2) * 2) -1);
		float2 vSamTC = vRotatedKernel.xy + screenTC.xy;
		
		float4 vSamValue = tex2D( LightMap, vSamTC);
		
		if( abs( abs(vPointValue.a) - abs(vSamValue.a)) < 0.01f)
		{	
			sample += vSamValue.rgb * fFactor;
			numSam += 1 * fFactor;
			fFactor += 0.5f;
		}
	}
	
	sample = saturate(sample / numSam);
	
	return float4( sample.rgb, 1);
	

}


technique Tmain
{
    pass P0
    {
        PixelShader  = compile ps_3_0 BlurPS();
    }
}

