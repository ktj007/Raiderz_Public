#pragma once

#include "..\Reflection\BaseObject.h"

class CCar 
{
public:
	CCar(void)
	{
		m_size		= 5.0f;
		m_number	= 2;
		strcpy_s(m_szCarName,"default_car_name");
	};

	void	setSize(float f)	{	m_size = f;			}
	float	getSize(void)		{	return m_size;		}

	void	setNumber(int i)	{	m_number = i;		}
	int		getNumber(void)		{	return m_number;	}

	void	setCarName(char* szCarName)	{	strcpy_s(m_szCarName,szCarName);	}
	char*	getCarName(void)			{	return m_szCarName;					}

private:
	float m_size;
	int	  m_number;
	char  m_szCarName[255];
};

class ReCar : public CRTTIClass<ReCar, CBaseObject,0x01>,CCar
{
public:
	ReCar(void){};
	static	void RegisterReflection();
};