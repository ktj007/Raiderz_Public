//-----------------------------------------------------------------------------
// File: Distortion.fx
//
// Desc: Distortion 효과 적용
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4 g_dir = {-0.05f, 0.02f, 0.03f, 0.02f*0.7f};
float4 g_Info = {0.1f, 0.1f, 0.02f, 0.0f}; //x,y: Speed, z: Scale, w: Range
float g_fTime;

float4x4 g_matWVP;
float4 g_vResolution;

bool g_bPhysique =false;
static const int MAX_MATRICES = 54;
shared float4x4 g_blendMatrices[MAX_MATRICES];	

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
texture g_tRenderTarget;
texture g_tNormalMap;
texture g_tMaskingMap;

sampler2D samRenderTarget = sampler_state
{
    Texture = <g_tRenderTarget>;
    MagFilter = POINT;
    MipFilter = NONE;
    MinFilter = POINT;
	AddressU  = WRAP;        
    AddressV  = WRAP;
};

sampler2D samBumpMap = sampler_state
{
    Texture = <g_tNormalMap>;
    MagFilter = Linear;
    MipFilter = Linear;
    MinFilter = Linear;
	AddressU  = WRAP;        
    AddressV  = WRAP;
};

sampler2D samMaskingMap = sampler_state
{
    Texture = <g_tMaskingMap>;
    MagFilter = POINT;
    MipFilter = NONE;
    MinFilter = POINT;
	AddressU  = WRAP;        
    AddressV  = WRAP;
};


//-----------------------------------------------------------------------------
// Structure
//-----------------------------------------------------------------------------
struct EMITTER_VS_INPUT
{
	float4 vPos: POSITION;
	float4 vDiffuse	: COLOR0;
	float2 vTexcoord: TEXCOORD0;
};

struct EMITTER_VS_OUTPUT
{
	float4 vPos: POSITION;
	float4 vDiffuse	: COLOR0;
	float2 vTexcoord: TEXCOORD0;
};

struct ACTOR_VS_INPUT
{
	float4 vPos: POSITION;
	float3 vNormal :NORMAL;
	float2 vTexcoord :TEXCOORD0;
	float2 vWeight : BLENDWEIGHT;
	float4 vIndex : BLENDINDICES;
};

