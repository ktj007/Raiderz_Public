sampler samMask0 : register(s0);
sampler samMask1 : register(s1);
sampler samMask2 : register(s2);
sampler samMask3 : register(s3);
sampler samMask4 : register(s4);
sampler samMask5 : register(s5);
sampler samMask6 : register(s6);
sampler samMask7 : register(s7);


shared int g_LayerCount=1;

struct PSOUTPUT {
	float4 color0  : COLOR0;
	float4 color1 : COLOR1;
};



// -------------------------------------------------------------------------------------
// Pixel Shader
// -------------------------------------------------------------------------------------
PSOUTPUT PSMAIN( in float2 vTexCoord : TEXCOORD0 ) : COLOR
{
	PSOUTPUT output = (PSOUTPUT)0;
	
	float fMask0 = g_LayerCount >= 2 ? tex2D( samMask0, vTexCoord).a : 0;
	float fMask1 = g_LayerCount >= 3 ? tex2D( samMask1, vTexCoord).a : 0;
	float fMask2 = g_LayerCount >= 4 ? tex2D( samMask2, vTexCoord).a : 0;
	float fMask3 = g_LayerCount >= 5 ? tex2D( samMask3, vTexCoord).a : 0;
	float fMask4 = g_LayerCount >= 6 ? tex2D( samMask4, vTexCoord).a : 0;
	float fMask5 = g_LayerCount >= 7 ? tex2D( samMask5, vTexCoord).a : 0;
	float fMask6 = g_LayerCount >= 8 ? tex2D( samMask6, vTexCoord).a : 0;
	float fMask7 = g_LayerCount >= 9 ? tex2D( samMask7, vTexCoord).a : 0;
	
	output.color0 = float4( fMask0, fMask1, fMask2, fMask3);
	output.color1 = float4( fMask4, fMask5, fMask6, fMask7);
		
	return output;
}

technique MainTechnique
{
    pass P0
    {
		PixelShader  = compile ps_3_0 PSMAIN();
    }
}