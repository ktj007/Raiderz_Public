#pragma once

class XItemRelatedNPCInfo
{
public:
	int			m_nItemID;
	set<int>	m_setNPCID;

public:
	XItemRelatedNPCInfo(void);
	~XItemRelatedNPCInfo(void);
};
