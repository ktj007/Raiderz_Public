#pragma once

#include "RAnimationKey.h"

using namespace rs3;

class MCPosKey : public RPosKey
{
public:
	bool IsAlmostEqualTo(const RPosKey& key)
	{
		// 각각 0.1% 이하이면 true
		return (fabs((key.x-x) / x) < .001f) &&
			(fabs((key.y-y) / y) < .001f) &&
			(fabs((key.z-z) / z) < .001f) ;
	}
	bool IsEqualTo(const RPosKey& key) const
	{
		return key.x==x && key.y==y && key.z==z;
	}

	float GetMetricTo(const RPosKey& key) const
	{
		return (key-*this).Length();
	}
};

class MCQuatKey : public RQuatKey
{
public:
	bool IsAlmostEqualTo(const RQuatKey& key)
	{
		RQuaternion quatInv = Inverse();
		RQuaternion quatMul = quatInv*key;

		// 오차각이 .1도 이하이면 생략
		return (quatMul.w > cosf( DegreeToRadian(.1f /2.f) ) );
	}
	bool IsEqualTo(const RQuatKey& key) const
	{
		return key.x==x && key.y==y && key.z==z && key.w==w;
	}

	float GetMetricTo(const RQuatKey& key) const
	{
		RQuaternion quatInv = Inverse();
		RQuaternion quatMul = quatInv*key;
		return fabs(quatMul.w);
	}
};

typedef MCPosKey MCScaleKey;

class MCVisKey : public RVisKey
{
public:
	bool IsAlmostEqualTo(const RVisKey& key)
	{
		// 오차 1% 이하이면 생략
		return fabs(v-key.v)<0.01f ;
	}
	bool IsEqualTo(const RVisKey& key) const
	{
		return key.v==v;
	}
	float GetMetricTo(const RVisKey& key) const
	{
		return fabs(key.v-v);
	}
};

class MCVec3Key : public RVec3Key
{
public:
	bool IsAlmostEqualTo(const MCVec3Key& key)
	{
		return (( key - *this).Length() < .013f);
	}
	bool IsEqualTo(const MCVec3Key& key) const
	{
		return key.x==x && key.y==y && key.z==z;
	}

	float GetMetricTo(const MCVec3Key& key) const
	{
		return (key-*this).Length();
	}
};

// optimize 기능을 가진 keytrack template
template < class _KEYTYPE >
class MCKeyTrack : public list<_KEYTYPE>
{
public:
	int Optimize()	// 불필요한 키를 삭제한다. 리턴값은 삭제한 개수
	{
		// 키가 두개이하라면 의미가 없다.
		if(size()<=2) return 0;
		int nDeleted = 0;

		// 먼저 같은키가 있으면 삭제
		for(iterator i=begin();i!=end();)
		{
			iterator nexti = i;
			nexti++;
			if(nexti==end()) break;

			iterator nextnexti = nexti;
			nextnexti++;
			if(nextnexti==end()) break;

			if((*i).IsEqualTo(*nexti) && (*nexti).IsEqualTo(*nextnexti))
			{
				nDeleted++;
				erase(nexti);
			}else
				i++;
		}


		iterator prevItr = begin();
		iterator itr = prevItr; itr++;
		iterator nextItr = itr; nextItr++;
		for(;nextItr!=end();nextItr++) {
			const _KEYTYPE& prevKey = *prevItr;
			const _KEYTYPE& currentKey = *itr;
			const _KEYTYPE& nextKey = *nextItr;

			// 앞/뒤 키로 현재 키 값을 보간해서 값이 거의 비슷하면 생략한다.
			float s = float(currentKey.frame - prevKey.frame) / float(nextKey.frame - prevKey.frame);
			_KEYTYPE interpolatedKey;
			interpolatedKey.SetInterpolatedKey(prevKey,nextKey,s);

			bool bInflection = prevKey.GetMetricTo(currentKey) > prevKey.GetMetricTo(nextKey);
			if( !bInflection  &&	// 변곡점은 생략 불가
				(interpolatedKey.IsAlmostEqualTo(currentKey)) ) {
				erase(itr);
				itr=nextItr;
				nDeleted++;
			}else
			{
				prevItr++;
				itr++;
			}
		}
		return nDeleted;
	}
};

