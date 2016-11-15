#pragma once

#include "RSceneNode.h"
#include "RSceneNodeID.h"

namespace rs3 {

class RS_API RSceneNodeInstance : public RSceneNode
{
public:
	MDeclareRTTI;

	DECLARE_NODE_ID_STRING(RSID_SCENEINSTANCE);

public:
	RSceneNodeInstance();
	virtual ~RSceneNodeInstance();

	bool Create(const char* szFileName, bool bBackgroundCreation);	///< 실제 파일을 읽어들인다

	virtual bool CreateFromXML( MXmlElement &element, bool bBackgroundCreation ) override;
	virtual void SaveToXML( MXmlElement &element ) override;

	const string& GetFileName() const;

protected:

	// Path 관련
	string	m_strFileName;		// 실제 Scene 파일의 FullPath // NodeName은 씬이름만 가진다.(ex test.scene.xml)
};

//////////////////////////////////////////////////////////////////////////
inline const string& RSceneNodeInstance::GetFileName() const { return m_strFileName; }

}