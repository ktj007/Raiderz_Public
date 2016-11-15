#include "stdafx.h"
#include "MXml.h"
#include "RTypes.h"
//#include "RNameSpace.h"
#include "RD3DApplication.h"
#include "MWLua.h"
#include "../util/Mint4R3.h"
#include "MWidget.h"
#include "Mint.h"
#include "MIDLResource.h"
#include "MButton.h"
#include "MEdit.h"
#include "MFrame.h"
#include "MMsgBox.h"
#include "MintLoader.h"

#include "shlwapi.h"

#include "RConfiguration.h"
#include "RGlobal.h"
#include "RsceneManager.h"

#include "MyApp.h"

#ifdef _DEBUG
#pragma comment(lib,"../../../cml2/Lib/cml2d.lib")
#pragma comment(lib,"../../../RealSpace3/lib/RealSpace3d.lib")
#pragma comment(lib, "../../../../../sdk/lua51/lib/vc8/lua51d.lib")
#pragma comment(lib, "../../../../../sdk/tinyxml/lib/tinyxmld_STL.lib")
#else
#pragma comment(lib,"../../../cml2/Lib/cml2.lib")
#pragma comment(lib,"../../../RealSpace3/lib/RealSpace3.lib")
#pragma comment(lib, "../../../../../sdk/lua50/lib/lua50.lib")
#pragma comment(lib, "../../../../../sdk/tinyxml/lib/tinyxml_STL.lib")
#endif

#pragma comment(lib, "../../../../../sdk/zlib/lib/zlib.lib")

#pragma comment(lib,"winmm.lib")
//#pragma comment(lib,"dxguid.lib")

MyApp g_App;

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	return g_App.Run();
}