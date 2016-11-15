#include "stdafx.h"
#include "IdvVectorMath.h"
#include <memory>
using namespace idv;


    

///////////////////////////////////////////////////////////////////////  
//  Transform::Transform definition

Transform::Transform( )
{
    LoadIdentity( );
}


///////////////////////////////////////////////////////////////////////  
//  Transform::~Transform definition

Transform::~Transform( )
{
    // do nothing
}


///////////////////////////////////////////////////////////////////////  
//  Transform::Set definition

void Transform::Set(const float afData[4][4])
{
    memcpy(&m_afData[0][0], &afData[0][0], 16 * sizeof(float));
}


///////////////////////////////////////////////////////////////////////  
//  Transform::Set definition

void Transform::Set(const float* pData)
{
	memcpy(&m_afData[0][0], pData, 16 * sizeof(float));
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::LoadIdentity definition

void Transform::LoadIdentity( )
{
    m_afData[0][0] = 1.0f;
    m_afData[0][1] = 0.0f;
    m_afData[0][2] = 0.0f;
    m_afData[0][3] = 0.0f;
    m_afData[1][0] = 0.0f;
    m_afData[1][1] = 1.0f;
    m_afData[1][2] = 0.0f;
    m_afData[1][3] = 0.0f;
    m_afData[2][0] = 0.0f;
    m_afData[2][1] = 0.0f;
    m_afData[2][2] = 1.0f;
    m_afData[2][3] = 0.0f;
    m_afData[3][0] = 0.0f;
    m_afData[3][1] = 0.0f;
    m_afData[3][2] = 0.0f;
    m_afData[3][3] = 1.0f;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::Translate definition

void Transform::Translate(float fX, float fY, float fZ)
{
    Transform  cTemp;

    cTemp.m_afData[3][0] = fX;
    cTemp.m_afData[3][1] = fY;
    cTemp.m_afData[3][2] = fZ;

    *this = cTemp * *this;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::Rotate definition

void Transform::Rotate(float fAngle, char chAxis)
{
    Transform cRotMatrix;

    float fCosine = VectorCosD(fAngle);
    float fSine = VectorSinD(fAngle);

    switch (chAxis)
    {
    case 'x': case 'X':
        cRotMatrix.m_afData[1][1] = fCosine;
        cRotMatrix.m_afData[1][2] = fSine;
        cRotMatrix.m_afData[2][1] = -fSine;
        cRotMatrix.m_afData[2][2] = fCosine;
        break;
    case 'y': case 'Y':
        cRotMatrix.m_afData[0][0] = fCosine;
        cRotMatrix.m_afData[0][2] = -fSine;
        cRotMatrix.m_afData[2][0] = fSine;
        cRotMatrix.m_afData[2][2] = fCosine;
        break;
    case 'z': case 'Z':
        cRotMatrix.m_afData[0][0] = fCosine;
        cRotMatrix.m_afData[1][0] = -fSine;
        cRotMatrix.m_afData[0][1] = fSine;
        cRotMatrix.m_afData[1][1] = fCosine;
        break;
    }

    *this = cRotMatrix * *this;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::RotateAxis definition

void Transform::RotateAxis(float fAngle, const Vec3& cAxis)
{
	Vec3 cNormAxis = cAxis;
	cNormAxis.Normalize( );

    float s = VectorSinD(fAngle);
    float c = VectorCosD(fAngle);
    float t = 1.0f - c;

    float x = cNormAxis.m_afData[0];
    float y = cNormAxis.m_afData[1];
    float z = cNormAxis.m_afData[2];

    Transform  cRotMatrix;
    cRotMatrix.m_afData[0][0] = t * x * x + c;
    cRotMatrix.m_afData[0][1] = t * x * y + s * z;
    cRotMatrix.m_afData[0][2] = t * x * z - s * y;
    cRotMatrix.m_afData[0][3] = 0.0;
    cRotMatrix.m_afData[1][0] = t * x * y - s * z;
    cRotMatrix.m_afData[1][1] = t * y * y + c;
    cRotMatrix.m_afData[1][2] = t * y * z + s * x;
    cRotMatrix.m_afData[1][3] = 0.0;
    cRotMatrix.m_afData[2][0] = t * x * z + s * y;
    cRotMatrix.m_afData[2][1] = t * y * z - s * x;
    cRotMatrix.m_afData[2][2] = t * z * z + c;
    cRotMatrix.m_afData[2][3] = 0.0;
    cRotMatrix.m_afData[3][0] = 0.0;
    cRotMatrix.m_afData[3][1] = 0.0;
    cRotMatrix.m_afData[3][2] = 0.0;
    cRotMatrix.m_afData[3][3] = 1.0;

    *this = cRotMatrix * *this;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::IsIdentity definition

bool Transform::IsIdentity(void) const
{
    Transform cId;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (cId.m_afData[i][j] != m_afData[i][j])
                return false;

    return true;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::Scale definition

void Transform::Scale(float fX)
{
    Transform  cTemp;

    cTemp.m_afData[0][0] = fX;
    cTemp.m_afData[1][1] = fX;
    cTemp.m_afData[2][2] = fX;

    *this = cTemp * *this;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::Scale definition

void Transform::Scale(float fX, float fY, float fZ)
{
    Transform  cTemp;

    cTemp.m_afData[0][0] = fX;
    cTemp.m_afData[1][1] = fY;
    cTemp.m_afData[2][2] = fZ;

    *this = cTemp * *this;
}


///////////////////////////////////////////////////////////////////////  
//  Transform::operator* definition

Transform  Transform::operator*(const Transform& cTransform) const
{
    Transform  cTemp;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
        {
            cTemp.m_afData[i][j] = 0.0;
            for (int k = 0; k < 4; ++k)
                cTemp.m_afData[i][j] += m_afData[i][k] * cTransform.m_afData[k][j];
        }

    return cTemp;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::operator* definition

Vec3 Transform::operator*(const Vec3& cVec) const
{
    const int nSize = 3;//cVec.GetSize( );
    Vec3 cTemp;

    for (int i = 0; i < nSize; ++i)
    {
        cTemp.m_afData[i] = 0.0f;
        for (int j = 0; j < 4; ++j)
            if (j < nSize)
                    cTemp.m_afData[i] += m_afData[i][j] * cVec.m_afData[j];
            else if (j == 3)
                    cTemp.m_afData[i] += m_afData[i][j];
    }

    return cTemp;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::operator* definition

Region Transform::operator*(const Region& cRegion) const
{
    Region cTemp;

    cTemp.m_cMin = *this * cRegion.m_cMin;
    cTemp.m_cMax = *this * cRegion.m_cMax;

    return cTemp;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::operator+ definition

Transform Transform::operator+(const Transform& cTransform) const
{
    Transform  cTemp;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            cTemp.m_afData[i][j] = cTransform.m_afData[i][j] + m_afData[i][j];

    return cTemp;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::operator- definition

Transform Transform::operator-(const Transform& cTransform) const
{
    Transform  cTemp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            cTemp.m_afData[i][j] = m_afData[i][j] - cTransform.m_afData[i][j];

    return cTemp;
}
    

///////////////////////////////////////////////////////////////////////  
//  Transform::LookAt definition

void Transform::LookAt(const Vec3& cEye, const Vec3& cCenter, const Vec3& cUp)
{
    Vec3 cF = cCenter - cEye;
    cF.Normalize( );

    Vec3 cUpPrime = cUp;
    cUpPrime.Normalize( );

    Vec3 cS = cF.Cross(cUpPrime);
    Vec3 cU = cS.Cross(cF);

    Transform cTemp;
    cTemp.m_afData[0][0] = cS[0];
    cTemp.m_afData[1][0] = cS[1];
    cTemp.m_afData[2][0] = cS[2];

    cTemp.m_afData[0][1] = cU[0];
    cTemp.m_afData[1][1] = cU[1];
    cTemp.m_afData[2][1] = cU[2];

    cTemp.m_afData[0][2] = -cF[0];
    cTemp.m_afData[1][2] = -cF[1];
    cTemp.m_afData[2][2] = -cF[2];

    *this = cTemp * *this;

    Translate(-cEye.m_afData[0], -cEye.m_afData[1], -cEye.m_afData[2]);
}




///////////////////////////////////////////////////////////////////////  
//  Region::Region definition

Region::Region( )
{
	Initialize( );
}


///////////////////////////////////////////////////////////////////////  
//  Region::operator+ definition

Region Region::operator+(const Region& cRegion) const
{
	Region cTemp = *this;

	for (int i = 0; i < 3; i++)
	{
		if (cRegion.m_cMin[i] < m_cMin[i])
			cTemp.m_cMin[i] = cRegion.m_cMin[i];
		if (cRegion.m_cMax[i] > m_cMax[i])
			cTemp.m_cMax[i] = cRegion.m_cMax[i];
	}

	return cTemp;
}


///////////////////////////////////////////////////////////////////////  
//  Region::operator+ definition

Region Region::operator+(const Vec3& cVec) const
{
	Region cTemp = *this;

	cTemp.m_cMin = cTemp.m_cMin + cVec;
	cTemp.m_cMax = cTemp.m_cMax + cVec;

	return cTemp;
}


///////////////////////////////////////////////////////////////////////  
//  Region::operator^ definition

Region Region::operator^(const Vec3& cVec) const
{
	Region cTemp = *this;

	if (cVec[0] < cTemp.m_cMin[0])
		cTemp.m_cMin[0] = cVec[0];
	if (cVec[1] < cTemp.m_cMin[1])
		cTemp.m_cMin[1] = cVec[1];
	if (cVec[2] < cTemp.m_cMin[2])
		cTemp.m_cMin[2] = cVec[2];

	if (cVec[0] > cTemp.m_cMax[0])
		cTemp.m_cMax[0] = cVec[0];
	if (cVec[1] > cTemp.m_cMax[1])
		cTemp.m_cMax[1] = cVec[1];
	if (cVec[2] > cTemp.m_cMax[2])
		cTemp.m_cMax[2] = cVec[2];

	return cTemp;
}


///////////////////////////////////////////////////////////////////////  
//  Region::operator* definition

Region Region::operator*(float fScale) const
{
	Region cTemp = *this;
	Vec3 cMidpoint = Midpoint( );

	for (int i = 0; i < 3; i++)
	{
		float fDistance = (m_cMax[i] - m_cMin[i]) * 0.5F;
		cTemp.m_cMin[i] = cMidpoint[i] - fDistance * fScale;
		cTemp.m_cMax[i] = cMidpoint[i] + fDistance * fScale;
	}

	return cTemp;
}


///////////////////////////////////////////////////////////////////////  
//  Region::operator* definition

Region Region::operator*(const Transform& cTransform) const
{
	Region cTemp;

	cTemp.m_cMin = m_cMin * cTransform;
	cTemp.m_cMax = m_cMax * cTransform;

	return cTemp;
}


///////////////////////////////////////////////////////////////////////  
//  Region::operator< definition

bool Region::operator<(const Region& cRegion) const
{
	bool bReturn = true;
	for (int i = 0; i < 3; i++)
		if (m_cMin[i] <= cRegion.m_cMin[i] || m_cMax[i] >= cRegion.m_cMax[i])
		{
			bReturn = false;
			break;
		}

		return bReturn;
}


///////////////////////////////////////////////////////////////////////  
//  Region::AverageExtent definition

float Region::AverageExtent( ) const
{
	float fSum = 0.0f;
	for (int i = 0; i < 3; i++)
		fSum += m_cMax[i] - m_cMin[i];

	return 0.33333333f * fSum;
}


///////////////////////////////////////////////////////////////////////  
//  Region::Initialized definition

void Region::Initialize( )
{
	m_cMin[0] = m_cMin[1] = m_cMin[2] = FLT_MAX;
	m_cMax[0] = m_cMax[1] = m_cMax[2] = -FLT_MAX;
}


///////////////////////////////////////////////////////////////////////  
//  Region::IsSet definition

bool Region::IsSet( ) const
{
	return (m_cMin[0] != FLT_MAX || 
		m_cMin[1] != FLT_MAX || 
		m_cMin[2] != FLT_MAX || 
		m_cMax[0] != -FLT_MAX || 
		m_cMax[1] != -FLT_MAX || 
		m_cMax[2] != -FLT_MAX);
}


///////////////////////////////////////////////////////////////////////  
//  Region::Scale definition

void Region::Scale(float fScale)
{
	m_cMin = m_cMin * fScale;
	m_cMax = m_cMax * fScale;
}

