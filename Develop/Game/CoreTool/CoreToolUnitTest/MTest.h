#pragma once

#include "UnitTest++.h"
#include "MUnitTestRunner.h"
#include "MBaseUnitTest.h"

// 실패 시, TEST 종료
#define ASSERT_CHECK(value)	\
	do \
{ \
	int nFailureCount = UnitTest::CurrentTest::Results()->GetFailureCount();	\
	try { \
	if (!UnitTest::Check(value)) \
	UnitTest::CurrentTest::Results()->OnTestFailure( UnitTest::TestDetails(m_details, __LINE__), #value); \
} \
	catch (...) { \
	UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(m_details, __LINE__), \
	"Unhandled exception in CHECK(" #value ")"); \
} \
	if (nFailureCount < UnitTest::CurrentTest::Results()->GetFailureCount())	return;	\
} while (0)


// 실패 시, TEST 종료
#define ASSERT_EQUAL(expected, actual)	\
	do \
{ \
	int nFailureCount = UnitTest::CurrentTest::Results()->GetFailureCount();	\
	try { \
	UnitTest::CheckEqual(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(m_details, __LINE__)); \
} \
	catch (...) { \
	UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(m_details, __LINE__), \
	"Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")"); \
} \
	if (nFailureCount < UnitTest::CurrentTest::Results()->GetFailureCount())	return;	\
} while (0)

// CHECK_CLOSE에서 tolerance를 0.001로 설정
#define CHECK_CLOSE2(expected, actual) \
	do \
	{ \
	try { \
	UnitTest::CheckClose(*UnitTest::CurrentTest::Results(), expected, actual, 0.001f, UnitTest::TestDetails(m_details, __LINE__)); \
		} \
		catch (...) { \
		UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(m_details, __LINE__), \
		"Unhandled exception in CHECK_CLOSE(" #expected ", " #actual ")"); \
		} \
	} while (0)
