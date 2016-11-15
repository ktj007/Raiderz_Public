#pragma once

class GEntityNPC;

/// GEntityNPC의 디버그 정보는 이 클래스에서 관리한다.
class GNPCDebugger : public MTestMemPool<GNPCDebugger>
{
private:
	GEntityNPC*		m_pEntityNPC;
public:
	GNPCDebugger(GEntityNPC* pNPC) : m_pEntityNPC(pNPC) {}
	void Logf(const wchar_t* szText, ...);
	void Log( const wchar_t* szText );
	wstring GetCurrentMotionfactorName();
	wstring GetCurrentFSM();
	wstring GetCurrentJobName();
	wstring GetCurrentTaskListName();
	wstring GetPathFindingString();
	wstring GetMotionFactorString();
	wstring GetCurTalentPhaseString();
	int GetStress();
	int GetRage();
	int GetLodLevel(void) const;
	int GetSightRange() const;
	bool IsAutoAssist() const;
	float GetAutoAssistRange() const;
	wstring GetAAString();
	wstring GetAttackableString();
	vector<string> GetTimerStrings();
};