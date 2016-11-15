#pragma once

#include <vector>

template<typename T_Key, typename T_KeyContainer = std::vector<T_Key> >
class XCutSceneKeyFrameUpdator
{
public:
	XCutSceneKeyFrameUpdator() : m_fAccumulatedElapsed(0.f), m_nCurrentKey(0) {}
	virtual ~XCutSceneKeyFrameUpdator(){}

	template< typename T_Caller >
	bool UpdateKey(T_Caller* pCaller, float fElapsed, T_KeyContainer& _rKeys)
	{
		size_t nTotal = _rKeys.size();
		if (m_nCurrentKey >= nTotal)
			return false;

		// 시간 증가
		m_fAccumulatedElapsed += fElapsed;

		bool bProcStop = false;
		while (!bProcStop && m_nCurrentKey < nTotal)
		{
			T_Key* pPre = (m_nCurrentKey == 0) ? NULL : &_rKeys[m_nCurrentKey-1];
			T_Key* pCurrent = &_rKeys[m_nCurrentKey];
			T_Key* pNext = (m_nCurrentKey + 1 == nTotal) ? NULL : &_rKeys[ m_nCurrentKey + 1];

			// 모두있는경우
			// a - [pPre] - b - [pCurrent] - c - [pNext] - d
			// pNext만 널
			// e - [pPre] - f - [pCurrent] - g
			// pPre만 널
			// h - [pCurrent] - i - [pNext] - j
			// 양쪽 모두 널
			// k - [pCurrnet] - l

			// current는 시간이 지났을때 바뀐다
			// a, e : 키를 업데이트하고 다음상황(b,f)로 변경
			// b, f : 키를 업데이트하고 다음상황(d, g, j, l vs c, i)로 변경
			// h, k : 업데이트만 멈춰준다
			// d, g, j, l vs c, i

			// [key 1] - 1 - [key 2] - 2
			T_Key* pProcKey = NULL;
			int nType = 0;

			// 2 가 되는 경우 : 
			// - d, j: pNext가 있고, 현재 시간이 pNext의 시간을 지났을 경우, pProcKey2 = pNext, 다음키는 진행하고 프록을 멈추지는 않음
			// - g, l : pNext가 없고 현재 시간이 pCurrent의 시간을 지났을 경우, pProcKey2 = pCurrent 다음키는 진행하고 프록을 멈추지는 않음
			if ( (pNext) && (m_fAccumulatedElapsed >= pNext->fTime) )
			{
				nType = 2;
				pProcKey = pNext;
			}
			else if ( (!pNext) && (m_fAccumulatedElapsed >= pCurrent->fTime) )
			{
				nType = 2;
				pProcKey = pCurrent;
			}
			// 1이 되는 경우
			// - c, i : pNext 가 있고 현재시간이 pNext의 시간보다 작고 pCurrent보다 크거나 같음, 키 증가 안함, 프록 멈춤
			else if ( (pNext) && (m_fAccumulatedElapsed < pNext->fTime) && (m_fAccumulatedElapsed >= pCurrent->fTime) )
			{
				nType = 1;
			}
			// h, k : 업데이트만 멈춰준다
			else if (NULL == pPre)
			{
				nType = 4;
			}

			switch(nType)
			{
			case 0:
				{
					// a,e,b,f
					m_nCurrentKey += 1;
					break;
				}
			case 1:
				{
					// c, i
					bProcStop = true;

					float fInterpolationRatio = (m_fAccumulatedElapsed - pCurrent->fTime) / ( pNext->fTime - pCurrent->fTime);
					T_Key* pNextNext = (m_nCurrentKey + 2 >= nTotal) ? NULL : &_rKeys[ m_nCurrentKey + 2 ];
					pCaller->OnUpdateBetweenKey(pCurrent, pNext, fInterpolationRatio, pPre, pNextNext);
					break;
				}
			case 2:
			 {
				 // d, g, j, l
				 m_nCurrentKey += 1;
				 if (pNext == NULL)
					 pCaller->OnUpdateOverKey(pProcKey);
				 break;
			 }
			case 4:
				{
					// h, k
					bProcStop = true;
					break;
				}
			}
		}

		return (m_nCurrentKey < nTotal);
	}

	void SortKey(T_KeyContainer& _rKeys, float* _pfStartTime = NULL, float* _pfEndTime = NULL)
	{
		struct SFuncKeyLess
		{
			bool operator()(const T_Key& _rLhs, const T_Key& _rRhs)
			{
				return _rLhs.fTime < _rRhs.fTime;
			}
		};

		std::sort( _rKeys.begin(), _rKeys.end(), SFuncKeyLess() );
		size_t nCount = _rKeys.size();
		if (nCount > 0 && _pfStartTime && _pfEndTime)
		{
			*_pfStartTime = _rKeys[0].fTime;
			*_pfEndTime = _rKeys[nCount - 1].fTime;
		}
	}

	void Reset()
	{
		m_fAccumulatedElapsed = 0.f;
		m_nCurrentKey = 0;
	}

	size_t	GetCurrentKey()			{ return m_nCurrentKey; }
	float	GetAccumulatedElapsed()	{ return m_fAccumulatedElapsed; }

	int	GetKeyFromTime(float fValue, const T_KeyContainer& _rKeys)
	{
		int nSize = (int)_rKeys.size();
		for (int i = 0; i < nSize; ++i)
		{
			float fError = abs(_rKeys[i].fTime - fValue);
			if (fError < 0.001f)
				return i;
		}

		return -1;
	}

protected:
	float	m_fAccumulatedElapsed;
	size_t	m_nCurrentKey;
};
