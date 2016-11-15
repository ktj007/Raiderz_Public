/// @file WS_RzDev.h
#pragma once

#include "WS_WorkSpaceManager.h"

class CWorkSpaceRzDev : public CWorkSpaceImpl<CWorkSpaceRzDev>
{
public:
								CWorkSpaceRzDev(){}
	virtual						~CWorkSpaceRzDev(){}

protected:

	virtual const char*			GetWorkSpaceKeyword(){ return "rz_dev.xml"; }
	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction);

	virtual const char*			GetFileExtension()	{ return "*.rz_dev.xml"; }
	virtual const char*			GetFileExtensionDesc()	{ return "레이더즈 개발용"; }

	virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName);
	virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs);
	virtual void				OnCloseFile() {}

	virtual void				OnBetweenCoreToolUpdateAndRS3Render();
	virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy);

	virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message){}
	virtual bool				OnCheckAlwaysPickingWhenMoused(){ return false; }

	virtual void				OnWBDevAction();
	virtual void				OnWBHelperRender();
};
