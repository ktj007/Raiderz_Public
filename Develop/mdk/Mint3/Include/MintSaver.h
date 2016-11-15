#pragma once

namespace mint3 {


class MintSaver
{
protected:
public:
	MintSaver();
	virtual ~MintSaver();
	bool SaveToFile(const char* szFileName);
};


} // namespace mint3
