#pragma once

class XMotionLogic;

// 버프 애니메이션을 기억해서 idle 모션을 버프 애니메이션으로 변경하는 클래스입니다.
// 버프 종료이후에 idle 모션을 사용이 가능합니다.
class XMotionIdleMemory
{
	struct stBuffAniMemory
	{
		wstring			m_strMotionName;
		wstring			m_strAnimationName;

		int				m_nBuffID;

		XMotionTypes	m_nType;

		bool			m_bForced;

		stBuffAniMemory()
		{
			m_strMotionName.clear();
			m_strAnimationName.clear();

			m_nBuffID	= 0;

			m_nType		= MT_DEFAULT;
			m_bForced	= false;
		}
	};

private:
	vector<stBuffAniMemory> m_vecBuffAniMemory;

	bool			m_bMemory;

private:
	bool			ChangeMotion(XMotionLogic* pLogic);
	bool			PushMotion(XMotionLogic* pLogic);

	stBuffAniMemory* GetBuffAniLastData();
	stBuffAniMemory* GetBuffAniData(int nBuffID);

	bool			DeleteBuffAni(int nBuffID, stBuffAniMemory& stDelData);

public:
	XMotionIdleMemory() 
	{
		Init();
	}

	virtual ~XMotionIdleMemory() {}

	void			Init();
	void			End(int nBuffID, XMotionLogic* pLogic, bool bNpc);
	void			AllDel();

	void			SetIdleMemoryMotion(int nBuffID, wstring strMotion, XMotionTypes nType=MT_DEFAULT, bool bForced=false);
	void			SetIdleMemoryCustomMotion(int nBuffID, wstring strMotion, wstring strAnimationName, bool bForced=false);

	bool			CheckChangeMotion(wstring strMotion, XMotionLogic* pLogic, bool bForceIdle);
	bool			CheckPushMotion(wstring strMotion, XMotionLogic* pLogic, bool bForceIdle);

	bool			IsMemory() { return m_bMemory; }
};