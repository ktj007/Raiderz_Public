/// @file WS_Script.h
#pragma once

#include "C_RubyManager.h"
#include "WS_WorkSpaceManager.h"
#include <string>
#include <map>

class CWorkSpaceScript : public CWorkSpaceImpl<CWorkSpaceScript>
{
public:
								CWorkSpaceScript();
	virtual						~CWorkSpaceScript();

protected:

	virtual const char*			GetWorkSpaceKeyword(){ return "script.xml"; }

	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction){}

	virtual const char*			GetFileExtension()	{ return "*.script.xml"; }
	virtual const char*			GetFileExtensionDesc()	{ return "스크립트 파일"; }

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
