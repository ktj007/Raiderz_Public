
#include "StdAfx.h"
#include "ProjectType.h"
#include "MFileSystem.h"


void ProjectType::SetProjectType( const char* szPath)
{
	PROJECT_TYPE type = PT_NONE;

	// 입력한 패스의 상위 폴더에 특정 파일이 존재하는지를 찾아서 프로젝트 타입을 판별함.
	string strPathRuntime = szPath;
	while ( type == PT_NONE  &&  strPathRuntime.empty() == false)
	{
		strPathRuntime = MGetPathFileNameFromRelativePath( strPathRuntime, "../");

		for ( int i = 0;  i < PT_END;  i++)
		{
			if ( strlen( FIND_FILE_NAME[ i]) < 2)
				continue;

			string find = strPathRuntime + FIND_FILE_NAME[ i];

			WIN32_FIND_DATA findfile;
			HANDLE hFind = FindFirstFile( find.c_str(), &findfile);

			if ( hFind != INVALID_HANDLE_VALUE)
			{
				type = (PROJECT_TYPE)i;
				break;
			}
		}
	}

	if ( m_pProjectTypeInfo->m_ProjectType != type)
	{
		delete m_pProjectTypeInfo;


		// 프로젝트 별로 추가합니다.
		switch ( type)
		{
		case PT_RAIDERZ :	{ m_pProjectTypeInfo = new ProjectTypeInfoSoulHunt( strPathRuntime.c_str());	break;	}
		case PT_GUNZ :		{ m_pProjectTypeInfo = new ProjectTypeInfoGunz( strPathRuntime.c_str());		break;	}
		default :			{ m_pProjectTypeInfo = new ProjectTypeInfo();									break;	}
		}
	}

	m_pProjectTypeInfo->m_strPathRoot = szPath;
}







// Soul Hunt 설정
ProjectTypeInfoSoulHunt::ProjectTypeInfoSoulHunt( const char* szPathRuntime)
{
	m_ProjectType = PT_RAIDERZ;
	m_strPrefix = "sh_";
	m_strPathRuntime = szPathRuntime;
	m_strPathSound = m_strPathRuntime + "Data/Sound/Effect";
	m_strFnameSndXml = m_strPathRuntime + "Data/Sound/sound.xml";
}


// GunZ 설정
ProjectTypeInfoGunz::ProjectTypeInfoGunz( const char* szPathRuntime)
{
	m_ProjectType = PT_GUNZ;
	m_strPrefix = "gz_";
	m_strPathRuntime = szPathRuntime;
	m_strPathSound = m_strPathRuntime + "Data/Sound/Effect";
	m_strFnameSndXml = m_strPathRuntime + "Data/Sound/sound.xml";
}
