#include "stdafx.h"
#include "XNet.h"

XNet XNet::singleton;

XNet::XNet()
{
	MakeWorkers(4);
}

XNet::~XNet()
{
}
