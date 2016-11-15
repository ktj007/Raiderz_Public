#include "stdafx.h"
#include "Global.h"
#include "Util.h"

//파일이름만
string GetPureFileName(const string& szFileName)
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szFileName.c_str(),drive,dir,fname,ext);
	return string(fname);
}

//폴더네임.
string GetFolderName(const string& szFileName)
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szFileName.c_str(),drive,dir,fname,ext);

	string dirpath(dir);
	string dirpath2 = dirpath.substr(0, dirpath.size()-1);

	return GetPureFileName(dirpath2);
}

//파일이름+확장자
string GetRemovePathName(const string& strPathName)
{
	int nPos = strPathName.find_last_of("/\\");
	if(nPos != string::npos)
		return strPathName.substr(nPos+1);
	return strPathName;
}

//. 앞부분을 넘겨줌.
string GetRemoveDotlast(const string& strPathName)
{
	int nPos = strPathName.find_first_of(".");
	if(nPos != string::npos)
		return strPathName.substr(0, nPos);
	return strPathName;
}

string GetFormatString(string& strPathName, const char *pFormat, ...)
{
	char temp[2048];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	strPathName = temp;

	return strPathName;
}

TreeNode^ FindTreeNode(TreeNodeCollection^ nodes, System::String^ str)
{
	TreeNode^ tn = nullptr;
	System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(nodes))->GetEnumerator();
	while ( myNodes->MoveNext() )
	{
		tn = safe_cast<TreeNode^>(myNodes->Current);
		System::String^ tnstr = (safe_cast<TreeNode^>(myNodes->Current))->Text;
		if( System::String::Compare(tnstr, str) == 0)
		{
			return tn;
		}
		else
		{
			tn = FindTreeNode( tn->Nodes, str);
			if( tn != nullptr )
				return tn;
		}
	}

	return tn;
}

TreeNode^ FindTreeNode(TreeNodeCollection^ nodes, RSceneNode* pSceneNode)
{
	TreeNode^ tn = nullptr;
	System::Collections::IEnumerator^ myNodes = (safe_cast<System::Collections::IEnumerable^>(nodes))->GetEnumerator();
	while ( myNodes->MoveNext() )
	{
		tn = safe_cast<TreeNode^>(myNodes->Current);
		RSceneNode* pTreeNodeSceneNode = GetSceneNodeFromTreeNode(tn);
		if(pTreeNodeSceneNode == pSceneNode)
			return tn;
		else
		{
			tn = FindTreeNode( tn->Nodes, pSceneNode);
			if( tn != nullptr )
				return tn;
		}
	}

	return tn;
}

// RSceneNode* 를 담기위한 컨테이너
ref class RSceneNodePointer
{
public:
	RSceneNode* m_pSceneNode;

public:
	RSceneNodePointer(RSceneNode* pSceneNode) : m_pSceneNode(pSceneNode) {}
};

void SetSceneNodeToTreeNode(System::Windows::Forms::TreeNode^ node, rs3::RSceneNode* pSceneNode)
{
	node->Tag = gcnew RSceneNodePointer(pSceneNode) ;	// 포인터 연결
}

rs3::RSceneNode* GetSceneNodeFromTreeNode(System::Windows::Forms::TreeNode^ node)
{
	RSceneNodePointer^ sceneNodePointer = (RSceneNodePointer^)node->Tag;
	rs3::RSceneNode* pSceneNode = sceneNodePointer->m_pSceneNode;
	return pSceneNode;
}

