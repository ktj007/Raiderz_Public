#ifndef	_M_AUTO_TEST_INFO_H_
#define	_M_AUTO_TEST_INFO_H_

#include <string>
using namespace std;

class MAutoTestInfo
{
public:		
	string			strTestType;	///< 테스트 종류	
	int				nRepeat;		///< 반복 회수
	DWORD			dwTime;			///< 한회 반복 할때의 종료 시간
	int				nSpeed;			///< Update 속도

	float			fLimitFrame;	///< 제한 프레임(이 값보다 작으면 실패)

public:
	MAutoTestInfo();
	~MAutoTestInfo();
};

#endif
