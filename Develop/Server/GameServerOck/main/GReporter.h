#pragma once

#include "GNPCReporter.h"
#include "GPlayerReporter.h"

class GReporter
{
private:
	GNPCReporter		m_NPCReporter;
	GPlayerReporter		m_PlayerReporter;
public:
	GReporter()	{}
	~GReporter(){}
	bool ReportTargetEntity(MUID uidField, MUID uidTarget);
	bool ReportNPC(MUID uidField, MUID uidNPC);
	bool ReportPC(MUID uidField, MUID uidPlayer);
};