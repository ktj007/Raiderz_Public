#pragma once

#include "CSCommonLib.h"

enum QOBJECTIVE_TYPE;
enum QO_PUBLIC_PROGRESS;

/// 퀘스트 목적 정보
class CSCOMMON_API CSQObjectiveInfo
{
public:
	CSQObjectiveInfo();
	virtual ~CSQObjectiveInfo();

public:
	int						nID;			///< 목적 ID
	int						nQuestID;		///< 퀘스트 ID
	tstring					strDesc;		///< 목적을 길게 기술한 필드
	QOBJECTIVE_TYPE			nType;			///< 타입
	vector<int>				vecParam1;
	int						nParam2;
	int						nFieldID;		///< 필드 ID
	int						nMarkerID;		///< 마커 ID
	int						nFieldID2;		///< 필드 ID
	int						nMarkerID2;		///< 마커 ID
	QO_PUBLIC_PROGRESS		nPublicProgress;///< 목적 상태 변경시 적용대상
};
