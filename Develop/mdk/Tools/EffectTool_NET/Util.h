#pragma once

#include <string>

#include "RActorNodeConstraintController.h"

//유니코드용
namespace std
{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

using namespace std;

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)
#define MStringToCharPointer(stringname) \
	(const char*) System::Runtime::InteropServices::Marshal \
	::StringToHGlobalAnsi(stringname).ToPointer()

// This macro frees a pointer that was allocated with MStringToCharPointer.
#define MFreeCharPointer(stringname) \
	System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*) \
	stringname))

string GetPureFileName(const string& strFileName);
string GetRemovePathName(const string& strPathName);
string GetFolderName(const string& szFileName);
string GetRemoveDotlast(const string& strPathName);
string GetFormatString(string& strPathName, const char *pFormat, ...);

// tree control 관련 helper, 파일로 빼도 좋을듯
using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace rs3;

TreeNode^	FindTreeNode(TreeNodeCollection^ nodes, System::String^ str);
TreeNode^	FindTreeNode(TreeNodeCollection^ nodes, RSceneNode* pSceneNode);

// tree node tag 에 RSceneNode* binding
void		SetSceneNodeToTreeNode(TreeNode^ node, RSceneNode* pSceneNode);
RSceneNode*	GetSceneNodeFromTreeNode(TreeNode^ node);

// RSceneNode 의 property 를 설정하기 위한 클래스
public ref class RSceneNodeProperty
{
private:
	RSceneNode*	m_pSceneNode;
public:
	RSceneNodeProperty(RSceneNode* pSceneNode) : m_pSceneNode(pSceneNode) {}

	[CategoryAttribute("SceneNode"), DescriptionAttribute("Name of SceneNode")]
	property String^ Name
	{
		String ^ get() { return gcnew String( m_pSceneNode->GetNodeName().c_str() ); }
		void set(String ^ value) 
		{
			const char* pcStr = MStringToCharPointer(value);
			m_pSceneNode->SetNodeName( pcStr );
			MFreeCharPointer(pcStr);
		}
	};

	[CategoryAttribute("Controllers"), DescriptionAttribute("ActorNode Constraint")]
	property String^ ActorNode
	{
		String ^ get() { 
			RSCENENODECONTROLLERLIST* pControllers = m_pSceneNode->GetControllers( RSNCTRL_PREUPDATE );

			for( RSCENENODECONTROLLERLIST::iterator it = pControllers->begin(); it != pControllers->end(); ++it )
			{
				RSceneNodeController* pController = *it;
				RActorNodeConstraintController* pActorNodeConstraintController = dynamic_cast<RActorNodeConstraintController*> (pController);
				if(pActorNodeConstraintController)
				{
					const RActorNode* pTarget = pActorNodeConstraintController->GetTargetActorNode();
					if(pTarget)
						return gcnew String( pTarget->GetNodeName().c_str() );
				}
			}
			return nullptr; 
		}
	};
};


