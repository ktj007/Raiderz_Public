#include "StdAfx.h"
#include "Car.h"


void ReCar::RegisterReflection()
{
	RegisterProperty<float>("Size",&getSize,&setSize);
	RegisterProperty<int>("Number",&getNumber,&setNumber);
	RegisterProperty<char*>("CarName",&getCarName,&setCarName);
}