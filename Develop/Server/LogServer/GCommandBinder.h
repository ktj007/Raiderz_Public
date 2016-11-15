#pragma once

#include "MCommand.h"
#include "MCommandParameter.h"

namespace cmd_binder
{
	template< typename T > struct Type2Enum { enum { Result = -1 }; };
	template<> struct Type2Enum< int >				{ enum { Result = minet::MPT_INT }; };
	template<> struct Type2Enum< unsigned int >		{ enum { Result = minet::MPT_UINT }; };
	template<> struct Type2Enum< float >			{ enum { Result = minet::MPT_FLOAT }; };
	template<> struct Type2Enum< double >			{ enum { Result = minet::MPT_DOUBLE }; };
	template<> struct Type2Enum< char >				{ enum { Result = minet::MPT_CHAR }; };
	template<> struct Type2Enum< unsigned char >	{ enum { Result = minet::MPT_UCHAR }; };
	template<> struct Type2Enum< short >			{ enum { Result = minet::MPT_SHORT }; };
	template<> struct Type2Enum< unsigned short>	{ enum { Result = minet::MPT_USHORT }; };
	template<> struct Type2Enum< int64 >			{ enum { Result = minet::MPT_INT64 }; };
	template<> struct Type2Enum< bool >				{ enum { Result = minet::MPT_BOOL }; };
	template<> struct Type2Enum< const char* >		{ enum { Result = minet::MPT_STR }; };
	template<> struct Type2Enum< MVector3>			{ enum { Result = minet::MPT_VECTOR }; };
	template<> struct Type2Enum< MSVector >			{ enum { Result = minet::MPT_SVECTOR }; };
	template<> struct Type2Enum< MUID >				{ enum { Result = minet::MPT_UID }; };
	template<> struct Type2Enum< MID >				{ enum { Result = minet::MPT_ID }; };
	template<> struct Type2Enum< MBlob* >			{ enum { Result = minet::MPT_BLOB }; };

	template<typename T>	struct ParamDefault				{ static T				_ret() { return 0; }; }; 
	template<>				struct ParamDefault<float>		{ static float			_ret() { return 0.0f; }; };
	template<>				struct ParamDefault<bool>		{ static bool			_ret() { return false; }; };
	template<>				struct ParamDefault<const char*>{ static const char*	_ret() { return ""; }; };
	template<>				struct ParamDefault<MVector3>	{ static MVector3		_ret() { return MVector3(0,0,0); }; };
	template<>				struct ParamDefault<MSVector>	{ static MSVector		_ret() { return MSVector(0,0,0); }; };
	template<>				struct ParamDefault<MUID>		{ static MUID			_ret() { return MUID::ZERO; }; };
	template<>				struct ParamDefault<MID>		{ static MID			_ret() { return 0; }; };
	template<>				struct ParamDefault<MBlob*>		{ static MBlob*			_ret() { return NULL; }; };

	template <typename T>
	T param(minet::MCommand* pCommand, int nParamIndex)
	{
		if ((pCommand->GetParameter(nParamIndex) == NULL) ||
			(Type2Enum<T>::Result != pCommand->GetParameter(nParamIndex)->GetType()))
		{
			return ParamDefault<T>::_ret();
		}

		return *(T*)(pCommand->GetParameter(nParamIndex)->GetPointerRaw());
	};
}


// 사용 방법
//
// int nParam1 = cmd_binder::param<int>(pCommand, 0);
// const char* szParam2 = cmd_binder::param<const char*>(pCommand, 1);
