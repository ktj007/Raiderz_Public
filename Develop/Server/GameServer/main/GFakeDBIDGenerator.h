#pragma once

class GFakeDBIDGenerator
{
public:
	GFakeDBIDGenerator(void);
	~GFakeDBIDGenerator(void);

	static CID NewCID();

private:
	static CID			m_nCID;	
};
