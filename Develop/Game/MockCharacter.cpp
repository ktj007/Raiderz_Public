#include "stdafx.h"
#include "MockCharacter.h"
#include "XEngine.h"

MockCharacter::MockCharacter() : m_pMockMesh(NULL), m_pMeshForCol(NULL)
{
	m_strDummyLocName.clear();
	m_vDummyLocPos.Set(0.0f, 0.0f, 0.0f);
	m_vDummyLocDir.Set(0.0f, -1.0f, 0.0f);
	m_vDummyLocMoveDir.Set(0.0f, 0.0f, 0.0f);
	m_fDummyLocMoveSpeed = 0.0f;
	m_colorTex = 0;

	XCharacter::XCharacter();

	m_bFakeResourceLoading = true;

	SAFE_DELETE(m_pParts);
	m_pParts = new TestCharacterParts(this);
}

bool MockCharacter::IsExistActorNode( wstring strDummyName )
{
	return (strDummyName == m_strDummyLocName);
}

bool MockCharacter::GetMovingDummyPosition(RVector& pos)
{
	if(IsExistActorNode(L"dummy_loc") == false)
		return false;

	pos = m_vDummyLocPos;

	return true;
}

bool MockCharacter::GetMovingDummyDirection( RVector& dir )
{
	if(IsExistActorNode(L"dummy_loc") == false)
		return false;

	dir = m_vDummyLocDir;

	return true;
}

void MockCharacter::SetDummyLoc(vec3 vPos, vec3 vDir)
{
	m_strDummyLocName = L"dummy_loc";
	m_vDummyLocPos = vPos;
	m_vDummyLocDir = vDir;
}

void MockCharacter::SetDummyLocData( wstring strDummyLocName, vec3& vStartPos, vec3& vMoveDir, float vMoveSpeed )
{
	m_strDummyLocName		= strDummyLocName;
	m_vDummyLocPos			= vStartPos;
	m_vDummyLocMoveDir		= vMoveDir;
	m_fDummyLocMoveSpeed	= vMoveSpeed;
}

void MockCharacter::MockCharacterUpdate(float fDelta)
{
	if(m_fDummyLocMoveSpeed > 0 &&
		m_strDummyLocName.size() > 0)
	{
		m_vDummyLocPos += m_vDummyLocMoveDir * (m_fDummyLocMoveSpeed * fDelta);
	}
}

bool MockCharacter::Create( const wchar_t* szMeshName, bool bBackgroundCreation /*= false*/ )
{
	m_pMockMesh = new MockMesh(MLocale::ConvUTF16ToAnsi(szMeshName).c_str());
	m_strMeshName = szMeshName;
	m_pMesh = m_pMockMesh;

	m_pMockMesh->BuildNodes();
	BuildDefaultActorNode(m_pMockMesh);


	SetUsable(true);

	return true;
}

void MockCharacter::Destroy()
{
	XCharacter::Destroy();
	if (m_pMockMesh)
	{
		SAFE_DELETE(m_pMockMesh);
	}

	if (m_pMeshForCol)
	{
		SAFE_DELETE(m_pMeshForCol);
	}

	m_pMesh = NULL;
}

void MockCharacter::OnUpdate()
{
	XCharacter::OnUpdate();

	REngine::GetDevice().Flip();
}

void MockCharacter::BuildDefaultActorNode( MockMesh* pMockMesh )
{
	AddActorNodeForTest(pMockMesh, L"dummy_shield");
	AddActorNodeForTest(pMockMesh, L"dummy_shield_b");
	AddActorNodeForTest(pMockMesh, L"dummy_shield_w");

	AddActorNodeForTest(pMockMesh, L"dummy_1hs_l");
	AddActorNodeForTest(pMockMesh, L"dummy_1hb_l");
	AddActorNodeForTest(pMockMesh, L"dummy_1hp_l");
	AddActorNodeForTest(pMockMesh, L"dummy_1hs_r");
	AddActorNodeForTest(pMockMesh, L"dummy_1hb_r");
	AddActorNodeForTest(pMockMesh, L"dummy_1hp_r");
	AddActorNodeForTest(pMockMesh, L"dummy_2hs");

	AddActorNodeForTest(pMockMesh, L"dummy_ef_body");
	AddActorNodeForTest(pMockMesh, L"dummy_1hs_r_b");
	AddActorNodeForTest(pMockMesh, L"dummy_1hb_r_b");
	AddActorNodeForTest(pMockMesh, L"dummy_1hp_r_b");
	AddActorNodeForTest(pMockMesh, L"dummy_1hs_l_b");
	AddActorNodeForTest(pMockMesh, L"dummy_1hb_l_b");
	AddActorNodeForTest(pMockMesh, L"dummy_1hp_l_b");
	AddActorNodeForTest(pMockMesh, L"dummy_2hs_b");

	AddActorNodeForTest(pMockMesh, L"dummy_1hs_r_w");
	AddActorNodeForTest(pMockMesh, L"dummy_1hb_r_w");
	AddActorNodeForTest(pMockMesh, L"dummy_1hp_r_w");
	AddActorNodeForTest(pMockMesh, L"dummy_1hs_l_w");
	AddActorNodeForTest(pMockMesh, L"dummy_1hb_l_w");
	AddActorNodeForTest(pMockMesh, L"dummy_1hp_l_w");
	AddActorNodeForTest(pMockMesh, L"dummy_2hs_w");

	AddActorNodeForTest(pMockMesh, L"dummy_loc");

}

void MockCharacter::AddActorNodeForTest( MockMesh* pMockMesh, const wchar_t* szNodeName )
{
	RMeshNode* pMeshNode = pMockMesh->GetNode(MLocale::ConvUTF16ToAnsi(szNodeName).c_str());
	if (pMeshNode == NULL) return;
	AddMeshNode(pMeshNode, NULL, MLocale::ConvUTF16ToAnsi(szNodeName).c_str(), false);
}

void MockCharacter::AddMeshNodeForTest( const wchar_t* szNodeName )
{
	if(m_pMockMesh)
	{
		m_pMockMesh->AddNodeForTest(szNodeName);
		AddActorNodeForTest(m_pMockMesh, szNodeName);
	}
}

MockCharacter::~MockCharacter()
{
	Destroy();
}

void MockCharacter::CreateMeshForCol( const wchar_t* szMeshName )
{
	m_pMeshForCol = new RMesh(MLocale::ConvUTF16ToAnsi(szMeshName).c_str(), NULL);

	m_pMesh = m_pMeshForCol;
}

void MockCharacter::SetColor( unsigned int nColor )
{
	m_colorTex = nColor;
}

void TestCharacterParts::ReleaseMesh( RMesh* pMesh )
{
	global.engine->ReleaseMesh(pMesh);
}

RMesh* TestCharacterParts::CreateMesh( const wchar_t* szMeshName )
{
	return global.engine->CreateMesh(szMeshName);
}