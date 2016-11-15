#ifndef _XCUTSCENE_WORLD_RESOURCE_MANAGER_H
#define _XCUTSCENE_WORLD_RESOURCE_MANAGER_H

#pragma once

class XWorld;

class XCutSceneWorldResourceMgr
{
private:
	XWorld*				m_pCutSceneWorld;

public:
	XCutSceneWorldResourceMgr();
	virtual ~XCutSceneWorldResourceMgr();
};

#endif