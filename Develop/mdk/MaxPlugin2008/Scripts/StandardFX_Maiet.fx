// 2010/05/18 - Part Color 추가
// 2010/06/08 - Specular 공식 엔진과 같게 변경 (D * L) + S => (D + S) * L 
// 2010/06/09 - Opcity 안나오던 문제 수정 AddressU = Wrap; AddressV = Wrap; 추가
// 2010/06/18 - NormalCalc() Normalize 추가
// This is used by 3dsmax to load the correct parser
string ParamID = "0x003";

float Script : STANDARDSGLOBAL <
	string UIWidget = "none";
	string ScriptClass = "object";
	string ScriptOrder = "standard";
	string ScriptOutput = "color";
	string Script = "Technique=Default;";
> = 0.8; // version #

// light direction (view space)
float3 g_LightDir : Direction <  
> = {1, 0, 0};

float4 g_LightPos : POSITION <  
	string UIName = "Light Position"; 
	string Object = "PointLight";
	string Space = "World";
	int refID = 0;
> = {1, 0, 0,0.0};


float4 g_LightCol : LIGHTCOLOR
<
	int LightRef = 0;
	string UIWidget = "None";
> = float4(0.0f, 0.0f, 0.0f, 0.0f);

bool g_CullNoneEnable <
	string UIName = "Cull None Enable";
> = false;

// material reflectivity
float4 k_a  <
	string UIName = "Ambient";
> = float4( 0.0f, 0.0f, 0.0f, 1.0f );    // ambient
	
float4 k_d  <
	string UIName = "Diffuse";
> = float4( 0.47f, 0.47f, 0.47f, 1.0f );    // diffuse
	
float4 k_s  <
	string UIName = "Specular";
> = float4( 1.0f, 1.0f, 1.0f, 1.0f );    // diffuse    // specular

int specular_level<
	string UIName = "Specular Level";
	string UIType = "IntSpinner";
	float UIMin = 0.0f;
	float UIMax = 500.0f;	
>  = 100;

int glossiness<
	string UIName = "Glossiness";
	string UIType = "IntSpinner";
	float UIMin = 0.0f;
	float UIMax = 500.0f;	
>  = 10;

float g_BumpScale <
	string UIName = "Bump Amount";
	float UIMin = 0.0f;
	float UIMax = 5.0f;
	float UIStep = 0.01f;
>   = 1.0f;

bool g_PartsColorEnable <
	string UIName = "Parts Color Enable";
> = false;

float4 g_PartsColor  <
	string UIName = "Parts Color";
> = float4( 0.0f, 0.0f, 0.0f, 1.0f );

bool g_OpacityEnable <
	string UIName = "Opacity Enable";
> = false;

texture g_OpacityTexture < 
	string UIName = "Opacity";
	int Texcoord = 4;
	int MapChannel = 1;
>;

bool g_TopDiffuseEnable <
	string UIName = "Diffuse Color Enable";
> = false;

texture g_TopTexture : DiffuseMap< 
	string UIName = "Diffuse Map ";
	int Texcoord = 0;
	int MapChannel = 1;
>;

bool g_GlowDiffuseEnable <
	string UIName = "Self illumination Color Enable";
> = false;

texture g_GlowTexture < 
	string UIName = "Glow Diffuse";
	int Texcoord = 5;
	int MapChannel = 1;
>;

bool g_SpecularEnable <
	string UIName = "Specular  Enable";
> = false;

bool g_UseDiffuseAlpha <
	string UIName = "Use Diffuse Alpha";
> = false;

texture g_SpecularTexture < 
	string UIName = "Specular";
	int Texcoord = 6;
	int MapChannel = 1;
>;

bool g_NormalEnable <
	string UIName = "Normal Enable";
> = false;

texture g_NormalTexture < 
	string UIName = "Normal";
	int Texcoord = 7;
	int MapChannel = 1;
>;

bool g_ReflectionEnable <
	string UIName = "Reflection Enable";
> = false;

texture g_ReflectionTexture < 
	string UIName = "Reflection";
	string ResourceType = "CUBE";
>;
float g_ReflectAmount <
	string UIName = "Reflection Amount";
	float UIMin = 0.0f;
	float UIMax = 2.0f;
	float UIStep = 0.01f;
>   = 1.0f;

