#pragma once

namespace rs3 {

/// 메모리 풀을 이 클래스의 소멸자에서 몰아서 해제합니다.
class RMemPoolReleaser
{
public:
	RMemPoolReleaser() {}
	~RMemPoolReleaser();
};

}