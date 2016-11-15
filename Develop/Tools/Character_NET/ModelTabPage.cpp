#include "StdAfx.h"
#include "ModelTabPage.h"

void Character_NET::ModelTabPage::SetModelTreeView()
{
	vector<tstring>	PathList;
	PathList.push_back(PATH_PLAYER_TOOL);
	PathList.push_back(PATH_MONSTER_TOOL);
	PathList.push_back(PATH_NPC_TOOL);
	PathList.push_back(PATH_MAPOBJ_TOOL);
	PathList.push_back(PATH_SFX_TOOL);

	ModeltreeView->Nodes->Clear();

	MODELMAP ModelMap = g_pMainApp->m_TabModel.GetMap();


	tstring subDir;
	TreeNode^ node;
	for(int i = 0; i< 3;i++)
	{
		node = ModeltreeView->Nodes->Add(
			String::Format("{0}",GlobalObjects::g_pGlobal->GetModelTypeStr((Character_NET::ModelType)i)));

		int size = ModelMap[i].m_Index.size();
		for( int j = 0; j < size; j++)
		{
			String^ nodename = gcnew String(ModelMap[i].m_Index[j].c_str());
			TreeNode^ temp = node->Nodes->Add(nodename);
			subDir = PathList[i]+ModelMap[i].m_Index[j]+"/"+ModelMap[i].m_Index[j]+".elu";
			temp->Tag = gcnew String(subDir.c_str());

			if( !ExistFile(subDir.c_str()))
			{
				temp->BackColor = System::Drawing::Color::LightGray;
			}
		}
	}

	//for(int i = 3; i< 5; i++)	// MapObjcet, Effect Animation Format 뽑을때
	int i = 3;	// 이것과 체인지
	{
		node = ModeltreeView->Nodes->Add(
			String::Format("{0}",GlobalObjects::g_pGlobal->GetModelTypeStr((Character_NET::ModelType)i)));

		int size = ModelMap[i].m_Index.size();
		for( int j = 0; j < size; j++)
		{
			TreeNode^ temp = node;
			subDir = PathList[i]+ModelMap[i].m_Index[j];
			SetMapObjTreeView(subDir, temp, node, PathList[i]);
		}
	}
}

void Character_NET::ModelTabPage::SetMapObjTreeView( tstring &subDir, TreeNode^ temp, TreeNode^ node, tstring &Path )
{
	BOOL bRet = TRUE;
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};
	tstring subDirPath = subDir + "*.*";
	hDir = FindFirstFile(subDirPath.c_str(), &DirData);
	while( (hDir != INVALID_HANDLE_VALUE) && bRet )
	{
		if (DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			if ((strcmp(DirData.cFileName, ".") != 0) &&
				(strcmp(DirData.cFileName, "..") != 0) &&
				(strcmp(DirData.cFileName, ".svn") != 0)) 
			{
				temp = node->Nodes->Add(gcnew String(DirData.cFileName));

				tstring strDir =  Path+DirData.cFileName+"/";
				SetMapObjTreeView(strDir, temp, node, Path);
			}
		}
		else if (DirData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) 
		{
			string ext = GetExtName(DirData.cFileName);

			if(strcmp(ext.c_str(), ".elu") == 0)
			{
				TreeNode^ newNode = temp->Nodes->Add(gcnew String(DirData.cFileName));

				tstring strDir = subDir;
				if (temp != node)	strDir = subDir+DirData.cFileName;
				newNode->Tag = gcnew String(strDir.c_str());
			}
		}
		bRet = FindNextFile(hDir, &DirData);
	}
}