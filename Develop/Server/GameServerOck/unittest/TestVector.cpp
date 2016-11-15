#include "stdafx.h"
#include "MTypes.h"

SUITE(Vector)
{
	TEST(TestSVectorPrecision1)
	{
		svec3	sv1;

		sv1.SetX(0.0f);
		sv1.SetY(0.0f);
		sv1.SetZ(0.0f);

		CHECK_CLOSE(0.0f, sv1.X(), 0.001f);
		CHECK_CLOSE(0.0f, sv1.Y(), 0.001f);
		CHECK_CLOSE(0.0f, sv1.Z(), 0.001f);
	}

	TEST(TestSVectorPrecision2)
	{
		svec3	sv1;

		sv1.SetX(10.0f);
		sv1.SetY(10.5f);
		sv1.SetZ(11.2f);

		CHECK_CLOSE(10.0f, sv1.X(), 0.001f);
		CHECK_CLOSE(10.5f, sv1.Y(), 0.001f);
		CHECK_CLOSE(11.2f, sv1.Z(), 0.001f);

		sv1.SetX(-10.0f);
		sv1.SetY(-10.5f);
		sv1.SetZ(-11.2f);

		CHECK_CLOSE(-10.0f, sv1.X(), 0.001f);
		CHECK_CLOSE(-10.5f, sv1.Y(), 0.001f);
		CHECK_CLOSE(-11.2f, sv1.Z(), 0.001f);

	}

	TEST(TestSVectorPrecision3)
	{
		svec3	sv1;

		sv1.SetX(0.0001f);
		sv1.SetY(0.1234f);
		sv1.SetZ(0.5678f);

		CHECK_CLOSE(0.0001f, sv1.X(), 0.001f);
		CHECK_CLOSE(0.1234f, sv1.Y(), 0.001f);
		CHECK_CLOSE(0.5678f, sv1.Z(), 0.001f);

		// 

		sv1.SetX(0.1542f);
		sv1.SetY(0.9999f);
		sv1.SetZ(0.9998f);

		CHECK_CLOSE(0.1542f, sv1.X(), 0.001f);
		CHECK_CLOSE(0.9999f, sv1.Y(), 0.001f);
		CHECK_CLOSE(0.9998f, sv1.Z(), 0.001f);

	}

}