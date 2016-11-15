#pragma once

#include "WS_WorkSpaceManager.h"

class CorePlugin : CPluginInterface
{
public:
	class CWorkSpaceDefault : public CWorkSpaceImpl<CWorkSpaceDefault>
	{
	public:
		static const char*			DEFAULT_WORKSPACE_NAME;
		virtual const char*			GetWorkSpaceKeyword(){ return DEFAULT_WORKSPACE_NAME; }

		virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);
		virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction){}

		virtual const char*			GetFileExtension()	{ return NULL; }
		virtual const char*			GetFileExtensionDesc()	{ return NULL; }

		virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName){ return MAP_EDIT_TYPE_ERROR; }
		virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs) { return "Save ERROR"; }
		virtual void				OnCloseFile() {}

		virtual void				OnBetweenCoreToolUpdateAndRS3Render(){}
		virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy){}

		virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message){}
		virtual void				OnWBDevAction(){}
		virtual void				OnWBHelperRender(){}
	};

	bool Initialize( CWorkSpaceManager* pWorkSpaceManager ) override;

	void Finalize( CWorkSpaceManager* pWorkSpaceManager ) override;

	HINSTANCE GetInstanceHandle() override;
};