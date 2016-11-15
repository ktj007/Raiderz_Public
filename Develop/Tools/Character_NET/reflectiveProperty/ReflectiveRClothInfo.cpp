#include "stdafx.h"
#include "ReflectiveRClothInfo.h"


void ReflectiveRClothInfo::RegisterReflection()
{
	//-[ simulation ]
	RegisterProperty<float>("GravityX",&getGravityX,&setGravityX);
	RegisterProperty<float>("GravityY",&getGravityY,&setGravityY);
	RegisterProperty<float>("GravityZ",&getGravityZ,&setGravityZ);

	RegisterProperty<float>("DampingCoefX",&getDampingCoefficientX,&setDampingCoefficientX);
	RegisterProperty<float>("DampingCoefY",&getDampingCoefficientY,&setDampingCoefficientY);
	RegisterProperty<float>("DampingCoefZ",&getDampingCoefficientZ,&setDampingCoefficientZ);

	RegisterProperty<float>("DampingJiggleVel",&getDampingJigglingVelocity,&setDampingJigglingVelocity);

	//RegisterProperty<float>("TimeToSimulate",&getTimeToSimulate,&setTimeToSimulate);
	
	RegisterProperty<bool>("WorldSimulation",&getWorldSimulation,&setWorldSimulation);

	RegisterProperty<char*>("ResolvingLogic",&getResolvingLogic,&setResolvingLogic);
	

	//-[ constraint ]
	RegisterProperty<float>("DistanceRange",&getDistanceRange,&setDistanceRange);
	RegisterProperty<float>("StretchHorizontalRange",&getStretchHorizontalRange,&setStretchHorizontalRange);
	RegisterProperty<float>("StretchVerticalRange",&getStretchVerticalRange,&setStretchVerticalRange);

	RegisterProperty<float>("DistanceStiffness",&getDistanceStiffness,&setDistanceStiffness);
	RegisterProperty<float>("StretchHorizontalStiffness",&getStretchHorizontalStiffness,&setStretchHorizontalStiffness);
	RegisterProperty<float>("StretchVerticalStiffness",&getStretchVerticalStiffness,&setStretchVerticalStiffness);

	//-[ virtual translation ]
	/*RegisterProperty<float>("VirtualTranslationThresholdX",&getVirtualTranslationThresholdX,&setVirtualTranslationThresholdX);
	RegisterProperty<float>("VirtualTranslationThresholdY",&getVirtualTranslationThresholdY,&setVirtualTranslationThresholdY);
	RegisterProperty<float>("VirtualTranslationThresholdZ",&getVirtualTranslationThresholdZ,&setVirtualTranslationThresholdZ);

	RegisterProperty<float>("VirtualTranslationSizeX",&getVirtualTranslationSizeX,&setVirtualTranslationSizeX);
	RegisterProperty<float>("VirtualTranslationSizeY",&getVirtualTranslationSizeY,&setVirtualTranslationSizeY);
	RegisterProperty<float>("VirtualTranslationSizeZ",&getVirtualTranslationSizeZ,&setVirtualTranslationSizeZ);*/

	//-[ warp ]
	RegisterProperty<float>("WarpThreshold",&getWarpThreshold,&setWarpThreshold);
	

	//-[ option ]
	RegisterProperty<bool>("DrawCollision",&getDrawCollision,&setDrawCollision);


}

ReflectiveRClothInfo::ReflectiveRClothInfo( void )
{

}

