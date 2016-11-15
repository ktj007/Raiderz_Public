#pragma once

#include "RSceneNodeInstance.h"

namespace rs3 {

class CORETOOL_API RToolSceneNodeInstance : public RSceneNodeInstance
{
public:
	MDeclareRTTI;

protected:
	virtual bool	OnPick(RPICKINFO& pickInfo);
	virtual void	OnPostUpdate();

public:
	RToolSceneNodeInstance();
	virtual ~RToolSceneNodeInstance();

	virtual RSceneNode* Clone(); 	// 복사 생성

	void	ClearForReload();

	static void	ConvertToAsCopy(RSceneNodeInstance* pSceneNode, RSceneNodeList* pList = NULL);

public:
	void	UpdateAABB();
};

}
