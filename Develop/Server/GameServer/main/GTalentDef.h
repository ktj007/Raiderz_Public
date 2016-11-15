#ifndef _GTALENT_DEF_H
#define _GTALENT_DEF_H

#include "CSDef.h"

namespace cscommon_server
{
	struct TALENT_TARGET_INFO
	{
		MUID uidTarget;
		int8 nCapsuleGroupIndex;
		int8 nCapsuleIndex;
		uint16 nCheckTime;
		vec3 vGroundPoint;
		int nHitBPart;

		inline TALENT_TARGET_INFO& operator=(const MUID& a){
			uidTarget = a;
			return *this;
		}

		inline const MUID& operator()(){
			return uidTarget;
		}

		static TALENT_TARGET_INFO Invalid()			{ return TALENT_TARGET_INFO(); }
		bool IsInvalid() const						{ return uidTarget.IsInvalid(); }
		bool IsValid()	 const						{ return uidTarget.IsValid(); }
		bool HasGroundPoint() const					{ return vGroundPoint != vec3::ZERO; }

		TALENT_TARGET_INFO(): uidTarget(MUID::Invalid()), nCapsuleGroupIndex(INVALID_ID), nCapsuleIndex(INVALID_ID), nCheckTime(0), vGroundPoint(vec3::ZERO), nHitBPart(0) {}
		TALENT_TARGET_INFO(MUID uid) : uidTarget(uid), nCapsuleGroupIndex(INVALID_ID), nCapsuleIndex(INVALID_ID), nCheckTime(0), vGroundPoint(vec3::ZERO), nHitBPart(0) {}
	};


} // cscommon_server

#endif