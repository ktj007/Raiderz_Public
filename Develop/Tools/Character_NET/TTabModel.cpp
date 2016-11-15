#include "StdAfx.h"
#include "TTabModel.h"

TTabModel::TTabModel(void)
: m_ModelType(MODEL_TYPE_NONE)
, m_PlayerType(PLAYER_TYPE_NONE)
{
}

TTabModel::~TTabModel(void)
{
}

void TTabModel::InitModelList()
{
	vector<tstring>	PathList;
	PathList.push_back(PATH_PLAYER_TOOL);
	PathList.push_back(PATH_MONSTER_TOOL);
	PathList.push_back(PATH_NPC_TOOL);
	PathList.push_back(PATH_MAPOBJ_TOOL);
	PathList.push_back(PATH_SFX_TOOL);

	//위 경로의 폴더 훝기(?)
	for( size_t i = 0; i < PathList.size(); i++)
	{
		FindModelFiles(PathList[i], m_ModelMap[i]);
	}

	//bRet = FindNextFile(hDir, &DirData);
	//FindClose(hDir);

	//훝은거 트리뷰에 세팅
	//SetModelTreeView();

	//나중에 로드를 하게 되면 폴더/+폴더 + .xml 이렇게만
}

void TTabModel::FindModelFiles( tstring &Path,ModelList & modellist)
{
	BOOL bRet = TRUE;
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};

	tstring subDir = Path+"*.*";
	hDir = FindFirstFile(subDir.c_str(), &DirData);
	while( (hDir != INVALID_HANDLE_VALUE) && bRet )
	{
		if (DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			if ((strcmp(DirData.cFileName, ".") != 0) &&
				(strcmp(DirData.cFileName, "..") != 0) &&
				(strcmp(DirData.cFileName, ".svn") != 0)) 
			{
				modellist.m_Index.push_back(DirData.cFileName);

				//if (bSubSearch)
				//{
				//	tstring strDir = Path+DirData.cFileName+"/";
				//	FindModelFiles(strDir, modellist);
				//}
			}
		}
		else if (DirData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) 
		{
			string ext = GetExtName(DirData.cFileName);

			if(strcmp(ext.c_str(), ".elu") == 0)
			{
				modellist.m_Index.push_back(DirData.cFileName);
			}
		}
		bRet = FindNextFile(hDir, &DirData);
	}
	FindClose(hDir);
}

void TTabModel::SetModelType( String^ Name )
{
	if (Name->StartsWith(String::Format("Player")))
	{
		m_ModelType = MODEL_TYPE_PLAYER;
	}
	else if (Name->StartsWith(String::Format("Monster")))
	{
		m_ModelType = MODEL_TYPE_MONSTER;
	}
	else if (Name->StartsWith(String::Format("NPC")))
	{
		m_ModelType = MODEL_TYPE_NPC;
	}
	else if (Name->StartsWith(String::Format("MapObject")))
	{
		m_ModelType = MODEL_TYPE_MAPOBJECT;
	}
	else
	{
		m_ModelType = MODEL_TYPE_NONE;
	}
}

void TTabModel::SetPlayerType(String^ nodeName)
{
	if (m_ModelType != MODEL_TYPE_PLAYER)
	{
		m_PlayerType = PLAYER_TYPE_NONE;
		return;
	}

	if (nodeName->StartsWith(String::Format("hf")))
	{
		m_PlayerType = PLAYER_TYPE_FEMALE;
	}
	else if (nodeName->StartsWith(String::Format("hm")))
	{
		m_PlayerType = PLAYER_TYPE_MALE;
	}
}