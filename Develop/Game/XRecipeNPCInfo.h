#pragma once

#define MAX_RECIPENPC 10

class XRecipeNPCInfo
{
public:
	XRecipeNPCInfo(void);
	~XRecipeNPCInfo(void);

public:
	int	m_nRecipeID;
	vector<pair<int,int>> m_vecFieldNPC;	
};
