#include "GammaCorrection.fx"
#include "FxaaFunction.fx"

texture g_txScene;
float2 g_RcpFrame;


#if FXAA_PRESET0 == 1
sampler2D samRenderTarget = sampler_state
{
    Texture = <g_txScene>;
    MagFilter = POINT;
    MipFilter = NONE;
    MinFilter = POINT;
    AddressU  = WRAP;        
    AddressV  = WRAP;
};
#else
sampler2D samRenderTarget = sampler_state
{
    Texture = <g_txScene>;
	MinFilter = ANISOTROPIC;
	MagFilter = ANISOTROPIC;
	MipFilter = ANISOTROPIC;
	MaxAnisotropy = 4;
    AddressU = Clamp;
    AddressV = Clamp;	
	MaxMipLevel = 0;
	MipMapLodBias = 0;
};  
#endif

float4 FinalScenePS( float2 vTexcoord: TEXCOORD ) : COLOR
{	
	FxaaTex tex = { samRenderTarget };
	float4 vColor = float4( FxaaPixelShader( vTexcoord, tex, g_RcpFrame.xy ), 1.f );
	vColor.rgb = DecodeGammaCorrection( vColor.rgb );
	return 	vColor;
}


technique FinalScene
{
    pass P0
    {
		PixelShader  = compile ps_3_0 FinalScenePS();
    }
}