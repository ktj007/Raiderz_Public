#pragma once

namespace rs3 {

/// 신의 관리를 담당한다
class REngine;
class RS_API RSceneLoader
{

public:
	explicit RSceneLoader(REngine* pEngine){}
	virtual ~RSceneLoader(){}
};

}
