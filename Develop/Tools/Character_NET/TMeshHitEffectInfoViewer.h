#ifndef _TMESH_HIT_EFFECT_INFO_VIEWER_H
#define _TMESH_HIT_EFFECT_INFO_VIEWER_H

#include <Windows.h>
#include <MMSystem.h>
#include <shlwapi.h>
#include <math.h>
#include <algorithm>

#include "MainApp.h"
#include "TMeshInfo.h"

using namespace System;
using namespace System::Windows::Forms;

class TMeshHitEffectInfoViewer
{
private:
	string				m_strModelName;

private:
	void				InitMeshHitEffectInfoViewer();

	TreeView^			GetMeshHitEffectInfoViewerTreeNode();
	PropertyGrid^		GetMeshHitEffectInfoViewerPropertGrid();

	void				RenderEditionCapsule(MCapsule & capsuleInfo, DWORD dwColor);
	void				RenderCapsule(MCapsule & capsuleInfo, DWORD dwColor = 0x2000CCCC);

	void				SetMeshHitEffectCapsuleDataToProperty(CSColCapsule * pMeshColCapsule);
	void				SetPropertyToMeshHitEffectCapsuleData(CSColCapsule * pMeshColCapsule);

	void				CalAnimationRenderCapsule(CSColCapsule * pColCapsule, MCapsule & outCapsule);

public:
	TMeshHitEffectInfoViewer() {	}
	virtual ~TMeshHitEffectInfoViewer() {}

	void				ChangeMeshHitEffectInfoViewer(System::String^ ModelName);
	void				ChangeMeshHitEffectCapsuleInfo(int nCapsuleIndex);

	void				CreateMeshHitEffectInfoViewer(int nCapsuleIndex = -1);
	void				Reload();
	void				RenderGroup(int nSelectCapsuleID = -1, bool bSolo = false);

	void				SetMeshHitEffectCapsuleData(int nCapsuleIndex);

	void				NotifyChanging();

	MCapsule *			GetCurrentSelectCapsule();
	CSColCapsule *		GetMeshHitEffectCapsuleInfo(int nCapsuleIndex);
};


#endif