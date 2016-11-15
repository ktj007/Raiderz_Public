#pragma once

namespace EffectTool_NET {
	ref class MainForm;
}

ref class GlobalObjects
{
public:
	static EffectTool_NET::MainForm^	g_pMainForm;
	static System::Windows::Forms::TrackBar^ g_pTrackBar;
	static FLOAT g_fTrackBarValue;
};