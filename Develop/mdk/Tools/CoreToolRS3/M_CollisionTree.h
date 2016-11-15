#pragma once

#include "RCollisionTree.h"
#include "RSceneNode.h"

namespace rs3 
{
	class RDevice;
	class RSceneNode;
	class RCollisionTreeBuilder;
};

class RHeightField;

// collision collector
class CORETOOL_API SceneNodeCollisionCollector : public rs3::RSceneNodeConstVisitor
{
public:
	SceneNodeCollisionCollector(rs3::RCollisionTreeBuilder* pBuilder) : m_pBuilder(pBuilder) {}
	virtual ~SceneNodeCollisionCollector(){}

	virtual bool OnPreOrderVisit( const rs3::RSceneNode* pSceneNode );
	virtual void OnPostOrderVisit( const rs3::RSceneNode* pSceneNode ) {}

private:
	rs3::RCollisionTreeBuilder* m_pBuilder;
};

class CORETOOL_API CCollisionTree
{
private:
	rs3::RCollisionTree *m_pCollisionTree;
	bool	m_bRender;	// 그릴것인가

public:
	CCollisionTree();
	virtual ~CCollisionTree();

	rs3::RCollisionTree* GetCollisionTree() { return m_pCollisionTree; }

	void SetRender(bool bRender) { m_bRender = bRender; }

	bool Load(const char *pcCl2Filename);
	void Render(rs3::RDevice *pDevice);
	void Destroy();
};