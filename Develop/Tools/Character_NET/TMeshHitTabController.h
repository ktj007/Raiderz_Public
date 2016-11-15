#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>

#include "TTalentInfo.h"

class TMeshColInfoViewer;
class TPartsHitInfoViewer;
class TMeshHitEffectInfoViewer;

class TMeshHitTabController
{
private:
	TMeshColInfoViewer* m_pMeshColInfoViewer;
	TPartsHitInfoViewer* m_pPartsHitInfoViewControl;
	TMeshHitEffectInfoViewer * m_pMeshHitEffectInfoViewer;
public:
	TMeshHitTabController();
	~TMeshHitTabController();
	void Init(TMeshColInfoViewer* pMeshColInfoViewer, TPartsHitInfoViewer* pPartsHitInfoViewControl, TMeshHitEffectInfoViewer* pMeshHitEffectInfoViewer);

	void Serialize();
	void CopyColCapsule();
	void PasteColCapsule();
	void CopyHitCapsule();
	void PasteHitCapsule();
	void CopyHitEffectCapsule();
	void PasteHitEffectCapsule();

	// Æû ÀÌº¥Æ®
	void DelCollision_Click(System::Object^  sender, System::EventArgs^  e);
	void AddCollisionCapsule_Click(System::Object^  sender, System::EventArgs^  e);
	void AddCollisionBox_Click(System::Object^  sender, System::EventArgs^  e);

	void DelHitEffectCapsule_Click(System::Object^  sender, System::EventArgs^  e);
	void AddHitEffectCapsule_Click(System::Object^  sender, System::EventArgs^  e);

	void DelPartsCapsule_Click(System::Object^  sender, System::EventArgs^  e) ;


	void RefreshMeshHitColInfo();
};