sampler2D g_OpacitySampler = sampler_state
{
	Texture = <g_OpacityTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
    AddressV = Wrap;
};


sampler2D g_TopSampler = sampler_state
{
	Texture = <g_TopTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
    AddressV = Wrap;
};
	
sampler2D g_GlowSampler = sampler_state
{
	Texture = <g_GlowTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D g_SpecularSampler = sampler_state
{
	Texture = <g_SpecularTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D g_NormalSampler = sampler_state
{
	Texture = <g_NormalTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
    AddressV = Wrap;
};

samplerCUBE g_ReflectionSampler = sampler_state
{
	Texture = <g_ReflectionTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
    AddressV = Wrap;
};



// transformations
float4x4 World     		:	WORLD;
float4x4 WorldI			:	WORLDI;
float4x4 View       	:	VIEW;
float4x4 ViewI			:	VIEWI;
float4x4 Projection 	:	PROJECTION;
float4x4 WorldViewProj 	:	WORLDVIEWPROJ;
float4x4 WorldView 		:	WORLDVIEW;

int texcoord1 : Texcoord
<
	int Texcoord = 1;
	int MapChannel = 0;
	string UIWidget = "None";
>;

int texcoord2 : Texcoord
<
	int Texcoord = 2;
	int MapChannel = -2;
	string UIWidget = "None";
>;

int texcoord3 : Texcoord
<
	int Texcoord = 3;
	int MapChannel = -1;
	string UIWidget = "None";
>;


float3 NormalCalc(float3 mapNorm, float BumpScale)
{
	float3 v = {0.5f,0.5f,1.0f};
	mapNorm = lerp(v, mapNorm, BumpScale );
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	mapNorm.y = -mapNorm.y;
	mapNorm = normalize(mapNorm);
	return mapNorm;
}


struct VSIn
{
	float4 Position	: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float3 BiNormal	: BINORMAL;
	float2 UV0		: TEXCOORD0;	
	float3 Colour	: TEXCOORD1;
	float3 Alpha	: TEXCOORD2;
	float3 Illum	: TEXCOORD3;
	float3 UV1		: TEXCOORD4;
	float3 UV2		: TEXCOORD5;
	float3 UV3		: TEXCOORD6;
	float3 UV4		: TEXCOORD7;

};


struct VSOut
{
	float4 Position	: POSITION;
   	float4 UV0		: TEXCOORD0;
   	float3 LightDir	: TEXCOORD1;
   	float3 Normal	: TEXCOORD2;
   	float3 ViewDir	: TEXCOORD3;
	float4 UV1		: TEXCOORD4;
	float2 UV2		: TEXCOORD5;
	float2 UV3		: TEXCOORD6;
	float2 UV4		: TEXCOORD7;
   	
};

struct PSIn
{
   	float4 UV0		: TEXCOORD0;
   	float3 LightDir	: TEXCOORD1;
   	float3 Normal	: TEXCOORD2;
   	float3 ViewDir	: TEXCOORD3;
	float4 UV1		: TEXCOORD4;
	float2 UV2		: TEXCOORD5;
	float2 UV3		: TEXCOORD6;
	float2 UV4		: TEXCOORD7;
	float  bCULL	: VFACE;

};

struct PSOut
{
	float4 Colour		: COLOR;
};


VSOut VS(VSIn vsIn) 
{
	VSOut vsOut;                                                                                                                                                                                                                                                                                                                                   
   	vsOut.Position = mul(vsIn.Position, WorldViewProj);
   	float4 colour;
   	colour.rgb = vsIn.Colour * vsIn.Illum;
   	colour.a = vsIn.Alpha.x;
   	
   	vsOut.UV0.z = colour.r;
   	vsOut.UV0.a = colour.g;
   	
   	vsOut.UV1.z = colour.b;
   	vsOut.UV1.a = colour.a;
   	

	vsOut.Normal = vsIn.Normal;
	float4 g_CamPos = mul(ViewI[3],WorldI);
	float3 ViewDir = g_CamPos.xyz - vsIn.Position.xyz;
	float4 lightPos = mul(g_LightPos, WorldI);
	float3 lightVec = (lightPos.xyz - vsIn.Position.xyz);
	if(g_NormalEnable)
	{
		float3x3 objToTangent;

		objToTangent[0] = vsIn.BiNormal;
		objToTangent[1] = -vsIn.Tangent;
		objToTangent[2] = vsIn.Normal;
		
		vsOut.LightDir = mul(objToTangent, lightVec);
		vsOut.ViewDir = mul(objToTangent, ViewDir);
		
	}
	else
	{
		vsOut.LightDir = lightVec;
		vsOut.ViewDir = ViewDir;
	}
	
   	vsOut.UV0.xy = vsIn.UV0.xy;
   	vsOut.UV1.xy = vsIn.UV1.xy;
   	vsOut.UV2 = vsIn.UV2;
   	vsOut.UV3 = vsIn.UV3;
   	vsOut.UV4 = vsIn.UV4;
	return vsOut;
}

PSOut PS( PSIn psIn )
{
	if( !g_CullNoneEnable )
		clip(-psIn.bCULL);
	
	PSOut psOut;
	float3 GlowCol = k_d.rgb; 
	float4 TopCol = k_d; 
	float3 LightCol = float3(1.0,1.0,1.0);
	float3 Normal = normalize(psIn.Normal);
	float3 LightDir = normalize(psIn.LightDir); 
	float3 ViewDir = normalize(psIn.ViewDir);
	float3 Ambient = k_a.rgb;
	float3 SpecLevel = float3(1.0,1.0,1.0);
	float3 RefelctContrib=float3(1,1,1);
	float Alpha; 
	
	float2 normalUV = psIn.UV4;
	float2 diffuseUV = psIn.UV0;
	
	float4 newCol;
	float4 vertColour = float4(psIn.UV0.z,psIn.UV0.w,psIn.UV1.z,psIn.UV1.w);

	if(g_OpacityEnable)
		TopCol.a = tex2D(g_OpacitySampler, diffuseUV ).a;

	if(g_TopDiffuseEnable)
		TopCol.rgb = tex2D(g_TopSampler, diffuseUV );
	
	GlowCol = 0;
	if(g_GlowDiffuseEnable)
		GlowCol = tex2D(g_GlowSampler, psIn.UV2);
	
	Alpha = TopCol.a;
	
	// parts 컬러 조합 vDiffuseColor.rgb * vDiffuseColor.a + g_vPartsColor.rgb * ( 1 - vDiffuseColor.a )
	if(g_PartsColorEnable)
		TopCol.rgb = lerp( TopCol.rgb, g_PartsColor.rgb, 1 - tex2D(g_TopSampler, diffuseUV).a );

	if(g_NormalEnable)
		Normal = NormalCalc(tex2D(g_NormalSampler, normalUV).xyz, g_BumpScale);
		
	if(g_SpecularEnable)
	{
		if(g_UseDiffuseAlpha)
			SpecLevel = tex2D(g_TopSampler, psIn.UV3).aaa;
		else 
			SpecLevel = tex2D(g_SpecularSampler, psIn.UV3).xyz;
	}

	psOut.Colour.rgb = TopCol;
	
	if(g_ReflectionEnable)
	{
		float3 CUV = normalize( reflect( ViewDir, Normal).xzy);
		CUV.y = -CUV.y;
		float4 env = texCUBE(g_ReflectionSampler, CUV);
		env *=g_ReflectAmount;
		env *= SpecLevel.r;
		psOut.Colour.rgb += env;
	}
	
	float3 vReflect = 2 * dot(Normal, ViewDir) * Normal - ViewDir;
	float3 vSpecular = pow ( max(dot(vReflect, LightDir), 0), glossiness ) * ( specular_level * 0.01f );
	psOut.Colour.rgb += vSpecular * SpecLevel.rgb;
	
	psOut.Colour.rgb = (Ambient + (g_LightCol * saturate(dot(Normal, LightDir)))) * psOut.Colour.rgb;		
	psOut.Colour.rgb += GlowCol;
	psOut.Colour.a = Alpha;
	return psOut;
}


technique Default
<
	string script = "Pass=p0;";
> 
{
    pass p0 <
		string script = "Draw=Geometry;";
    >
    {		
		AlphaTestEnable	= true;
		ALPHAREF = 128;
		ALPHAFUNC = GREATEREQUAL;
			
		// AlphaBlendEnable	= false;
		// SrcBlend		= SrcAlpha;
		// DestBlend		= InvSrcAlpha;  
		VertexShader	= compile vs_3_0 VS();
		PixelShader		= compile ps_3_0 PS();
    }
}
