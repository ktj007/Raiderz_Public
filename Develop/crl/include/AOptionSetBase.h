#pragma once

class AOptionSetBase
{
public:
	AOptionSetBase(wstring option, wstring description);
	~AOptionSetBase();

	bool Match(wstring option);

	virtual bool SetArgument(wstring arguments, wstring splitter) = 0;
	
protected:
	bool Split(wstring arguments, wstring splitter);	

	virtual bool SetSplitArgument(wstring argument)	{ return false;	}

private:
	wstring m_option;
	wstring m_description;
};
