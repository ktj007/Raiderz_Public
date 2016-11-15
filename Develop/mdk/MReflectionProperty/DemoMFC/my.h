#pragma once

#include <string>
#include "..\reflection\BaseObject.h"
#include "..\reflection\PropertyHelper.h"

class CBox
{
public:
	CBox(void)
	{
		m_width = m_height = 0.0f;
		m_ID = 5;
		m_Number = 0;
		memset(m_szTestName,0,sizeof(m_szTestName));
		strcpy_s(m_szTestName,"TestChar");
	}
public:
	void		setWidth(float width)		{ 	m_width = width;	}
	void		setHeight(float height)		{ 	m_height = height;	}
	void		setID(int ID)				
	{	
		m_ID = ID;			
	}
	void		setNumber(int number)			{	m_Number = number;					}
	void		setName(std::string name)		{	m_name = name;						}
	void		setName_str(char* name)			{	m_name = name;}
	void		setTestName(char* szTestName)	{	strcpy_s(m_szTestName,szTestName);	}

	float		getWidth(void)				{ return m_width;	}
	float		getHeight(void)				{ return m_height;	}
	int			getID(void)					{ return m_ID;		}
	int			getNumber(void)				{ return m_Number;	}
	std::string	getName(void)				{ return m_name;	}
	char*		getName_cstr(void)			{ return (char*)m_name.c_str();}
	char*		getTestName(void)			{ return m_szTestName;	}


private:
	float		m_width;
	float		m_height;
	int			m_ID;
	int			m_Number;
	std::string	m_name;
	char		m_szTestName[255];
};



class REBox : public CRTTIClass<REBox, CBaseObject,0x01> , public CBox	
{	
	enum SIMULATE_TYPE
	{
		SIMUL_NONE = 0,
		SIMULATE_ANIMATION,
		SIMULATE_KINEMATIC,
		SIMULATE_DYNAMIC,
	};

public:
	REBox() : m_nSimulateType(SIMULATE_DYNAMIC), m_dwColor(0xff0000) {}

	int			getMinID(void)				{ return -1;		};
	int			getMaxID(void)				{ return 600;		};

	float		getMinWidth(void)			{ return -100.0f;		};
	float		getMaxWidth(void)			{ return +100.0f;		};

	vector<int>	m_vectorInt;
	vector<int>	getVectorInt()				{ return m_vectorInt; }
	void		setVectorInt(vector<int> v)	{ m_vectorInt = v; }

	DWORD GetColor()						{ return m_dwColor; }
	void SetColor(DWORD dwColor)			{ m_dwColor = dwColor; }

	int			m_nSimulateType;
	DWORD		m_dwColor;

	DECLARE_GET_SET_ENUM( SIMULATE_TYPE, m_nSimulateType, SIMULATE_ANIMATION, SIMULATE_DYNAMIC );

	static	void RegisterReflection();
};

class CCircle
{
public:
	CCircle(void)
	{
		m_bDraw = false;
	}	

	bool getDraw(void)			{	return m_bDraw;	}
	void setDraw(bool bDraw)
	{
		m_bDraw = bDraw;
	}

private:
	bool m_bDraw;

};

class RECircle : public CRTTIClass<RECircle, CBaseObject,0x01> , CCircle	
{
public:
	static	void RegisterReflection();

};
