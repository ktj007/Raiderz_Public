#pragma once

class GFakeDBIDGenerator
{
public:
	GFakeDBIDGenerator(void);
	~GFakeDBIDGenerator(void);

	static int NewCID();

private:
	static int			m_nCID;	
};
