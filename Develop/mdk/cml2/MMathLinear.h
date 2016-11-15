#ifndef _MMATHLINEAR_H_
#define _MMATHLINEAR_H_

#include "MVector3.h"
#include "MMatrix3.h"


class MMathLinear
{
	// gpg 4 - chapter 5.15 의 내용
private:
	static void solveLinearSystem(MVector3 *pX, const MMatrix3 &A, const MVector3 &B);

public:
	// 최소제곱합
	struct LeastSquareSums
	{
	public:

		float x, y, z;
		float xx, xy, xz;
		float yy, yz;
		int samples;
	
	public:
		LeastSquareSums();
		void clear();

		LeastSquareSums& operator +=(LeastSquareSums &other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			xx += other.xx;
			xy += other.xy;
			xz += other.xz;
			yy += other.yy;
			yz += other.yz;
			samples += other.samples;

			return *this;
		}
	};

	// 최소제곱 합 slow version
	static void calculateLeastSquareSums(LeastSquareSums *pSums, const MVector3 *pVertices, int nVectorCount);

	// 최소제곱 평면 slow version
	static void calculateLeastSquarePlane(const LeastSquareSums &sums, MVector3 &normal, MVector3 &center);

	// 여기 까지 gpg 4 - 5.15의 코드
};

#endif