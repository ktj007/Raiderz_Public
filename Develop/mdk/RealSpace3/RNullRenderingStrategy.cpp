#include "StdAfx.h"
#include "RNullRenderingStrategy.h"
#include "REngine.h"
#include "RRenderHelper.h"

namespace rs3 {


RNullRenderingStrategy::RNullRenderingStrategy()
{

}

RNullRenderingStrategy::~RNullRenderingStrategy()
{

}

void RNullRenderingStrategy::Init( int nW, int nH )
{
	m_dwWidth = nW; m_dwHeight = nH; 
}


}