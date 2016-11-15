#pragma once

#include "RClothWorldActorNode.h"
#include "BaseObject.h"

class ReflectiveRClothInfo : public CRTTIClass<ReflectiveRClothInfo, CBaseObject,0x01>,RClothInfo
{
public:
	ReflectiveRClothInfo(void);
	static	void RegisterReflection();
	void	setClothParameter(RClothParameter* pClothParameter)	{ m_pClothParameter = pClothParameter;	}

public:
	//-[ simulation ]
	void	setGravityX(float f)	{	m_pClothParameter->setGravityX(f);			}
	float	getGravityX(void)		{	return m_pClothParameter->getGravityX();	}
	void	setGravityY(float f)	{	m_pClothParameter->setGravityY(f);			}
	float	getGravityY(void)		{	return m_pClothParameter->getGravityY();	}
	void	setGravityZ(float f)	{	m_pClothParameter->setGravityZ(f);			}
	float	getGravityZ(void)		{	return m_pClothParameter->getGravityZ();	}

	void	setDampingCoefficientX(float f)		{	m_pClothParameter->setDampingCoefficientX(f);		}
	float	getDampingCoefficientX(void)		{	return m_pClothParameter->getDampingCoefficientX();	}

	void	setDampingCoefficientY(float f)		{	m_pClothParameter->setDampingCoefficientY(f);		}
	float	getDampingCoefficientY(void)		{	return m_pClothParameter->getDampingCoefficientY();	}

	void	setDampingCoefficientZ(float f)		{	m_pClothParameter->setDampingCoefficientZ(f);		}
	float	getDampingCoefficientZ(void)		{	return m_pClothParameter->getDampingCoefficientZ();	}


	void	setDampingJigglingVelocity(float f)		{	m_pClothParameter->setDampingJigglingVelocity(f);		}
	float	getDampingJigglingVelocity(void)		{	return m_pClothParameter->getDampingJigglingVelocity();	}

	//void	setTimeToSimulate(float f)	{	m_pClothParameter->setTimeToSimulate(f);		}
	//float	getTimeToSimulate(void)		{	return m_pClothParameter->getTimeToSimulate();	}

	void	setWorldSimulation(bool b)	{	m_pClothParameter->setWorldSimulation(b);		}
	bool	getWorldSimulation(void)	{	return m_pClothParameter->getWorldSimulation();	}

	void	setResolvingLogic(char* szResolvingLogic)	{	m_pClothParameter->setResolvingLogic(szResolvingLogic);	}
	char*	getResolvingLogic(void)						{	return m_pClothParameter->getResolvingLogic();			}


	//-[ constraint ]

	void	setDistanceRange(float f)		{	m_pClothParameter->setDistanceRange(f);			}
	float	getDistanceRange(void)			{	return m_pClothParameter->getDistanceRange();	}
	void	setDistanceStiffness(float f)	{	m_pClothParameter->setDistanceStiffness(f);			}
	float	getDistanceStiffness(void)		{	return m_pClothParameter->getDistanceStiffness();	}

	void	setStretchHorizontalRange(float f)		{	m_pClothParameter->setStretchHorizontalRange(f);		}
	float	getStretchHorizontalRange(void)			{	return m_pClothParameter->getStretchHorizontalRange();	}
	void	setStretchHorizontalStiffness(float f)	{	m_pClothParameter->setStretchHorizontalStiffness(f);		}
	float	getStretchHorizontalStiffness(void)		{	return m_pClothParameter->getStretchHorizontalStiffness();	}

	void	setStretchVerticalRange(float f)		{	m_pClothParameter->setStretchVerticalRange(f);			}
	float	getStretchVerticalRange(void)			{	return m_pClothParameter->getStretchVerticalRange();	}
	void	setStretchVerticalStiffness(float f)	{	m_pClothParameter->setStretchVerticalStiffness(f);			}
	float	getStretchVerticalStiffness(void)		{	return m_pClothParameter->getStretchVerticalStiffness();	}

	//-[ virtual translation ]
	/*void	setVirtualTranslationThresholdX(float f)	{	m_pClothParameter->setVirtualTranslationThresholdX(f);			}
	float	getVirtualTranslationThresholdX(void)		{	return m_pClothParameter->getVirtualTranslationThresholdX();	}
	void	setVirtualTranslationThresholdY(float f)	{	m_pClothParameter->setVirtualTranslationThresholdY(f);			}
	float	getVirtualTranslationThresholdY(void)		{	return m_pClothParameter->getVirtualTranslationThresholdY();	}
	void	setVirtualTranslationThresholdZ(float f)	{	m_pClothParameter->setVirtualTranslationThresholdZ(f);			}
	float	getVirtualTranslationThresholdZ(void)		{	return m_pClothParameter->getVirtualTranslationThresholdZ();	}


	void	setVirtualTranslationSizeX(float f)		{	m_pClothParameter->setVirtualTranslationSizeX(f);		}
	float	getVirtualTranslationSizeX(void)		{	return m_pClothParameter->getVirtualTranslationSizeX();	}
	void	setVirtualTranslationSizeY(float f)		{	m_pClothParameter->setVirtualTranslationSizeY(f);		}
	float	getVirtualTranslationSizeY(void)		{	return m_pClothParameter->getVirtualTranslationSizeY();	}
	void	setVirtualTranslationSizeZ(float f)		{	m_pClothParameter->setVirtualTranslationSizeZ(f);		}
	float	getVirtualTranslationSizeZ(void)		{	return m_pClothParameter->getVirtualTranslationSizeZ();	}*/

	//-[ warp ]
	void	setWarpThreshold(float f)		{	m_pClothParameter->setWarpThreshold(f);			}
	float	getWarpThreshold(void)			{	return m_pClothParameter->getWarpThreshold();	}

	//-[ option ]
	void	setDrawCollision(bool b)	{	m_pClothParameter->setDrawCollision(b);			}
	bool	getDrawCollision(void)		{	return m_pClothParameter->getDrawCollision();	}

	//-[ parameter ]
	RClothParameter*	getParameter(void)	{	return m_pClothParameter;	}
private:
	RClothParameter*	m_pClothParameter;

};