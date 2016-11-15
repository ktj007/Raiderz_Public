#pragma once

#include "MFramework.h"

using namespace System;
using namespace rs3;

namespace RSManaged
{
	public __gc class MSceneNode
	{
	public:
		MSceneNode();
		MSceneNode(RSceneNode* pRealSceneNode);
		~MSceneNode();

		bool AddChild(MSceneNode* pChild);
		bool RemoveChild(MSceneNode* pChild);
		bool RemoveFromParent();

		//void SetHierachy(bool bUse);

		//const RMatrix& GetLocalTransform();
		//const RMatrix& GetTransform(bool bRecalc = false);		// hierachy 를 사용하지 않으면 world = local 이다.
		//void SetTransform(const RMatrix& tm);
		//void SetTransform(const RVector& pos, const RVector& dir, const RVector& up=RVector(0,0,1));
		
		//void SetPosition(const RVector& pos);
		//void SetDirection(const RVector& dir, const RVector& up=RVector(0,0,1));
		void UpdateTransform();

		//void SetVisible(bool bFlag)		{	m_bVisible = bFlag;		}
		//bool GetVisible()				{	return m_bVisible;		}

		//const RVector& GetPosition();
		//const RVector& GetDirection();
		//const RVector& GetUp();

		// 충돌및 picking 에 관한 것들
//		virtual bool Pick(const RVector &origin,const RVector &to,RPICKINFO *pPickInfo) { return false; }

		void Message(System::String* msg, System::String* param);

		__property rs3::RSceneNode* get_NativeSceneNode();
		__property bool get_Visible();
		__property void set_Visible(bool bVisible);
		__property bool get_Valid();
	protected:
		rs3::RSceneNode* m_pSN;
	};
}
