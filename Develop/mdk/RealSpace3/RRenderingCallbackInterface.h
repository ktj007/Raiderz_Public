#pragma once


namespace rs3 {


class RS_API RRenderingCallbackInterface
{
public:
	RRenderingCallbackInterface(void);
	virtual ~RRenderingCallbackInterface(void);

	virtual void DeviceSetting()	{} ///< 전 패스에 영향을 미치는 디바이스 셋팅을 해준다. G 스테이지 시작전에 호출됨.
	virtual void PreOrderRender()	{}
	virtual void PostOrderRender()	{}
};


}
