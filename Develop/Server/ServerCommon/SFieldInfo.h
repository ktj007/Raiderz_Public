#pragma once

#include "ServerCommonLib.h"
#include <string>
#include "CSFieldInfo.h"

/// 필드 정보
class SCOMMON_API SFieldInfo : public CSFieldInfo
{
public:
	bool					m_bDynamic;		// 공유/인스턴스 여부
	int						m_nDefaultChannelCount;
	int						m_nCapacity;	///< 한 채널당 최대로 들어갈 수 있는 인원수
	bool					m_bValidation;	///< Validator에서 체크할 지 여부
	bool					m_bAILODEnable;	///< AI LOD를 할지 여부


	SFieldInfo() : m_bDynamic(false), m_nDefaultChannelCount(DEFAULT_CHANNEL_COUNT), m_nCapacity(CHANNEL_PLAYER_LIMIT), m_bValidation(true), m_bAILODEnable(true) {}
	virtual ~SFieldInfo() {}
	void Assign(CSFieldFileInfo* pFileInfo);
	std::wstring MakeFieldFileName(const wchar_t* szFieldPath);
};