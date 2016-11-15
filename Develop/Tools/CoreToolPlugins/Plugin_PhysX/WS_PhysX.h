/// @file WS_PhysX.h
#pragma once

#include "WS_WorkSpaceManager.h"

class CPhysXObject;
class CCollisionTree;
namespace physx{ class MPxObjStatic; }

class CWorkSpacePhysX : public CWorkSpaceImpl<CWorkSpacePhysX>
{
public:
								CWorkSpacePhysX();
	virtual						~CWorkSpacePhysX();

	void						InitMPhysX();
	void						HaltMPhysX();

	// NOTE : XZone
	void						InitStaticObject();
	void						HaltStaticObject();

	void						LoadPxObject(const char* _pSzMeshName);
	void						UnLoadPxObject();

	void						StartBPart(int _nIdx);

protected:

	virtual const char*			GetWorkSpaceKeyword(){ return "physx.xml"; }
	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction);

	virtual const char*			GetFileExtension()	{ return "*.physx.xml"; }
	virtual const char*			GetFileExtensionDesc()	{ return "MPhysX"; }

	virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName);
	virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs);
	virtual void				OnCloseFile() {}

	virtual void				OnBetweenCoreToolUpdateAndRS3Render();
	virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy);

	virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message){}
	virtual bool				OnCheckAlwaysPickingWhenMoused(){ return false; }

	virtual void				OnWBDevAction();
	virtual void				OnWBHelperRender();

private:
	DWORD						m_dwOldCameraColor;
	CPhysXObject*				m_pCurrentPxObject;

	CCollisionTree*				m_pCollisionTree;
	physx::MPxObjStatic*		m_pPhysXStatic;
};
