#ifndef _SDSNFACTORY_H
#define _SDSNFACTORY_H


class SBaseDsnFactory;


class SDsnFactory
{
public :
	~SDsnFactory();

	SBaseDsnFactory* Get() { return m_pFactory; }
	void Set(SBaseDsnFactory* pFactory);
	void Release();

	static SDsnFactory& GetInstance()
	{
		static SDsnFactory df;
		return df;
	}

private :
	SDsnFactory() {}

protected :
	SBaseDsnFactory* m_pFactory;
};



#endif