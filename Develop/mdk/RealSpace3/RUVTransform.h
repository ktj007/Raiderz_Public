#pragma once

#include "RAnimationKey.h"

namespace rs3 {

class RS_API RUVTransform
{
public:
	RUVTransform() : m_vScale(DEFAULT_SCALE), m_vOffset(DEFAULT_OFFSET)
	{
	}
	~RUVTransform() {}

	void SetDefault()
	{
		m_vScale =DEFAULT_SCALE;
		m_vOffset =DEFAULT_OFFSET;
	}

	void SetScale(RVector2 vScale) { m_vScale=vScale; }
	void SetOffset(RVector2 vOffset) { m_vOffset=vOffset; }

	RVector2 GetScale() const { return m_vScale; }
	RVector2 GetOffset() const { return m_vOffset; }
	RVector4 GetOffsetScaleVector() { return RVector4(m_vOffset.x, m_vOffset.y, m_vScale.x, m_vScale.y); }

	bool IsDefault()	{ return IsDefaultScale() && IsDefaultOffset(); }
	bool IsDefaultScale() { return m_vScale==RVector2(1.0f, 1.0f); }
	bool IsDefaultOffset() { return m_vOffset==RVector2(0.0f, 0.0f); }


	bool operator==(const RUVTransform& other)
	{
		return this->GetOffset()==other.GetOffset() &&
			this->GetScale()==other.GetScale();
	}
	bool operator!=(const RUVTransform& other)
	{
		return !(*this==other);
	}	

private:
	RVector2 m_vScale;
	RVector2 m_vOffset;

	static const RVector2 DEFAULT_SCALE;
	static const RVector2 DEFAULT_OFFSET;
};




class RS_API RUVAnimationInfo
{
public:
	RUVAnimationInfo();
	~RUVAnimationInfo();

	RUVTransform Open(MXmlElement *pElement);
	void Save(MXmlElement *pElement);
	RUVTransform GetUVTransform(UINT nFrame);

	bool IsHaveAnimationKey() { return !m_offsetKeyTrack.empty() || !m_scaleKeyTrack.empty() || !m_rotateKeyTrack.empty(); }
	void ClearAnimationKey() { m_offsetKeyTrack.Clear(); m_scaleKeyTrack.Clear(); m_rotateKeyTrack.Clear(); }

	RAnimationTrackTest<RVec3Key>& GetOffsetKeyTrack() { return m_offsetKeyTrack; }
	RAnimationTrackTest<RVec3Key>& GetScaleKeyTrack() { return m_scaleKeyTrack; }
	RAnimationTrackTest<RVec3Key>& GetRotateKeyTrack() { return m_rotateKeyTrack; }

	bool operator==(RUVAnimationInfo& src);


private:
	RAnimationTrackTest<RVec3Key> m_offsetKeyTrack;
	RAnimationTrackTest<RVec3Key> m_scaleKeyTrack;
	RAnimationTrackTest<RVec3Key> m_rotateKeyTrack;
	UINT		m_uOffsetIter;
	UINT		m_uScaleIter;
	UINT		m_uRotateIter;
};

}