#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#include <process.h>
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable:4786)

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
#include <map>
using namespace std;



// cml
#include "MUID.h"
#include "MDebug.h"

#include "MiNetLib.h"
#include "MiNetLogger.h"