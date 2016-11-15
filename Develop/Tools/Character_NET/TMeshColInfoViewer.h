#ifndef _TMESH_COL_INFO_VIEWER_H
#define _TMESH_COL_INFO_VIEWER_H

#include <Windows.h>
#include <MMSystem.h>
#include <shlwapi.h>
#include <math.h>
#include <algorithm>

#include "MainApp.h"
#include "TMeshInfo.h"
#include "TMeshColInfoBoxEditor.h"

using namespace System;
using namespace System::Windows::Forms;

class TMeshColInfoViewer
{
private:
	string				m_strModelName;

	TMeshColInfoBoxEditor m_MeshColInfoBoxEditor;

private:
	void				InitMeshColInfoViewer();

	TreeView^			GetMeshColInfoViewerTreeNode();
	PropertyGrid^		GetMeshColInfoViewerPropertGrid();

	void				RenderEditionCapsule(MCapsule & capsuleInfo, DWORD dwColor);
	void				RenderCapsule(MCapsule & capsuleInfo, DWORD dwColor = 0x20ff00ff);

	void				SetMeshColCapsuleDataToProperty(CSColCapsule * pMeshColCapsule);
	void				SetPropertyToMeshColCapsuleData(CSColCapsule * pMeshColCapsule);

	void				SetMeshColBoxDataToProperty(CSColBox * pMeshColBox);
	void				SetPropertyToMeshColBoxData(CSColBox * pMeshColBox);

	void				CalAnimationRenderCapsule(CSColCapsule * pColCapsule, MCapsule & outCapsule);

public:
	TMeshColInfoViewer() {	}
	virtual ~TMeshColInfoViewer() {}

	void				ChangeMeshColInfoViewer(System::String^ ModelName);
	void				ChangeMeshCoInfo(int nColIndex);

	void				CreateMeshColCapsuleInfoViewer(int nCapsuleIndex = -1);
	void				CreateMeshColBoxInfoViewer(int nBoxID = -1);
	void				Reload();
	void				RenderGroup(int nSelectColID = -1);

	void				SetMeshColData(int nColIndex);

	void				NotifyChanging();
	bool				CheckMousePick(int stat, RVector2& pos, RVector2 prePos);
	bool				CheckMouseDown(int stat, RVector2& pos, RVector2 prePos);

	MCapsule *			GetCurrentSelectCapsule();
	CSColCapsule *		GetMeshColCapsuleInfo(int nCapsuleIndex);
	int					GetMeshColCapsuleCount();

	CSColBox *			GetMeshColBoxInfo(int nBoxIndex);

};


#endif