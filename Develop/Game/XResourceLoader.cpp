#include "stdafx.h"
#include "XResourceLoader.h"



XResourceLoader::XResourceLoader()
{
	// Files
	m_vFileList.push_back( XResFile( "Data/system.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Script.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/common.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/login.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/character.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/game.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/cutscene.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/icon.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/minimap.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Interface/screeneffects.mrs", 0));
	m_vFileList.push_back( XResFile( "Data/Field.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Zone.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/SFX.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Shader.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/CutScene.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/MapObject.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/Monster.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/NPC.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/Player.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/Sky.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/SpeedTree.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Model/weapons.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/Character.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/Map.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/SFX.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/Sky.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/SpeedGrass.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/SpeedTree.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/Tile.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Texture/Weapon.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Sound.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Sound/BGM.mrs", 1));
	m_vFileList.push_back( XResFile( "Data/Sound/Effect.mrs", 1));
}


// XResourceLoader
XResourceLoader::~XResourceLoader()
{
	m_vFileList.clear();
}


// LoadResource
bool XResourceLoader::LoadResource()
{
	::DeleteFile( L"time.txt");

	FILE* fp;
	char _buff[ 256];
	fopen_s( &fp, "time.txt", "a");

	DWORD _start = timeGetTime();

	for ( list< XResFile>::iterator itr = m_vFileList.begin();  itr != m_vFileList.end();  itr++)
	{
		DWORD _t = timeGetTime();

		global.filesys->AddEncryptedFile( (*itr).strFileName.c_str());

		sprintf_s( _buff, "%s = %u\n", (*itr).strFileName.c_str(), timeGetTime() - _t);
		fputs( _buff, fp);
	}

	sprintf_s( _buff, "total time = %u\n", timeGetTime() - _start);
	fputs( _buff, fp);
	fclose( fp);

	return true;
}


// GetInstance
XResourceLoader* XResourceLoader::GetInstance()
{
	static XResourceLoader instance;
	return &instance;
}
