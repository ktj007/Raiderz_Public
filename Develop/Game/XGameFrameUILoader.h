#pragma once

/// 게임 프레임의 UI를 로딩하는 역할을 한다.
class XGameFrameUILoader
{
private:
	map<wstring, wstring>			m_FrameDescriptorList;
	void AddFrameList(wstring strFrameName, wstring strFileName);
	bool LoadFrameDescriptor();
public:
	XGameFrameUILoader(){}
	~XGameFrameUILoader(){}
	bool Load(const wchar_t* szGameFrameName=NULL, bool bReload=false);
};