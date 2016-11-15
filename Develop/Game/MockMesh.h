#pragma once

#include "RMesh.h"
#include "RMeshNode.h"
#include "MLocale.h"

using namespace rs3;

class MockMesh : public RMesh
{
private:
	int	m_nAddNodeCounter;
	enum _MeshType
	{
		_PLAYER,
		_NPC,
		_WEAPON,
		_SHIELD
	};
	_MeshType m_nMeshType;
	bool ExistStrToken(const char* szString, char* szToken)
	{
		const char* token = strstr(szString, szToken);
		return (token != NULL);
	}
	void BuildNodeForPlayer()
	{
		AddNodeForTest(L"hair");
		AddNodeForTest(L"face");
		AddNodeForTest(L"chest");
		AddNodeForTest(L"chest_item");
		AddNodeForTest(L"head");
		AddNodeForTest(L"head_item");
		AddNodeForTest(L"hands");
		AddNodeForTest(L"hands_item");
		AddNodeForTest(L"legs");
		AddNodeForTest(L"legs_item");
		AddNodeForTest(L"feet");
		AddNodeForTest(L"feet_item");

		AddDummyNodeForTest(L"dummy_shield");
		AddDummyNodeForTest(L"dummy_shield_b");
		AddDummyNodeForTest(L"dummy_shield_w");

		AddDummyNodeForTest(L"dummy_1hs_l");
		AddDummyNodeForTest(L"dummy_1hb_l");
		AddDummyNodeForTest(L"dummy_1hp_l");
		AddDummyNodeForTest(L"dummy_1hs_r");
		AddDummyNodeForTest(L"dummy_1hb_r");
		AddDummyNodeForTest(L"dummy_1hp_r");
		AddDummyNodeForTest(L"dummy_2hs");

		AddDummyNodeForTest(L"dummy_ef_body");
		AddDummyNodeForTest(L"dummy_1hs_r_b");
		AddDummyNodeForTest(L"dummy_1hb_r_b");
		AddDummyNodeForTest(L"dummy_1hp_r_b");
		AddDummyNodeForTest(L"dummy_1hs_l_b");
		AddDummyNodeForTest(L"dummy_1hb_l_b");
		AddDummyNodeForTest(L"dummy_1hp_l_b");
		AddDummyNodeForTest(L"dummy_2hs_b");

		AddDummyNodeForTest(L"dummy_1hs_r_w");
		AddDummyNodeForTest(L"dummy_1hb_r_w");
		AddDummyNodeForTest(L"dummy_1hp_r_w");
		AddDummyNodeForTest(L"dummy_1hs_l_w");
		AddDummyNodeForTest(L"dummy_1hb_l_w");
		AddDummyNodeForTest(L"dummy_1hp_l_w");
		AddDummyNodeForTest(L"dummy_2hs_w");

		AddDummyNodeForTest(L"dummy_loc");
	}
	void BuildNodeForNPC()
	{
		BuildNodeForPlayer();
	}
	void BuildNodeForWeapon()
	{
		AddNodeForTest(L"1hs_01");
		AddNodeForTest(L"1hs_02");
		AddNodeForTest(L"1hs_03");

		AddNodeForTest(L"1hd_01");
		AddNodeForTest(L"1hd_02");
		AddNodeForTest(L"1hd_03");

		AddNodeForTest(L"1hb_01");
		AddNodeForTest(L"1hb_02");
		AddNodeForTest(L"1hb_03");

		AddNodeForTest(L"2hs_01");
		AddNodeForTest(L"2hs_02");
		AddNodeForTest(L"2hs_03");

		AddNodeForTest(SWORDTRAIL_START_NODE_NAME);
		AddNodeForTest(SWORDTRAIL_END_NODE_NAME);
	}
	void BuildNodeForShield()
	{
		AddNodeForTest(L"shield_01");
		AddNodeForTest(L"shield_02");
		AddNodeForTest(L"shield_03");
	}
public:
	MockMesh(const char* _pSzResourceName, RSceneManager* _pSceneManager=NULL) 
		: RMesh(_pSzResourceName, _pSceneManager), m_nAddNodeCounter(0)
	{
		AddRef();

		if (ExistStrToken(_pSzResourceName, "shield"))
		{
			m_nMeshType = _SHIELD;
		}
		else if (ExistStrToken(_pSzResourceName, "1hd"))
		{
			m_nMeshType = _WEAPON;
		}
		else if (ExistStrToken(_pSzResourceName, "1hs"))
		{
			m_nMeshType = _WEAPON;
		}
		else if (ExistStrToken(_pSzResourceName, "2hd"))
		{
			m_nMeshType = _WEAPON;
		}
		else if (ExistStrToken(_pSzResourceName, "2hb"))
		{
			m_nMeshType = _WEAPON;
		}
		else if (ExistStrToken(_pSzResourceName, "1hp"))
		{
			m_nMeshType = _WEAPON;
		}
		else if (ExistStrToken(_pSzResourceName, "1hp"))
		{
			m_nMeshType = _WEAPON;
		}
		else if (ExistStrToken(_pSzResourceName, "hf"))
		{
			m_nMeshType = _PLAYER;
		}
		else if (ExistStrToken(_pSzResourceName, "hm"))
		{
			m_nMeshType = _PLAYER;
		}
		else
		{
			m_nMeshType = _NPC;
		}
	}
	virtual ~MockMesh() {}

	virtual int Release()
	{
		--m_nRefCount;

		return m_nRefCount;
	}

	virtual bool BuildAll( bool bBackground = false, LoadingController* pController = NULL )
	{
		BuildNodes();

		ForceSetState(RRS_LOADED);
		return true;
	}

	void BuildNodes()
	{
		switch (m_nMeshType)
		{
		case _PLAYER:	BuildNodeForPlayer(); break;
		case _NPC:		BuildNodeForNPC(); break;
		case _WEAPON:	BuildNodeForWeapon(); break;
		case _SHIELD:	BuildNodeForShield(); break;
		}
	}

	void AddNodeForTest(const wchar_t* szNodeName, bool bDummy = false)
	{
		if (GetNode(MLocale::ConvUTF16ToAnsi(szNodeName).c_str()) != NULL) return;

		m_nAddNodeCounter++;

		RMeshNode* pMeshNode = new RMeshNodeMLeaf();
		pMeshNode->m_nID = m_nAddNodeCounter;
		pMeshNode->m_pParentMesh = this;
		pMeshNode->m_pBaseMesh = this;
		pMeshNode->SetName(MLocale::ConvUTF16ToAnsi(szNodeName).c_str());

		if(bDummy)
			pMeshNode->SetFlag(RM_FLAG_DUMMY_MESH);

		PushNode(pMeshNode);
	}

	void AddDummyNodeForTest(const wchar_t* szNodeName)
	{
		AddNodeForTest( szNodeName, true);
	}
};