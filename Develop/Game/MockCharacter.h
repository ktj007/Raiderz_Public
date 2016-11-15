#pragma once

#include "XCharacter.h"
#include "MockMesh.h"

class TestCharacterParts : public XCharacterParts
{
protected:
	virtual void ReleaseMesh(RMesh* pMesh);
	virtual RMesh* CreateMesh(const wchar_t* szMeshName);
public:
	TestCharacterParts(XCharacter* pCharacter) : XCharacterParts(pCharacter) {}
};

class MockCharacter : public XCharacter
{
private:
	wstring				m_strDummyLocName;
	vec3				m_vDummyLocPos;
	vec3				m_vDummyLocDir;
	vec3				m_vDummyLocMoveDir;
	float				m_fDummyLocMoveSpeed;

	wstring				m_strMeshName;

	MockMesh*			m_pMockMesh;
	RMesh*				m_pMeshForCol;

public:
	unsigned int		m_colorTex;

protected:
	virtual void OnUpdate();
	void BuildDefaultActorNode( MockMesh* pMockMesh );
	void AddActorNodeForTest(MockMesh* pMockMesh, const wchar_t* szNodeName);
public:
	MockCharacter();
	virtual ~MockCharacter();

	virtual bool		Create(const wchar_t* szMeshName, bool bBackgroundCreation = false);
	virtual void		Destroy();

	void				MockCharacterUpdate(float fDelta);
	void				CreateMeshForCol(const wchar_t* szMeshName);

	// dummy_loc를 테스트 하기 위한 함수들...
	virtual bool		IsExistActorNode(wstring strDummyName);
	virtual bool		GetMovingDummyPosition(RVector& pos);
	virtual bool		GetMovingDummyDirection(RVector& dir);


	void				SetDummyLocData(wstring strDummyLocName, vec3& vStartPos, vec3& vMoveDir, float vMoveSpeed);
	void				SetDummyLoc(vec3 vPos, vec3 vDir);

	virtual void		SetColor(unsigned int nColor);

	TestCharacterParts* GetCharacterParts() { return dynamic_cast<TestCharacterParts*>(m_pParts); }

	void				AddMeshNodeForTest(const wchar_t* szNodeName);
};