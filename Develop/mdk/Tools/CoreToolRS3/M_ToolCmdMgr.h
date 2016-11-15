#pragma once
#include <deque>
#include <vector>
#include <string>

#include "RSceneNode.h"
#include "RTypes.h"

//command buffer
class ICommandBuffer;

class CControlMediator;
class CORETOOL_API CToolCmdMgr
{
public:
	CToolCmdMgr(void);
	~CToolCmdMgr(void);

	//Action Entry Confirm Code
	inline bool IsMouseCmdEntryL()
	{
		if(m_bPreMouseCmdL_ && !m_bPostMouseCmdL_)
			return true;
		else
			return false;
	}
	inline void InitMouseCmdFlagL()
	{
		m_bPreMouseCmdL_ = true;
		m_bPostMouseCmdL_ = false;
	}
	inline void ResetMouseCmdFlagL()
	{
		m_bPreMouseCmdL_ = false;
		m_bPostMouseCmdL_ = false;
	}
	inline void DoneMouseCmdL(){ m_bPostMouseCmdL_ = true; }

	inline bool IsMouseCmdEntryM()
	{
		if(m_bPreMouseCmdM_ && !m_bPostMouseCmdM_)
			return true;
		else
			return false;
	}
	inline void InitMouseCmdFlagM()
	{
		m_bPreMouseCmdM_ = true;
		m_bPostMouseCmdM_ = false;
	}
	inline void ResetMouseCmdFlagM()
	{
		m_bPreMouseCmdM_ = false;
		m_bPostMouseCmdM_ = false;
	}
	inline void DoneMouseCmdM(){ m_bPostMouseCmdM_ = true; }

	//일반 멤버함수
	void UndoCommand();
	void InitCommandManager(CControlMediator* _pControlMediator){ m_pControlMediator_ = _pControlMediator; }
	void ResetCommandManager();

	void RecordCommand(ICommandBuffer* _pCommandBuffer);

	//정보전달 함수
	const char* GetNextUndoMsg(){ return m_strNextUndoMsg_.c_str(); }
	bool IsMapChanged(){ return m_bMapChanged_; }
	void ResetMapChanged(){ m_bMapChanged_ = false; }

private:
	std::deque<ICommandBuffer*> m_dqCommandBuffer_;

	//토탈 사이즈
	size_t m_tTotalCmdSize_;

	//액션 플래그
	bool m_bPreMouseCmdL_;
	bool m_bPostMouseCmdL_;

	bool m_bPreMouseCmdM_;
	bool m_bPostMouseCmdM_;

	CControlMediator* m_pControlMediator_;

	//Undo Message
	std::string m_strNextUndoMsg_;
	std::string m_strCurrentUndoMsg_;
	std::string m_strFullMsg_;

	//private member function
	void SetNextUndoMsg_();

	//맵 변경여부
	bool m_bMapChanged_;
};
