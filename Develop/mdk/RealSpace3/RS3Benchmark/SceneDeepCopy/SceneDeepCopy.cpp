// SceneDeepCopy.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <string>
#include <set>

#include "RLib.h"
#include "MFileSystem.h"
#include "FileInfo.h"
#include "MXml.h"
#include "MStringUtil.h"
#include "SpeedTreeRT.h"

using namespace std;
using namespace cml2;

string g_ResourcePath;
string g_FileName;
string g_copyDestinationPath;
set<string> g_CopiedFiles;

bool CopyResourceFileFromXML( const string& xmlFileName );

bool ParseCommandLine( int argc, _TCHAR* argv[] )
{
	if( argc <= 3 )
	{
		cout << "Usage: " << argv[0] << " [Resource path] [XML file name] [Destination path]";
		return false;
	}

	g_ResourcePath = argv[1];
	g_FileName = argv[2];
	g_copyDestinationPath = argv[3];
	return true;
}

bool MakeDirectory()
{
	CreateDirectory( g_copyDestinationPath.c_str(), NULL );
	cout << "Make directory - " + g_copyDestinationPath + "\n";
	return true;
}

bool MakeSubDirectory( const string& subDirectoryName )
{
	string path = g_copyDestinationPath + "/" + subDirectoryName;
	CreateDirectory( path.c_str(), NULL );
	cout << "Make subdirectory - " + path + "\n";
	return true;
}

string GetFilePath( const string fileName )
{
	string fileNameToLower = ToLower( fileName );

	if( false == MIsExistFile( fileNameToLower.c_str() ) )
		return "";

	const FILEINDEX* fileIndex = MGetFileSystem()->GetFileIndex();
	FILEINDEX::const_iterator itr = fileIndex->find( fileNameToLower );
	if( itr != fileIndex->end() )
	{
		MFileInfo* pFileInfo = (*itr).second;
		return pFileInfo->strFileName;
	}

	return "";
}

bool CopyOneFile( const string& fileName, const string& subDirectory )
{
	string p = MGetPureFileNameExt( fileName );
	string sourceFilePath = GetFilePath( MGetPureFileNameExt( fileName ) );
	if( sourceFilePath.empty() )
		return false;

	string pureFileName = MGetPureFileNameExt( sourceFilePath );

	if( g_CopiedFiles.find( pureFileName ) != g_CopiedFiles.end() )
		return false;

	string destFilePath = g_copyDestinationPath + "/" + subDirectory + pureFileName;
	CopyFile( sourceFilePath.c_str(), destFilePath.c_str(), FALSE );

	cout << "Copy file - " + pureFileName + " to " + destFilePath + "\n";
	g_CopiedFiles.insert( pureFileName );

	return true;
}

bool CopyTextureFile( const string& fileName )
{
	bool bResult1 = CopyOneFile( fileName, "texture/" );
	bool bResult2 = CopyOneFile( fileName + ".dds", "texture/" );
	return ( bResult1 || bResult2 );
}

bool CopyResourceFileRecourse( MXmlElement* pParentElement )
{
	_ASSERT( pParentElement );

	for( MXmlElement* childElement = pParentElement->FirstChildElement(); childElement != NULL; childElement = childElement->NextSiblingElement() )
	{
		string val;
		if( _Contents( val, childElement ) && false == val.empty() )
		{
			if( val.find( ".dds" ) != string::npos || val.find( ".tga" ) != string::npos )
			{
				CopyTextureFile( val );
			}
			else
			if( val.find( ".elu" ) != string::npos )
			{
				CopyOneFile( val, "mesh/" );

				string eluXMLFileName = val + ".xml";
				if( CopyOneFile( val + ".xml", "mesh/" ) )
					CopyResourceFileFromXML( eluXMLFileName );

				string sceneXMLFileName = MGetPureFileName( val ) + ".scene.xml";
				if( CopyOneFile( sceneXMLFileName, "mesh/" ) )
					CopyResourceFileFromXML( sceneXMLFileName );
			}
			else
			if( val.find( ".scene.xml" ) != string::npos )
			{
				if( CopyOneFile( val, "mesh/" ) )
					CopyResourceFileFromXML( val );
			}
			else
			if( val.find( ".spt" ) != string::npos )
			{
				CopyOneFile( val, "tree/" );

				CSpeedTreeRT speedTree;
				CSpeedTreeRT::SMapBank textureInfo;

				string sptFileName = GetFilePath( val );
				if( speedTree.LoadTree( sptFileName.c_str() ) )
				{
					speedTree.GetMapBank( textureInfo );

					for( int i = 0; i < CSpeedTreeRT::TL_NUM_TEX_LAYERS - 1; ++i )
					{
						CopyTextureFile( textureInfo.m_pBranchMaps[i] );
						CopyTextureFile( textureInfo.m_pCompositeMaps[i] );
						CopyTextureFile( textureInfo.m_pBillboardMaps[i] );
					}
					CopyTextureFile( textureInfo.m_pSelfShadowMap );
				}
			}
			else
			if( val.find( ".grs" ) != string::npos )
			{
				CopyOneFile( val, "" );

				string grassTextureFileName = MGetPureFileName( val ) + ".dds";
				CopyOneFile( grassTextureFileName, "" );
			}
			else
			{
				string effectFileName = val + ".effect.scene.xml";
				if( CopyOneFile( effectFileName, "effect/" ) )
				{
					CopyResourceFileFromXML( effectFileName );
				}
			}
		}

		string nodeName = childElement->Value();
		if( false == nodeName.empty() && "TEXTURE" == nodeName )
		{
			string textureFileName;
			_Attribute( textureFileName, childElement, "name" );
			if( false == textureFileName.empty() )
			{
				CopyTextureFile( textureFileName );
			}
		}

		CopyResourceFileRecourse( childElement );
	}

	return true;
};

bool CopyResourceFileFromXML( const string& xmlFileName )
{
	MXml xml;
	if( false == xml.LoadFile( xmlFileName.c_str() ) )
		return false;

	MXmlElement* rootElement = xml.Doc()->FirstChildElement();
	if( NULL == rootElement )
		return false;

	CopyResourceFileRecourse( rootElement );
	return true;
}

void CopyResourceEtc()
{
	// SSAO
	CopyOneFile( "default.gcf", "" );
	CopyTextureFile( "vector_noise.dds" );

	// 환경
	CopyTextureFile( "rain.tga" );
	CopyTextureFile( "snow.tga" );
	CopyTextureFile( "cloudshadow.tga" );

	// 디스토션
	CopyTextureFile( "distortion_nm.dds" ); // TODO: 이 파일이 없으면 디스토션 옵션이 활성화 될 때 뻗는다.

}

int _tmain(int argc, _TCHAR* argv[])
{
	if( false == ParseCommandLine( argc, argv ) )
		return 1;

	if( NULL == MCreateFileSystem( MFILEACCESS_GENERIC, g_ResourcePath.c_str() ) )
		return 1;

	MakeDirectory();
	MakeSubDirectory( "texture" );
	MakeSubDirectory( "mesh" );
	MakeSubDirectory( "tree" );
	MakeSubDirectory( "effect" );

	CopyOneFile( g_FileName, "" );

	if( false == CopyResourceFileFromXML( g_FileName ) )
		return 1;

	// 하드 코딩된 리소스 파일;
	CopyResourceEtc();
	return 0;
}

