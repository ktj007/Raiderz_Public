#pragma once

#include "RActor.h"
//class RToolMeshMgr;

namespace rs3 {

class CORETOOL_API RToolActor : public RActor
{
	struct SPointLightSelector
	{
		static bool Selector( RSceneNode* pSceneNode )
		{
			if ( _stricmp( pSceneNode->GetNodeIDString(), RSID_LIGHTSCENENODE ) == 0 )
				return true;
			else
				return false;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 이팩트 씬노드 Selector
	// by pok. 옛 유물같은데.. 제가 만든것 같기도 하고..-_- 없앨수 있으면 없애는게 좋을것 같습니다.
	// 09.02.09
	//
	struct EffectSelector
	{
		static bool Selector( RSceneNode* pSceneNode )
		{
			if ( _stricmp(pSceneNode->GetNodeIDString(), RSID_EFFECT ) == 0 )
				return true;
			return false;
		}
	};


public:
	RToolActor(CControlMediator* pControlMediator);
	virtual ~RToolActor();

	int GetPartsNum();
	void GetPartsMatrix(int nIndex, RMatrix *pMat);
	void DrawParts(int nIndex);
	void SetCreationType(bool bEluInstance);

	void SetTexColor(DWORD _dwColor);

	virtual RSceneNode* Clone(); 	// 복사 생성
	virtual void CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const;

	void	SetCullState( bool _bSet = false );

public:
	bool			m_bEluInstanceType;

protected:
	// 디바이스 의존적인 부분 생성시에 툴에서 필요한 정보 - 가지고 있는 포인트라이트의 이름이라든지 - 를 추가한다.
	virtual void CreateDeviceDependentPart(RMesh* pMesh);

	// 포인트 라이트의 이름을 설정
	void SetPointLightName(const char* _pSzDelim);

	virtual void	OnAdd();
	virtual void	OnRemove();

	std::vector< RSceneNode* >	m_vecEffectSceneNode;
	CControlMediator*	m_pControlMediator;
};

}
