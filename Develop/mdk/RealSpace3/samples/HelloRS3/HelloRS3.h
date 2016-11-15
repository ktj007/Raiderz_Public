#pragma once

#ifdef _DEBUG
#pragma comment (lib,"../../../cml2/lib/cml2d.lib")
#pragma comment (lib,"../../../../../sdk/tinyxml/lib/tinyxmld_STL.lib")

#else
#pragma comment (lib,"../../../cml2/lib/cml2.lib")
#pragma comment (lib,"../../../../../sdk/tinyxml/lib/tinyxml_STL.lib")

#endif

#pragma comment (lib,"../../../../../sdk/zlib/lib/zlib.lib")
