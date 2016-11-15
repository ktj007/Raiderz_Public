#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>

#include "TTalentInfo.h"

class TTalentEventMgr;
class TTalentEventViewer;
class CSTalentEventInfo;

namespace Character_NET {

using namespace System;
using namespace System::Windows::Forms;

ref class EventTabPage;

public ref class TTalentEventController
{
private:
	EventTabPage^		m_pMainForm;
	TTalentEventMgr* m_pTalentEventMgr;
	TTalentEventViewer*	m_pEventViewControl;

	bool SetTalentEventView( CSTalentInfo* pTalentInfo );
	TreeView^ GetEventViewerTreeNode();

	void SetTalentEventReference(int nRefID, int nRefMode);

	void UpdateReferenceControls();
	void UpdateDereferenceControls();

	void CalProjectileStartPos(CSTalentEventInfo * pTalentEventInfo, MMatrix& matCalWorld);
	void CalProjectilHitCapsuleCheckTime( CSTalentEventInfo * pTalentEventInfo, MMatrix matWorld );

public:
	TTalentEventController(EventTabPage^ pForm);
	~TTalentEventController();
	void Init(TTalentEventMgr* pTalentEventMgr);

	void Clear();
	void SetTalentEvent(CSTalentInfo* pTalentInfo);
	TreeNode^ AddTalentEventView(int nStart);

	void CreateTalentEvent( int nStart );
	void DeleteTalentEvent();

	int GetCurrentTalentEventType();
	void SetCurrentPropertyTalentEvent(bool bNewData);

	void CheckSegmentIndexCountAndCapsuleIndexCount();

	bool IsExistenceTalentEvent();

	void OnTalentEventCopy();
	void OnTalentEventPasete();
	void OnTalentEventRefSelect(System::Object^  sender, System::EventArgs^  e);
	void OnDeDereferenceTalentEffectIDViewer();

	void GetTalentHitInfoSegmentIndex(ArrayList^ indexList);
	void GetTalentHitInfoCapsuleIndex(ArrayList^ indexList, int nIndex);
	void GetProjectileType(ArrayList^ typeList);
	void GetProjectileVisualType(ArrayList^ typeList);
	void GetTalentEventTypeList(ArrayList^ eventList);
};

}