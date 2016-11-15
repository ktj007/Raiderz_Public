#pragma once

#include "MCommandParameter.h"
#include <map>

class MSVector;
namespace minet {

	class MTypeConvert
	{
		typedef map<const char*, MCommandParameterType>		AutoTypeMap;
		typedef AutoTypeMap::value_type						AutoType;

		AutoTypeMap mapType;

	public:
		MCommandParameterType Convert(const char* str)
		{
			return mapType[str];
		}

		MTypeConvert()
		{
			mapType[typeid(INT).name()] = MPT_INT;
			mapType[typeid(UINT).name()] = MPT_UINT;
			mapType[typeid(FLOAT).name()] = MPT_FLOAT;
			mapType[typeid(double).name()] = MPT_DOUBLE;
			mapType[typeid(CHAR).name()] = MPT_CHAR;
			mapType[typeid(UCHAR).name()] = MPT_UCHAR;
			mapType[typeid(SHORT).name()] = MPT_SHORT;
			mapType[typeid(USHORT).name()] = MPT_USHORT;
			mapType[typeid(INT64).name()] = MPT_INT64;
			mapType[typeid(UINT64).name()] = MPT_UINT64;
			mapType[typeid(bool).name()] = MPT_BOOL;
			mapType[typeid(MVector3).name()] = MPT_VECTOR;
			mapType[typeid(MSVector).name()] = MPT_SVECTOR;
			mapType[typeid(MUID).name()] = MPT_UID;
			mapType[typeid(MID).name()] = MPT_ID;
		}
	};

	static MTypeConvert TypeConvert;
} 