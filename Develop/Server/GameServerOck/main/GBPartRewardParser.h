#pragma once

#include "GNPCInfo.h"

class GBPartRewardParser
{
public:
	GBPartRewardParser(void);
	~GBPartRewardParser(void);

	bool Parse( const wchar_t* szFileName );
private:
	void ParseNPC(MXmlElement* pElement, MXml* pXml);
	void ParseBPart( BPART_REWARD_INFO& infoBPartReward, MXmlElement* pElement, MXml* pXml );

};
