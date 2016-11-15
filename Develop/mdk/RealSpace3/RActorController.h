#pragma once

#include "RSceneNodeController.h"

namespace rs3 {

class RActor;

class RS_API RActorController : public RSceneNodeController
{
	friend RActor;

public:
	RActorController(RSNCTRL_TYPE eType);
	virtual ~RActorController();

	virtual void	OnAdd() override;
	virtual void	OnRemove() override;

	virtual bool	Create() { return true; }

	void			RefreshAABB();
	void			UpdateActorForRender();
	void			SetAutoUpdate(bool bEnable);

protected:
	RActor*			m_pActor;
};

}


