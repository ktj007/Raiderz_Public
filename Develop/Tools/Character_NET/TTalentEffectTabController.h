#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>

#include "TTalentInfo.h"

class TTalentEffectViewer;
class TTalentEffectMgr;

class TTalentEffectTabController
{
private:
	TTalentEffectMgr* m_pTalentEffectMgr;
	TTalentEffectViewer* m_pTalentEffectViewer;
	string m_strSelEffectName;
	string m_strSelHitEffectPosDir;

	void UpdateTreeView();
	void UpdateTalentEffectTreeView(XTalentEffectDataMgr* pTalentEffectData);
	void UpdateHitEffectPosDirTreeView(XTalentEffectDataMgr* pTalentEffectData);

	void UpdatePropertyGrid();
	void UpdateTalentEffectPropertyGrid(XTalentEffectDataMgr* pTalentEffectData);
	void UpdateHitEffectPosDirPropertyGrid(XTalentEffectDataMgr* pTalentEffectData);
	
	void OnTalentEffectAttGrid_PropertyValueChanged(XTalentEffectDataMgr* pTalentEffectData);
	void OnHitEffectPosDirAttGrid_PropertyValueChanged(XTalentEffectDataMgr* pTalentEffectData);

	void UpdateReferenceControls();
	void UpdateDereferenceControls();

	string GetNewEffectInfoName(TTalentInfo* pTalentInfo, string strEffectName, int nCount = 0);
public:
	TTalentEffectTabController();
	~TTalentEffectTabController();
	void Init(TTalentEffectMgr* pTalentEffectMgr, TTalentEffectViewer* pTalentEffectViewer);

	void Serialize();

	bool CreateTalentEffectInfo(System::String^ NewEffectInfoName);
	bool DeleteTalentEffectInfo(System::String^ DelEffectInfoName);
	XTalentEffectInfo* GetTalentEffectInfo(System::String^ EffectInfoName);
	bool SetTalentEffectReference( int nRefID, int nRefMode );
	void CopyEffectInfo();
	void PasteEffectInfo();

	//////////////////////////////////////////////////////////////////////////
	// 이벤트
	// 공통
	void OnEffectAttGrid_PropertyValueChanged();

	// 탤런트 이펙트
	void OnEffectTreeView_AfterSelect(System::Windows::Forms::TreeViewEventArgs^  e);
	void OnAddEffectInfoBtn_Click();
	void OnDelEffectInfoBtn_Click();

	// 피격 이펙트 위치 방향
	void OnHitEffectPosDirTreeView_AfterSelect(System::Windows::Forms::TreeViewEventArgs^  e);
	void OnEnableHitEffectPosDirBtn_Click();
	void OnDisableHitEffectPosDirBtn_Click();
	void OnDeDereferenceTalentEffectIDViewer();

	void GetTalentEffectImpactList(ArrayList^ impactList);
	void GetTalentEffectTypeList(ArrayList^ effectTypeList);
	void GetTalentEffectDirectionList(ArrayList^ directionList);

	void RenderHitEffectPosDir();
};



