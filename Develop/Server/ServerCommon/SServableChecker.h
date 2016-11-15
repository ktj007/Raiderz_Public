#pragma once

#include "ServerCommonLib.h"

class MRegulator;

class SCOMMON_API SServableChecker
{
public:
	static float TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC;
public:
	SServableChecker();
	virtual ~SServableChecker();

	bool IsServable() { return m_bIsServable; }
	void Update(const float fDelta);

protected:
	/// @desc			override 해서 작성합니다.
	///					리턴값에 따라 Servable 속성이 셋팅됩니다.
	///
	/// @return	true	Servable 속성이 true 로 설정됩니다.
	///			false	Servable 속성이 false 로 설정됩니다.
	virtual bool Check(const float fDelta) = 0;

private:
	void SetServable(bool bIs) { m_bIsServable = bIs; }

	MRegulator* m_pServableCheckerRglt;
	bool m_bIsServable;

};