struct ACTOR_VS_OUTPUT
{
	float4 vPos: POSITION;
	float2 vTexcoord: TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Type: Vertex shader                                      
// Desc: Distortion shader
//-----------------------------------------------------------------------------

ACTOR_VS_OUTPUT Actor_VS(ACTOR_VS_INPUT Input, uniform bool bPhysique)
{
	ACTOR_VS_OUTPUT Output;
	
	float4 vPosition = Input.vPos;
	float3 vNormal = Input.vNormal;
	
	if(bPhysique)
	{
		vPosition =0;
		vNormal =0;
		
		float3 Index = Input.vIndex;
	
		float3 Weight;
		Weight[0] = Input.vWeight[0];
		Weight[1] = Input.vWeight[1];
		Weight[2] = 1.0f-(Input.vWeight[0]+Input.vWeight[1]);
	
		for(int i=0; i<3; ++i)
		{
			vPosition += mul(Input.vPos, g_blendMatrices[Index[i]])*Weight[i];
			vNormal += mul(Input.vNormal,(float3x3)g_blendMatrices[Index[i]])*Weight[i];	
		}
	}
	
	vPosition.xyz += vNormal*g_Info.w;	//g_Info.w => Extension Range
	
	Output.vPos = mul( vPosition, g_matWVP );
	Output.vTexcoord = Input.vTexcoord;
	
	return Output;
}


EMITTER_VS_OUTPUT EMITTER_VS(EMITTER_VS_INPUT Input)
{
	EMITTER_VS_OUTPUT Output;
	
	Output.vPos = mul( Input.vPos, g_matWVP );
	Output.vTexcoord =Input.vTexcoord;
	Output.vDiffuse =Input.vDiffuse;

	return Output;
}


//-----------------------------------------------------------------------------
// Type: Pixel shader                                      
// Desc: Distortion shader
//-----------------------------------------------------------------------------
float4 FullScreenDistortion_PS( float2 vTexcoord: TEXCOORD ) : COLOR
{
	float2 tex0 = vTexcoord + frac(g_dir.xy*g_Info.x*g_fTime);
	float2 tex1 = vTexcoord + frac(g_dir.zw*g_Info.y*g_fTime);

	float3 bump0 = tex2D(samBumpMap, tex0)*2.0f -1.0f;
	float3 bump1 = tex2D(samBumpMap, tex1)*2.0f -1.0f;

	float3 ts =g_Info.z;	//g_Info.z => Distortion Scale
	//텍스쳐 좌표가 화면 밖을 벗어나지 않게 조절(화면 경계 문제때문)
	ts.xy =min(min(g_Info.z, vTexcoord.xy), 1-vTexcoord.xy);
		
	float3 sumedBump = normalize((bump0 + bump1)/2.0f) *ts;

	float4 colDistorted = tex2D(samRenderTarget, vTexcoord + sumedBump.xy);
	colDistorted.a = 1.0f;
	
	return colDistorted;
}


float4 Distortion_PS( float2 vTexcoord: TEXCOORD0, float2 vPos: VPOS ) : COLOR
{
	float2 vRenderTargetTexcoord = vPos.xy / g_vResolution.xy + g_vResolution.zw;

	//HDR을 사용할 때는 마스킹 버퍼가 RFMT_A16B16G16R16F 포맷을 사용한다.
	//따라서 1보다 큰 수가 저장되어 왜곡 세기가 커지므로 최대값을 1로 한정한다.
	float fVisibility = min(1.0f, tex2D(samMaskingMap, vRenderTargetTexcoord).a);

	if( fVisibility > 0.0f )
	{
		float4 tex = float4(vTexcoord,vTexcoord) + frac(g_dir*g_Info.xxyy*g_fTime);

		float3 bump0 = tex2D(samBumpMap, tex.xy)*2.0f -1.0f;
		float3 bump1 = tex2D(samBumpMap, tex.zw)*2.0f -1.0f;

		//vRTTexcoord /= vRTTexcoord.w;
		
		//g_Info.z => Distortion Scale
		float3 ts =(g_Info.z *fVisibility);
		//텍스쳐 좌표가 화면 밖을 벗어나지 않게 조절(화면 경계 문제때문)
		ts.xy =min(min(ts, vRenderTargetTexcoord.xy), 1.0f-vRenderTargetTexcoord.xy);
			
		float3 sumedBump = normalize((bump0 + bump1)/2.0f) *ts;

		float4 colDistorted =0;
		
		tex.xy = vRenderTargetTexcoord.xy;
		tex.xy += sumedBump.xy;
			
		if( tex2D(samMaskingMap, tex).a > 0.0f )
			colDistorted = tex2D(samRenderTarget, tex);
			
		else
			colDistorted = tex2D(samRenderTarget, vRenderTargetTexcoord);

		return colDistorted;
	}
	else
	{
		return tex2D(samRenderTarget, vRenderTargetTexcoord);
	}	
}


float4 NoDistortion_PS( float2 vTexcoord: TEXCOORD ) : COLOR
{
	float4 vRTColor = tex2D(samRenderTarget, vTexcoord);
	clip(sign(vRTColor.a)-1);	//Alpht Test
	
	return vRTColor;
}

float4 Masking_PS() :COLOR0
{
	return 1;
}


//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------

technique FullScreenDistortion
{
    pass P0
    {
        PixelShader  = compile ps_3_0 FullScreenDistortion_PS();
    }
}

technique NoDistortion
{
    pass P0
    {
        PixelShader  = compile ps_3_0 NoDistortion_PS();
    }
}

technique Actor_Distortion
{
    pass P0
    {
		VertexShader  = compile vs_3_0 Actor_VS(g_bPhysique);
        PixelShader  = compile ps_3_0 Masking_PS();
    }
    
    pass P1
    {
		VertexShader  = compile vs_3_0 Actor_VS(g_bPhysique);
        PixelShader  = compile ps_3_0 Distortion_PS();
    }
}

technique Emitter_Distortion
{
    pass P0
    {
		VertexShader  = compile vs_3_0 EMITTER_VS();
        PixelShader  = compile ps_3_0 Distortion_PS();
    }
}