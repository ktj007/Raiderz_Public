
#pragma once


namespace mint3
{

class MResource
{
private:
	string	m_strName;

public:
	void SetName( const string& strName)	{ m_strName = strName;		}
	const string& GetName()					{ return m_strName;			}
};

}
