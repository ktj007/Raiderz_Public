#pragma once

class CSBasicAttributeCalculator
{
public:
	// ½ºÅÈ °ü·Ã
	static int CalcMaxHP(int nLevel, int nCON);
	static int CalcMaxEN(int nLevel, int nINT);
	static int CalcMaxSP(int nLevel, int nDEX);
};