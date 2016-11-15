#include "stdafx.h"
#include "RShaderFX.h"

using namespace rs3;

class TestRShaderFX : public RShaderFX
{
public:
	using RShaderFX::m_strFilename;
	using RShaderFX::m_strFullFilename;
	using RShaderFX::m_strShaderString;
	using RShaderFX::m_vShaderDefines;
	using RShaderFX::m_pShaderPool;
	using RShaderFX::m_bOpenCompiledShaderFile;
	using RShaderFX::m_pCompiledEffectFileBuffer;
	using RShaderFX::m_nCompiledEffectFileBufferSize;
	using RShaderFX::m_strCachingFileName;
	using RShaderFX::IsSettingFromFile;

	TestRShaderFX() : RShaderFX(NULL) {}
	virtual ~TestRShaderFX() {}
};

class TestShaderFX : public testing::Test
{
public:
	TestShaderFX() {}
	virtual ~TestShaderFX() {}
	TestRShaderFX m_ShaderFX;
};

TEST_F( TestShaderFX, SetFromFile )
{
	string strFileName = "foo.fx";
	vector<string> vShaderDefines;

	m_ShaderFX.SetFromFile( strFileName, vShaderDefines, NULL );

	EXPECT_STREQ( strFileName.c_str(), m_ShaderFX.m_strFilename.c_str() );
}

TEST_F( TestShaderFX, SetFromString )
{
	string strShader = "abcd";
	vector<string> vShaderDefines;

	m_ShaderFX.SetFromString( strShader, vShaderDefines, NULL );

	EXPECT_STREQ( strShader.c_str(), m_ShaderFX.m_strShaderString.c_str() );
	EXPECT_EQ( false, m_ShaderFX.IsSettingFromFile() );
}

TEST_F( TestShaderFX, IsSettingFromFile )
{
	m_ShaderFX.m_strFilename.clear();
	EXPECT_EQ( false, m_ShaderFX.IsSettingFromFile() );

	vector<string> vShaderDefines;
	m_ShaderFX.SetFromFile( string("foo.fx"), vShaderDefines, NULL );

	EXPECT_EQ( true, m_ShaderFX.IsSettingFromFile() );
}