#pragma once


namespace mint3 {

class MScriptHelper
{
public:
	MScriptHelper() {}
	virtual ~MScriptHelper() {}

	// Draw °ü·Ã
	// »ö±ò
	BYTE m_a;		// ¾ËÆÄ
	BYTE m_r;		// »¡°­
	BYTE m_g;		// ³ì»ö
	BYTE m_b;		// ÆÄ¶û

	static void glueDrawLine();//int sx, int sy, int tx);//, int ty, unsigned int color);
	void glueDrawBox(int sx, int sy, int tx, int ty);
	void glueFillBox(int sx, int sy, int tx, int ty);
};

} // namespace mint3
