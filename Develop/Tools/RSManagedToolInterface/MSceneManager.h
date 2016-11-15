#pragma once

#include "MFramework.h"
#include "MSceneNode.h"

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MLightSceneNode;
	public __gc class MCameraSceneNode;

	public __gc class MSceneManager : public MSceneNode
	{
	public:
		MSceneManager();
		void Set(RSceneManager* pSM);

		void Clear();
		bool Create();
		void Destroy();

		MCameraSceneNode* SetActiveCamera(MCameraSceneNode *pCamera);
		MCameraSceneNode* GetActiveCamera();

		void SetCurrentPass(RSManaged::RRENDERPASS pass);
		RSManaged::RRENDERPASS	GetCurrentPass();

		virtual void Update();
		virtual void Render();

		// scene 전체 load/save
		bool LoadXML(System::String* szFileName);
		bool SaveXML(System::String* szFileName);

		// 임시
		void AddLight(MLightSceneNode* pLight);
//		void AttachAABBTreeNode(MSceneNode* pAABBTreeNode);

		// scene node factory
		MSceneNode* CreateSceneNode(System::String* szNodeName);
	protected:
		RSceneManager* m_pSM;
		MCameraSceneNode* m_pActiveCamera;
	};
}
