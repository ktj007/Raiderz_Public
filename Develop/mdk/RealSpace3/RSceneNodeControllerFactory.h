#pragma once

namespace rs3 {

//RS_API extern const char *RCID_ACTORNODECONSTRAINT;

class RSceneNodeController;

class RSceneNodeControllerFactory
{
public:
	virtual RSceneNodeController* CreateSceneNodeController(const char* _pSzControllerName);
};

}