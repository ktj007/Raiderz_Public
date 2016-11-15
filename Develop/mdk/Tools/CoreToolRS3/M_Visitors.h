#pragma once

//////////////////////////////////////////////////////////////////////////
// utilities - visitors

class CORETOOL_API CVisibleSetter : public rs3::RSceneNodeVisitor
{
public:
	CVisibleSetter(bool bVisible) : m_bVisible(bVisible) {}
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode ) override;
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode ) override {}

	bool m_bVisible;
};

class CORETOOL_API CEffectStarter : public rs3::RSceneNodeVisitor
{
public:
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode ) override;
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode ) override {}
};

class CORETOOL_API CEffectStopper : public rs3::RSceneNodeVisitor
{
public:
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode ) override;
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode ) override {}
};

class CORETOOL_API CEffectInstanceReloader : public rs3::RSceneNodeVisitor
{
public:
	CEffectInstanceReloader(const string& strEffectName ) : m_strEffectName(strEffectName) {}
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode ) override;
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode ) override {}

	string m_strEffectName;
};

class CORETOOL_API CSceneInstanceClearVisitor : public rs3::RSceneNodeVisitor
{
public:
	CSceneInstanceClearVisitor(const string& strSceneName ) : m_strSceneName(strSceneName) {}
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode ) override;
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode ) override {}

	string m_strSceneName;
};

class CORETOOL_API CSceneInstanceReloader : public rs3::RSceneNodeVisitor
{
public:
	CSceneInstanceReloader(const string& strSceneName ) : m_strSceneName(strSceneName) {}
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode ) override;
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode ) override {}

	string m_strSceneName;
};

class CORETOOL_API CEditableNodeCollector : public rs3::RSceneNodeVisitor
{
public:
	virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode );
	virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode );
public:
	std::vector<rs3::RSceneNode*>	m_SceneNodes;
};