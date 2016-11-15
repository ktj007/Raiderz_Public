#pragma once

#define RESOLVING_LOGIC_SIZE 255

class RClothParameter
{
public:
	//-[ simulation ]
	void	setDampingCoefficientX(float f)		{	m_dampingCoefficientX = f;		}
	float	getDampingCoefficientX(void)		{	return m_dampingCoefficientX;	}

	void	setDampingCoefficientY(float f)		{	m_dampingCoefficientY = f;		}
	float	getDampingCoefficientY(void)		{	return m_dampingCoefficientY;	}

	void	setDampingCoefficientZ(float f)		{	m_dampingCoefficientZ = f;		}
	float	getDampingCoefficientZ(void)		{	return m_dampingCoefficientZ;	}

	void	setDampingJigglingVelocity(float f)		{	m_dampingJigglingVelocity = f;		}
	float	getDampingJigglingVelocity(void)		{	return m_dampingJigglingVelocity;	}

	void	setGravityX(float f)	{	m_gravityX = f;		}
	float	getGravityX(void)		{	return m_gravityX;	}
	void	setGravityY(float f)	{	m_gravityY = f;		}
	float	getGravityY(void)		{	return m_gravityY;	}
	void	setGravityZ(float f)	{	m_gravityZ = f;		}
	float	getGravityZ(void)		{	return m_gravityZ;	}

	void	setTimeToSimulate(float f)	{ m_timeToSimulate = f;		}
	float	getTimeToSimulate(void)		{ return m_timeToSimulate;	}

	void	setWorldSimulation(bool b)	{ m_worldSimulation = b;	}
	bool	getWorldSimulation(void)	{ return m_worldSimulation;	}

	void	setResolvingLogic(char* szSimulationLogic)	{	strcpy_s(m_szResolvingLogic,szSimulationLogic);	}
	char*	getResolvingLogic(void)						{	return m_szResolvingLogic;	}

	//-[ constraint ]
	void	setDistanceRange(float f)		{	m_distanceRange = f;	}
	float	getDistanceRange(void)			{	return m_distanceRange;	}
	void	setDistanceStiffness(float f)	{	m_distanceStiffness = f;	}
	float	getDistanceStiffness(void)		{	return m_distanceStiffness;	}

	void	setStretchHorizontalRange(float f)		{	m_stretchHorizontalRange = f;		}
	float	getStretchHorizontalRange(void)			{	return m_stretchHorizontalRange;	}
	void	setStretchHorizontalStiffness(float f)	{	m_stretchHorizontalStiffness = f;		}
	float	getStretchHorizontalStiffness(void)		{	return m_stretchHorizontalStiffness;	}

	void	setStretchVerticalRange(float f)		{	m_stretchVerticalRange = f;	}
	float	getStretchVerticalRange(void)			{	return m_stretchVerticalRange;	}
	void	setStretchVerticalStiffness(float f)	{	m_stretchVerticalStiffness = f;		}
	float	getStretchVerticalStiffness(void)		{	return m_stretchVerticalStiffness;	}

	//-[ virtual translation ]
	void	setVirtualTranslationThresholdX(float f)	{	m_virtualTranslationThresholdX = f;		}
	float	getVirtualTranslationThresholdX(void)		{	return m_virtualTranslationThresholdX;	}
	void	setVirtualTranslationThresholdY(float f)	{	m_virtualTranslationThresholdY = f;		}
	float	getVirtualTranslationThresholdY(void)		{	return m_virtualTranslationThresholdY;	}
	void	setVirtualTranslationThresholdZ(float f)	{	m_virtualTranslationThresholdZ = f;		}
	float	getVirtualTranslationThresholdZ(void)		{	return m_virtualTranslationThresholdZ;	}

	void	setVirtualTranslationSizeX(float f)		{	m_virtualTranslationSizeX = f;		}
	float	getVirtualTranslationSizeX(void)		{	return m_virtualTranslationSizeX;	}
	void	setVirtualTranslationSizeY(float f)		{	m_virtualTranslationSizeY = f;		}
	float	getVirtualTranslationSizeY(void)		{	return m_virtualTranslationSizeY;	}
	void	setVirtualTranslationSizeZ(float f)		{	m_virtualTranslationSizeZ = f;		}
	float	getVirtualTranslationSizeZ(void)		{	return m_virtualTranslationSizeZ;	}

	//-[ warp ]
	void	setWarpThreshold(float f)				{	m_warpThreshold = f;	}
	float	getWarpThreshold(void)					{	return m_warpThreshold;	}

	//-[ option ]
	void	setDrawCollision(bool b)	{	m_bDrawCollision = b;	}
	bool	getDrawCollision(void)		{	return m_bDrawCollision;}

	//-[ save/load ]
	bool	save(FILE* pFile);
	bool	load(FILE* pFile);

private:
	//-[ simulation ]
	float	m_dampingCoefficientX;
	float	m_dampingCoefficientY;
	float	m_dampingCoefficientZ;
	float	m_dampingJigglingVelocity;
    float	m_gravityX;
    float	m_gravityY;
	float	m_gravityZ;
	float	m_timeToSimulate;
	bool    m_worldSimulation;
	char	m_szResolvingLogic[RESOLVING_LOGIC_SIZE];

	//-[ constraint ]
	float	m_distanceRange;
	float	m_distanceStiffness;

	float	m_stretchHorizontalRange;
	float	m_stretchHorizontalStiffness;

	float	m_stretchVerticalRange;
	float	m_stretchVerticalStiffness;

	//-[ virtual translation ]
	float	m_virtualTranslationThresholdX;
	float	m_virtualTranslationThresholdY;
	float	m_virtualTranslationThresholdZ;

	float	m_virtualTranslationSizeX;
	float	m_virtualTranslationSizeY;
	float	m_virtualTranslationSizeZ;

	//-[ warp ]
	float	m_warpThreshold;

	//-[ option ]
    bool    m_bDrawCollision;

public:
	RClothParameter(void);


};