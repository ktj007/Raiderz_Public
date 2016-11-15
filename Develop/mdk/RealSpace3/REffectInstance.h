#pragma once

#include "REffectSceneNode.h"

namespace rs3 {

RS_API extern const char *RSID_EFFECTINSTANCE;

/// Effect.scene.xml 로 부터 생성되는 이펙트 인스턴스
class RS_API REffectInstanceSceneNode : public REffectSceneNode
{
public:
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_EFFECTINSTANCE);

	REffectInstanceSceneNode();
	~REffectInstanceSceneNode();

	virtual bool			Create(const char* szFileName, bool bBackground = false);

	void					ClearForReload();
	void					Reload();

	REffectSceneNode*		GetEffectSceneNode();
	const string&			GetFileName() const;

	bool					GetAutoStart() const { return m_bAutoStart; }
	void					SetAutoStart(bool bAutoStart) { m_bAutoStart = bAutoStart; }

	virtual RSceneNode*		Clone();

protected:
	virtual bool			CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void			SaveToXML(MXmlElement &element);

	// 이미 만들어진 이펙트를 로딩해서 만든경우, 이경우 클래스를 분리하는게 나을까 ?
	string					m_strEffectSourceName;
	bool					m_bAutoStart;		///< 로딩후 자동실행 여부
	REffectSceneNode*		m_pEffectSceneNode;	///< 실제 이펙트 노드
};

//////////////////////////////////////////////////////////////////////////
inline REffectSceneNode* REffectInstanceSceneNode::GetEffectSceneNode() { return m_pEffectSceneNode; }
//////////////////////////////////////////////////////////////////////////
inline const string& REffectInstanceSceneNode::GetFileName() const { return m_strEffectSourceName; }


} //namespace rs3
